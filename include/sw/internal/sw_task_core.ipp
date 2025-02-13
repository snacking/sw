// implementation file for sw::_Task_base and derived (inline and template members) (internal)

_SW_BEGIN

inline void _Task_base::set_priority(::std::uint8_t priority) _SW_NOEXCEPT {
    priority_ = priority;
    return;
}

inline ::std::uint8_t _Task_base::get_priority() const _SW_NOEXCEPT {  
    return priority_;  
}

template <typename _Fn, typename ..._Args>
void task<_Fn, _Args...>::execute() _SW_NOEXCEPT {
    _TRY_BEGIN
        if (ppromise_) {
            if constexpr(::std::is_same<ret_type, void>::value) { // nothing to set
                ::std::apply(fn_, args_);
                ppromise_->set_value();
            } else {
                ppromise_->set_value(::std::apply(fn_, args_));  
            }   
        }  
    _CATCH_ALL  
        if (ppromise_) {  
            ppromise_->set_exception(::std::current_exception());  
        }  
    _CATCH_END
}

_SW_END // _SW_BEGIN
