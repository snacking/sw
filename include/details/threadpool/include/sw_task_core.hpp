// header file for sw::_Task_base and derived (internal)

#ifndef _SW_TASK_CORE_H_
#define _SW_TASK_CORE_H_

#include "../../sw_vals.h"

#include <cstdint>
#include <memory>
#include <type_traits>
#include <tuple>
#include <future>

_SW_BEGIN

class _Task_base {
public:
	using priority_t = ::std::uint8_t;

	_Task_base() : priority_(0) {}

	virtual ~_Task_base() = default; // nothing

	virtual void execute() = 0;

	inline void set_priority(priority_t priority) noexcept {
		priority_ = priority;
		return;
	}

	inline priority_t get_priority() const noexcept {  
        return priority_;  
    }
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
    
	task(::std::shared_ptr<promise_type> ppromise, fn_type&& fn, args_type&& args) : 
		ppromise_(ppromise), fn_(fn), args_(args) {}

	~task() = default; // nothing

	task& operator = (const task&) = delete;
	task& operator = (task&&) = delete;

	void execute() override {
		try {  
            if (ppromise_) {
				if constexpr(::std::is_same<ret_type, void>::value) { // nothing to set
					::std::apply(fn_, args_);
					ppromise_->set_value();
				} else {
					ppromise_->set_value(::std::apply(fn_, args_));  
				}   
            }  
        } catch (...) {  
            if (ppromise_) {  
                ppromise_->set_exception(::std::current_exception());  
            }  
        }  
	}
private:
	fn_type fn_;
	args_type args_;
	::std::shared_ptr<promise_type> ppromise_;
};

_SW_END // _SW_BEGIN

#endif // _SW_TASK_CORE_H_
