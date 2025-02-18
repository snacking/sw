// implementation file for sw::threadpool (inline and template members) (internal)

_SW_BEGIN

template <typename _Fn, typename... _Args>  
::std::future<typename ::std::invoke_result<_Fn, _Args...>::type> threadpool::submit(_Fn&& fn, _Args&&... args) _SW_NOEXCEPT {
    static_assert(!::std::is_same<typename ::std::invoke_result<_Fn, _Args...>::type, void>::value, "return type not supported");
    auto promise = ::std::make_shared<::std::promise<typename ::std::invoke_result<_Fn, _Args...>::type> >();
    _Create_and_push_task(promise, std::forward<_Fn>(fn), std::forward<_Args>(args)...);
    return promise->get_future();
}

template <typename _Fn, typename... _Args>  
void threadpool::execute(_Fn&& fn, _Args&&... args) _SW_NOEXCEPT {
    static_assert(::std::is_invocable<_Fn, _Args...>::value, "function not callable");
    static_assert(::std::is_same<typename ::std::invoke_result<_Fn, _Args...>::type, void>::value, "return type not supported");
    auto promise = ::std::make_shared<::std::promise<typename ::std::invoke_result<_Fn, _Args...>::type> >(); 
    _Create_and_push_task(promise, std::forward<_Fn>(fn), std::forward<_Args>(args)...);
    return;
}

inline ::std::size_t threadpool::size() const _SW_NOEXCEPT {
    return pqueue_->size();
}

inline ::std::tuple<::std::size_t, ::std::size_t, ::std::size_t> threadpool::statistics() const _SW_NOEXCEPT {
    return ::std::make_tuple(info_.total_submissions_, info_.total_rejected_, info_.total_completions_.load());
}

inline void threadpool::_Init() _SW_NOEXCEPT {
    for (::std::size_t i = 0; i < worker_capacity_; ++i) {
        if (i == 0) 
            pthreads_[i] = ::std::make_shared<_Leader>(threadpool::wptr(shared_from_this()));
        else if(i <= core_capacity_)
            pthreads_[i] = ::std::make_shared<_Worker>(threadpool::wptr(shared_from_this()), true);
        else
            pthreads_[i] = ::std::make_shared<_Worker>(threadpool::wptr(shared_from_this()), false);
        pthreads_[i]->start();
    }
    return;
}

inline void threadpool::_Close() _SW_NOEXCEPT {
    pthreads_[0]->stop();
    {
        ::std::lock_guard<std::mutex> lock(mutex_);
        state_ = _State::TERMINATED;
    }
    return;
}

template <typename _Fn, typename... _Args>
inline void threadpool::_Create_and_push_task(::std::shared_ptr<::std::promise<typename ::std::invoke_result<_Fn, _Args...>::type> > ppromise, 
    _Fn&& fn, _Args&&... args) _SW_NOEXCEPT {
    auto _Task = ::std::make_unique<task<_Fn, _Args...> >(
        ppromise,
        ::std::forward<_Fn>(fn), 
        ::std::forward_as_tuple(::std::forward<_Args>(args)...)
    );  
    auto success = pqueue_->push(::std::move(_Task));
    info_.total_submissions_ += (int)success;
    info_.total_rejected_ += (int)(!success);
    return;
}

_SW_END // _SW_BEGIN
