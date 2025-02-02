// implementation file for sw::task_core and derived (internal)

#include "../include/sw_task_core.hpp"

_SW_BEGIN

_Task_base::_Task_base() : priority_(0) {}

template <typename _Fn, typename ..._Args>
task<_Fn, _Args...>::task(::std::shared_ptr<promise_type> ppromise, fn_type&& fn, args_type&& args) : 
		ppromise_(ppromise), fn_(fn), args_(args) {}

_SW_END // _SW_BEGIN
