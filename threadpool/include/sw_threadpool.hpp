// header file for sw::threadpool

#ifndef _SW_THREADPOOL_H_
#define _SW_THREADPOOL_H_

#include "../../include/sw_vals.h"
#include "./sw_thread.h"
#include "./sw_epoll_core.h"
#include "./sw_queue_core.h"
#include "./sw_task_core.hpp"
#include <future>
#include <utility>
#include <thread>
#include <cstddef>
#include <vector>
#include <cassert>
#include <chrono>
#include <tuple>
#include <atomic>
#include <stdexcept>

_SW_BEGIN

/*
	threadpool(
		worker capacity(worker thread's maximum size),
		queue(queue to store tasks),
		queue capacity(queue's maximum size),
		keepalive time(spare thread's maximum keepalive time),
		handler(reject handler)
	)
*/
class threadpool {
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
					assert(worker_capacity_ >= 1);
					pqueue_->set_handler(_Reject_handler_create_helper(handler));
					pthreads_.resize(worker_capacity_ + 1);
					_Init();
				}

	threadpool(const threadpool&) = delete;

	threadpool(threadpool&&) = delete;

	threadpool& operator=(const threadpool&) = delete;

	threadpool& operator=(threadpool&&) = delete;

	~threadpool() {
#ifdef _SW_DEBUG_
    std::cout << "[INFO] threadpool deconstructing\n"; 
#endif // _SW_DEBUG_
		if (state_ == _State::RUNNING) {
			_Close();
		}
		for (int i = 0; i < pthreads_.size(); ++i) {
			delete pthreads_[i];
		}
		delete pqueue_;
#ifdef _SW_DEBUG_
    std::cout << "[INFO] threadpool deconstructed\n"; 
#endif // _SW_DEBUG_
	}

	template <typename _Fn, typename... _Args>  
    ::std::future<typename ::std::invoke_result<_Fn, _Args...>::type> submit(_Fn&& fn, _Args&&... args) {
		static_assert(!::std::is_same<typename ::std::invoke_result<_Fn, _Args...>::type, void>::value, "return type not supported");
        auto _Promise = ::std::make_shared<::std::promise<typename ::std::invoke_result<_Fn, _Args...>::type> >();
        _Create_and_push_task(_Promise, std::forward<_Fn>(fn), std::forward<_Args>(args)...);
        return _Promise->get_future();
    }

	template <typename _Fn, typename... _Args>  
    void execute(_Fn&& fn, _Args&&... args) {
		static_assert(::std::is_same<typename ::std::invoke_result<_Fn, _Args...>::type, void>::value, "return type not supported");
        auto _Promise = ::std::make_shared<::std::promise<typename ::std::invoke_result<_Fn, _Args...>::type> >(); 
        _Create_and_push_task(_Promise, std::forward<_Fn>(fn), std::forward<_Args>(args)...);
        return;
    }

	void shutdown() {
#ifdef _SW_DEBUG_
    std::cout << "[INFO] threadpool stopping\n"; 
#endif // _SW_DEBUG_
		state_ = _State::STOPPING;
		while (!pqueue_->empty()) {
			::std::this_thread::sleep_for(::std::chrono::milliseconds(10));
		}
		_Close();
#ifdef _SW_DEBUG_
    std::cout << "[INFO] threadpool stopped\n"; 
#endif // _SW_DEBUG_
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
	_Queue_base *_Queue_create_helper(queue_type type, size_type capacity) {
		switch (type) {
		case QUEUE_TYPE_FIFO:
			return new _Queue(capacity);
		case QUEUE_TYPE_PRIORITY:
			return new _Queue_priority(capacity);
		}
		return new _Queue(capacity);
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
		for (size_type i = 0; i <= worker_capacity_; ++i) {
			if (i == 0) 
				pthreads_[i] = new _Leader(this);
			else 
				pthreads_[i] = new _Worker(this);
			pthreads_[i]->start();
		}
		return;
	}

	void _Close() {
		pthreads_[0]->stop();
		state_ = _State::TERMINATED;
		return;
	}

	template <typename _Fn, typename... _Args>
	inline void _Create_and_push_task(::std::shared_ptr<::std::promise<typename ::std::invoke_result<_Fn, _Args...>::type> > ppromise, _Fn&& fn, _Args&&... args) {
		static_assert(::std::is_invocable<_Fn, _Args...>::value, "function not callable");
		auto _Task = ::std::make_unique<task<_Fn, _Args...> >(
			ppromise,
			::std::forward<_Fn>(fn), 
			::std::forward_as_tuple(::std::forward<_Args>(args)...)
		);  
        try {
			pqueue_->push(::std::move(_Task));
			info_.total_submissions_++;
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
	::std::vector<_Thread_base*> pthreads_;
	_Queue_base *pqueue_;
};

_SW_END // _SW_BEGIN

#endif // _SW_THREADPOOL_H_
