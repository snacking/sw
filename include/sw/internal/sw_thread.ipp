// implementation file for sw::_Thread_base and derived (inline members) (internal)

_SW_BEGIN

inline ::std::thread::id _Thread_base::get_thread_id() const _SW_NOEXCEPT {
	return id_;
}

_SW_END // _SW_BEGIN
