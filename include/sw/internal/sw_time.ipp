// implementation file for sw::time (inline and template members) (internal)

_SW_BEGIN

template <typename _Ct, typename _Dr>
stopwatch<_Ct, _Dr>::stopwatch(bool auto_start) _SW_NOEXCEPT : 
    accumulated_duration_(0), start_(), paused_(false) {
    if (auto_start) 
        start();
}

template <typename _Ct, typename _Dr>
void stopwatch<_Ct, _Dr>::start() _SW_NOEXCEPT {
    accumulated_duration_ = 0;
    start_ = _Ct::now();
    paused_ = false;
}

template <typename _Ct, typename _Dr>
void stopwatch<_Ct, _Dr>::pause() _SW_NOEXCEPT {
    accumulated_duration_ += elapsed();
    paused_ = true;
}

template <typename _Ct, typename _Dr>
bool stopwatch<_Ct, _Dr>::is_paused() const _SW_NOEXCEPT {
    return paused_;
}

template <typename _Ct, typename _Dr>
void stopwatch<_Ct, _Dr>::resume() _SW_NOEXCEPT {
    start_ = _Ct::now();
    paused_ = false;
}

template <typename _Ct, typename _Dr>
::std::uint64_t stopwatch<_Ct, _Dr>::elapsed() const _SW_NOEXCEPT {
    if (paused_) {
        return accumulated_duration_;
    }
    return accumulated_duration_ + ::std::chrono::duration_cast<_Dr>
        (_Ct::now() - start_).count();
}

template <typename _Ct, typename _Dr, typename _Cb>
counter<_Ct, _Dr, _Cb>::counter(const std::string& key, callback_type&& callback) _SW_NOEXCEPT :
        key_(key), callback_(callback) {
    }

template <typename _Ct, typename _Dr, typename _Cb>
counter<_Ct, _Dr, _Cb>::~counter() {
    callback_(key_, stopwatch_.elapsed());
}

_SW_END // _SW_BEGIN
