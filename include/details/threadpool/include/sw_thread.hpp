// header file for sw::_Thread_base and derived (internal)

#ifndef _SW_THREAD_H_
#define _SW_THREAD_H_

#include "../../time/include/sw_time.hpp"

#include "../../sw_vals.h"
#include "sw_task_core.hpp"
#include <memory>
#include <thread>
#include <cstddef>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>

_SW_BEGIN

class threadpool;

class _Thread_base {
public:
	using ptr = ::std::shared_ptr<_Thread_base>;
	using threadpool_ptr = ::std::weak_ptr<threadpool>;
	using thread_id = ::std::thread::id;

	_Thread_base(threadpool_ptr);

	virtual ~_Thread_base() = default; // nothing

	virtual void start() = 0;

	virtual void stop() = 0;

	enum class _State : ::std::uint8_t;

	_State state() ;

	inline thread_id get_thread_id() const;
protected:
	virtual void run() = 0;

	enum class _State : ::std::uint8_t {
		IDLE,
		RUNNING,
		SLEEPING,
		STOPPING,
		STOPPED
	} state_;

#ifdef _SW_DEBUG_
	constexpr static const char *state_str[] = {
		"IDLE",
		"RUNNING",
		"SLEEPING",
		"STOPPING",
		"STOPPED"
	};

	const char *to_state_str(enum _State s) const {
		return state_str[static_cast<::std::uint8_t>(s)];
	}
#endif // _SW_DEBUG_

	::std::thread thread_;
	thread_id id_;
	threadpool_ptr ptp_;
	::std::shared_mutex mutex_; // state mutex
};

class _Leader final : 
	public _Thread_base {
public:
	_Leader(threadpool_ptr);

	~_Leader() = default; // nothing
	
	void start() override;

	void stop() override;
private:
	void run() override;

	::std::mutex ct_mutex_; // time counter helper mutex
	::std::condition_variable ct_cv_; // time counter helper cv
};

class _Worker final : 
	public _Thread_base {
public:
	_Worker(threadpool_ptr, bool);

	~_Worker() = default; // nothing

	void start() override;

	void stop() override;

	void sleep();
private:
	void run() override;

	bool is_core_;
};

_SW_END // _SW_BEGIN

#include "sw_thread.ipp"

#ifdef _SW_HEADER_ONLY_
	#include "../src/sw_thread.cc"
#endif // _SW_HEADER_ONLY_

#endif // _SW_THREAD_H_
