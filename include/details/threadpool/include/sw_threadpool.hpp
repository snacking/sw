// header file for sw::threadpool

#ifndef _SW_THREADPOOL_H_
#define _SW_THREADPOOL_H_

#include "../../sw_vals.h"

#include "./sw_thread.h"
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
#include <condition_variable>

_SW_BEGIN

struct threadpool_settings {
	using size_type = ::std::size_t;

	threadpool_settings();
	size_type worker_capacity, core_capacity, queue_capacity;
	int keepalive_time;
	queue_type queue;
	handler_type handler;
};

/*
	threadpool(
		worker capacity(worker thread's maximum size),
		core_capacity(core thread(never expire)'s maximum size),
		queue(queue to store tasks),
		queue capacity(queue's maximum size),
		keepalive time(spare thread's maximum keepalive time(ms), negative value means never expire),
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

	threadpool() = delete;

	explicit threadpool(threadpool_settings);

	threadpool(const threadpool&) = delete;

	threadpool(threadpool&&) = delete;

	threadpool& operator=(const threadpool&) = delete;

	threadpool& operator=(threadpool&&) = delete;

	~threadpool();

	template <typename _Fn, typename... _Args>  
    ::std::future<typename ::std::invoke_result<_Fn, _Args...>::type> submit(_Fn&&, _Args&&...);

	template <typename _Fn, typename... _Args>  
    void execute(_Fn&&, _Args&&...);

	void shutdown();

	::std::vector<task_type> shutdown_now();

	void join();

	inline size_type size() const;

	inline ::std::tuple<size_type, size_type, size_type> statistics() const;
private:
	_Queue_base *_Queue_create_helper(queue_type, size_type);

	::std::unique_ptr<_Reject_handler_base> _Reject_handler_create_helper(handler_type);

	void _Init();

	inline void _Close();

	template <typename _Fn, typename... _Args>
	inline void _Create_and_push_task(::std::shared_ptr<::std::promise<typename ::std::invoke_result<_Fn, _Args...>::type> >, _Fn&&, _Args&&...);

	struct _Statistics {
		size_type total_submissions_ = 0, total_rejected_ = 0;
		atomic_type total_completions_ = 0;
	} info_;

	enum class _State : ::std::uint8_t {
		RUNNING,
		STOPPING,
		TERMINATED
	} state_;

	size_type worker_capacity_, core_capacity_;
	int keepalive_time_;
	::std::vector<_Thread_base*> pthreads_;
	::std::mutex mutex_;
	::std::condition_variable cv_;
	_Queue_base *pqueue_;
};

_SW_END // _SW_BEGIN

#include "./sw_threadpool.ipp"

#ifdef _SW_HEADER_ONLY_
	#include "../src/sw_threadpool.cc"
#endif // _SW_HEADER_ONLY_

#endif // _SW_THREADPOOL_H_
