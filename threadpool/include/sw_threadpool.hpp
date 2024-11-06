// header file for sw::threadpool

#ifndef _SW_THREADPOOL_H_
#define _SW_THREADPOOL_H_

#include "../../include/sw_vals.h"
#include "./sw_epoll_core.h"
#include "./sw_queue_core.h"
#include "./sw_task_core.hpp"
#include <future>
#include <utility>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cstddef>
#include <memory>
#include <vector>
#include <cassert>
#include <chrono>
#include <tuple>
#include <atomic>
#include <stdexcept>

_SW_BEGIN

class _Thread_base {
public:
	using thread_id = ::std::thread::id;

	_Thread_base() : 
		state_(_State::IDLE) {}

	virtual ~_Thread_base() = default; // nothing

	virtual void start() = 0;

	virtual void stop() = 0;

	thread_id get_thread_id() const noexcept {
		return id_;
	}
protected:
	virtual void run() = 0;

	enum class _State : ::std::uint8_t {
		IDLE,
		RUNNING,
		SLEEPING,
		STOPPING,
		STOPPED
	} state_;

	::std::thread thread_;
	thread_id id_;
};

class _Worker;
class _Leader;

/*
	threadpool(
		worker capacity(worker thread's maximum size),
		queue(queue to store tasks),
		queue capacity(queue's maximum size),
		keepalive time(spare thread's maximum keepalive time),
		handler(reject handler)
	)
*/
class threadpool : 
	public ::std::enable_shared_from_this<threadpool> {
public:
	friend class _Leader;
	friend class _Worker;

	using size_type = ::std::size_t;
	using task_type = ::std::unique_ptr<_Task_base>;
	using atomic_type = ::std::atomic<size_type>;

	threadpool(size_type worker_capacity = 1, queue_type queue = QUEUE_TYPE_FIFO, size_type queue_capacity = 1024, 
		size_t keepalive_time = 1000, handler_type handler = HANDLER_TYPE_IGNORE) : 
			worker_capacity_(worker_capacity), pthreads_(worker_capacity), pqueue_(_Queue_create_helper(queue, queue_capacity)),
				keepalive_time_(keepalive_time) {
					pqueue_->set_handler(_Reject_handler_create_helper(handler));
					_Init();
				}

	threadpool(const threadpool&) = delete;

	threadpool(threadpool&&) = delete;

	threadpool& operator=(const threadpool&) = delete;

	threadpool& operator=(threadpool&&) = delete;

	~threadpool() {
		_Close();
	}

	template <typename _Fn, typename... _Args>  
    ::std::future<typename ::std::invoke_result<_Fn, _Args...>::type> submit(_Fn&& fn, _Args&&... args) {
		static_assert(!::std::is_same<typename ::std::invoke_result<_Fn, _Args...>::type, void>::value, "return type not supported");
        auto _Promise = ::std::make_shared<::std::promise<typename ::std::invoke_result<_Fn, _Args...>::type> >();   
        _Create_and_push_task(std::forward<_Fn>(fn), std::forward<_Args>(args)..., _Promise);
        return _Promise->get_future();
    }

	template <typename _Fn, typename... _Args>  
    void execute(_Fn&& fn, _Args&&... args) {
		static_assert(::std::is_same<typename ::std::invoke_result<_Fn, _Args...>::type, void>::value, "return type not supported");
        auto _Promise = ::std::make_shared<::std::promise<void>();  
        _Create_and_push_task(std::forward<_Fn>(fn), std::forward<_Args>(args)..., _Promise);
        return;
    }

	void shutdown() {
		state_ = _State::STOPPING;
		while (!pqueue_->empty()) {
			::std::this_thread::sleep_for(::std::chrono::milliseconds(10));
		}
		_Close();
		return;
	}

	::std::vector<task_type> shutdown_now() {
		state_ = _State::STOPPING;
		_Close();
		::std::vector<task_type> _Remains;
		while (!pqueue_->empty()) {
			auto _Task = pqueue_->pop();
			_Remains.emplace_back(::std::move(_Task));
		}
		return _Remains;
	}

	inline size_type size() const {
		return pqueue_->size();
	}
private:
	::std::unique_ptr<_Queue_base> _Queue_create_helper(queue_type type, size_type capacity) {
		switch (type) {
		case QUEUE_TYPE_FIFO:
			return ::std::make_unique<_Queue>(capacity);
		case QUEUE_TYPE_PRIORITY:
			return ::std::make_unique<_Queue_priority>(capacity);
		}
		return ::std::make_unique<_Queue>(capacity);
	}

	::std::unique_ptr<_Reject_handler_base> _Reject_handler_create_helper(handler_type type) {
		switch (type) {
		case HANDLER_TYPE_IGNORE:
			return ::std::make_unique<_Reject_handler_ignore>();
		case HANDLER_TYPE_IGNORE_THROW:
			return ::std::make_unique<_Reject_handler_ignore_throw>();
		case HANDLER_TYPE_DELETE_OLDEST:
			return ::std::make_unique<_Reject_handler_delete_oldest>();
		}
		return ::std::make_unique<_Reject_handler_ignore>();
	}

	void _Init() {
		auto _Self = shared_from_this();
		leader_ = ::std::make_unique<_Leader>(_Self);
		for (size_type i = 0; i < worker_capacity_; ++i) {
			pthreads_.emplace_back(::std::make_unique<_Worker>(_Self));
			pthreads_[i]->start();
		}
		return;
	}

	void _Close() {
		leader_->stop();
		state_ = _State::TERMINATED;
		return;
	}

	template <typename _Fn, typename... _Args>
	inline void _Create_and_push_task(_Fn&& fn, _Args&&... args, ::std::shared_ptr<::std::promise<typename ::std::invoke_result<_Fn, _Args...>::type> > ppromise) {
		static_assert(::std::is_invocable<_Fn, _Args...>::value, "function not callable");
		auto _Task = ::std::make_unique<task<_Fn, _Args...> >(
			::std::forward<_Fn>(fn), 
			::std::forward_as_tuple(::std::forward<_Args>(args)...),
			ppromise
		);  
        try {
			pqueue_->push(::std::move(_Task));
			info_.total_submissions_++;
			cv_.notify_one();
		} catch (...) {
			info_.total_rejected_++;
		}
		return;
	}

	struct _Statistics {
		size_type total_submissions_, total_rejected_;
		atomic_type total_completions_;
	} info_;

	enum class _State : ::std::uint8_t {
		RUNNING,
		STOPPING,
		TERMINATED
	} state_;

	size_type worker_capacity_, keepalive_time_;
	::std::unique_ptr<_Leader> leader_;
	::std::vector<::std::unique_ptr<_Thread_base> > pthreads_;
	::std::unique_ptr<_Queue_base> pqueue_;
	::std::mutex mutex_;
	::std::conditional_variable cv_;
};

class _Leader final : 
	public _Thread_base {
public:
	_Leader(::std::shared_ptr<threadpool> ptp) : 
		_Thread_base(), ptp_(ptp) {}

	~_Leader() = default; // nothing
	
	void start() override {
		thread_ = ::std::thread(&_Leader::run, this);
		id_ = ::std::this_thread::get_id();
		state_ = _State::RUNNING;
		return;
	}

	inline void stop() override {
		state_ = _State::STOPPING;
		for (auto& worker : ptp_->pthreads_) {
			worker->stop();
		}
        if (thread_.joinable()) {
            thread_.join();
        }
        state_ = _State::STOPPED;
		return;
	}
private:
	void run() override {
		auto _Ptp = ptp_.lock();
		while (state_ != _State::STOPPING && state_ != _State::STOPPED) {
			auto _Start = ::std::chrono::high_resolution_clock::now();
			while (::std::chrono::high_resolution_clock::now() - _Start < ::std::chrono::milliseconds(ptp_->keepalive_time_));
			for (auto &worker : ptp_->pthreads_) {
				if (worker->state_ == _Worker::_State::SLEEPING) {
					// dynamic adjust
				}
			}
		}
	}

	::std::weak_ptr<threadpool> ptp_;
};

class _Worker final : 
	public _Thread_base {
public:
	_Worker(::std::shared_ptr<threadpool> ptp) : 
		_Thread_base(), ptp_(ptp) {}

	~_Worker() = default; // nothing

	void start() override {
		thread_ = ::std::thread(&_Worker::run, this);
		id_ = ::std::this_thread::get_id();
		state_ = _State::IDLE;
		return;
	}

	inline void stop() override {
		state_ = _State::STOPPING;
		if (thread_.joinable()) {
            thread_.join();
        }
        state_ = _State::STOPPED;
		return;
	}
private:
	void run() override {
		auto _Ptp = ptp_.lock();
		::std::unique_ptr<_Task_base> _Task;
		while (state_ != _State::STOPPING && state_ != _State::STOPPED) {
			auto _Start = ::std::chrono::high_resolution_clock::now();
			std::unique_lock<std::mutex> lock(_Ptp->mtx_);
			_Ptp->cv_.wait(lock, [this, _Ptp] {
				return !_Ptp->pqueue_->empty() || 
				(::std::chrono::high_resolution_clock::now() - _Start > ::std::chrono::milliseconds(_Ptp->keepalive_time_)) ||
				(state_ == _State::STOPPING || state_ == _State::STOPPED);
			});
			if (::std::chrono::high_resolution_clock::now() - _Start > ::std::chrono::milliseconds(_Ptp->keepalive_time_)) {
				state_ = _State::SLEEPING;
				while (true); // to do: wait leader to wake
			} else {
				state_ = _State::RUNNING;
				_Task = _Ptp->pqueue_->pop();
				_Task->execute();
				_Ptp->info_.total_completions_.fetch_add(1);
				_Task.reset();
			}
		}
	}

	::std::weak_ptr<threadpool> ptp_;
};

_SW_END // _SW_BEGIN

#endif // _SW_THREADPOOL_H_
