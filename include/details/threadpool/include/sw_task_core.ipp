// implementation file for sw::_Task_base and derived (inline members) (internal)

_SW_BEGIN

inline void _Task_base::set_priority(::std::uint8_t priority) noexcept {
    priority_ = priority;
    return;
}

inline ::std::uint8_t _Task_base::get_priority() const noexcept {  
    return priority_;  
}

_SW_END // _SW_BEGIN
