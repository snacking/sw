// header file for sw::_Thread_base and derived (internal)

#ifndef _SW_THREAD_H_
#define _SW_THREAD_H_

#include "../../include/sw_vals.h"
#include "./sw_task_core.hpp"
#include <thread>
#include <cstddef>
#include <mutex>

_SW_BEGIN

class threadpool;

class _Thread_base {
public:
	using thread_id = ::std::thread::id;

	_Thread_base(threadpool *);

	virtual ~_Thread_base() = default; // nothing

	virtual void start() = 0;

	virtual void stop() = 0;

	enum class _State : ::std::uint8_t;

	_State state() const;

	thread_id get_thread_id() const;
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
	threadpool *ptp_;
};

class _Leader final : 
	public _Thread_base {
public:
	_Leader(threadpool *ptp);

	~_Leader() = default; // nothing
	
	void start() override;

	void stop() override;
private:
	void run() override;
};

class _Worker final : 
	public _Thread_base {
public:
	_Worker(threadpool *);

	~_Worker() = default; // nothing

	void start() override;

	void stop() override;
private:
	void run() override;

	::std::mutex mutex_;
};

_SW_END // _SW_BEGIN

#endif // _SW_THREAD_H_
