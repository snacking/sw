// implementation file for sw::_Queue_base and derived (inline members) (internal)

_SW_BEGIN

inline bool _Queue_base::empty() const {
    return size_ == 0;
}

inline ::std::size_t _Queue_base::size() const {
    return size_;
}

_SW_END