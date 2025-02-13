// header file for sw::_Task_base and derived (internal)

#ifndef _SW_TASK_CORE_H_
#define _SW_TASK_CORE_H_

#include "sw_vals.h"

#include <cstdint>
#include <future>
#include <memory>
#include <tuple>
#include <type_traits>

_SW_BEGIN

class _Task_base {
public:
	using uptr = ::std::unique_ptr<_Task_base>;
	using priority_t = ::std::uint8_t;

	_Task_base() _SW_NOEXCEPT;

	virtual ~_Task_base() _SW_NOEXCEPT = default; // nothing

	virtual void execute() = 0;

	inline void set_priority(priority_t) _SW_NOEXCEPT;

	inline priority_t get_priority() const _SW_NOEXCEPT;
private:
	priority_t priority_;
};

template <typename _Fn, typename ..._Args>
class task final:
	public _Task_base {
public:
	using fn_type = _Fn;
	using ret_type = typename ::std::invoke_result<_Fn, _Args...>::type;
	using args_type = ::std::tuple<_Args...>;
	using promise_type = ::std::promise<ret_type>;

	task() = delete;

	task(const task&) = delete;	

	task(task&&) = delete;
    
	task(::std::shared_ptr<promise_type>, fn_type&&, args_type&&) _SW_NOEXCEPT;

	~task() _SW_NOEXCEPT = default; // nothing

	task& operator = (const task&) = delete;
	
	task& operator = (task&&) = delete;

	void execute() _SW_NOEXCEPT override;
private:
	fn_type fn_;
	args_type args_;
	::std::shared_ptr<promise_type> ppromise_;
};

_SW_END // _SW_BEGIN

#include "internal/sw_task_core.ipp"

#endif // _SW_TASK_CORE_H_
