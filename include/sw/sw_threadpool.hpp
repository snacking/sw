// header file for sw::threadpool

#ifndef _SW_THREADPOOL_H_
#define _SW_THREADPOOL_H_

#include "sw/internal/sw_vals.h"
#include "sw/internal/sw_queue_core.hpp"
#include "sw/internal/sw_task_core.hpp"
#include "sw/internal/sw_thread.hpp"

#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <future>
#include <memory>
#include <stdexcept>
#include <thread>
#include <utility>
#include <tuple>
#include <vector>

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
class threadpool : public ::std::enable_shared_from_this<threadpool> {
public:
	friend class _Leader;
	friend class _Worker;

	using ptr = ::std::shared_ptr<threadpool>;
	using wptr = ::std::weak_ptr<threadpool>;
	using size_type = ::std::size_t;
	using atomic_type = ::std::atomic<size_type>;
	using task_ptr = _Task_base::uptr;
	using queue_ptr = _Queue_base::ptr;
	using thread_ptr = _Thread_base::ptr;
	using handler_ptr = _Reject_handler_base::uptr;

	static ptr create(threadpool_settings) _SW_NOEXCEPT;

	threadpool() = delete;

	threadpool(const threadpool&) = delete;

	threadpool(threadpool&&) = delete;

	threadpool& operator=(const threadpool&) = delete;

	threadpool& operator=(threadpool&&) = delete;

	~threadpool() _SW_NOEXCEPT;

	template <typename _Fn, typename... _Args>  
    ::std::future<typename ::std::invoke_result<_Fn, _Args...>::type> submit(_Fn&&, _Args&&...) _SW_NOEXCEPT;

	template <typename _Fn, typename... _Args>  
    void execute(_Fn&&, _Args&&...) _SW_NOEXCEPT;

	void shutdown() _SW_NOEXCEPT;

	::std::vector<task_ptr> shutdown_now() _SW_NOEXCEPT;

	void join() _SW_NOEXCEPT;

	inline size_type size() const _SW_NOEXCEPT;

	inline ::std::tuple<size_type, size_type, size_type> statistics() const _SW_NOEXCEPT;
private:
	explicit threadpool(threadpool_settings) _SW_NOEXCEPT;

	queue_ptr _Queue_create_helper(queue_type, size_type) _SW_NOEXCEPT;

	handler_ptr _Reject_handler_create_helper(handler_type) _SW_NOEXCEPT;

	void _Init() _SW_NOEXCEPT;

	inline void _Close() _SW_NOEXCEPT;

	template <typename _Fn, typename... _Args>
	inline void _Create_and_push_task(::std::shared_ptr<::std::promise<typename ::std::invoke_result<_Fn, _Args...>::type> >, _Fn&&, _Args&&...) _SW_NOEXCEPT;

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
	::std::vector<thread_ptr> pthreads_;
	::std::mutex mutex_;
	::std::condition_variable cv_;
	queue_ptr pqueue_;
};

_SW_END // _SW_BEGIN

#include "internal/sw_threadpool.ipp"

#endif // _SW_THREADPOOL_H_
