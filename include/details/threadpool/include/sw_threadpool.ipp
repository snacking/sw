// implementation file for sw::threadpool (inline and template members) (internal)

_SW_BEGIN

template <typename _Fn, typename... _Args>  
::std::future<typename ::std::invoke_result<_Fn, _Args...>::type> threadpool::submit(_Fn&& fn, _Args&&... args) {
    static_assert(!::std::is_same<typename ::std::invoke_result<_Fn, _Args...>::type, void>::value, "return type not supported");
    auto _Promise = ::std::make_shared<::std::promise<typename ::std::invoke_result<_Fn, _Args...>::type> >();
    _Create_and_push_task(_Promise, std::forward<_Fn>(fn), std::forward<_Args>(args)...);
    return _Promise->get_future();
}

template <typename _Fn, typename... _Args>  
void threadpool::execute(_Fn&& fn, _Args&&... args) {
    static_assert(::std::is_invocable<_Fn, _Args...>::value, "function not callable");
    static_assert(::std::is_same<typename ::std::invoke_result<_Fn, _Args...>::type, void>::value, "return type not supported");
    auto _Promise = ::std::make_shared<::std::promise<typename ::std::invoke_result<_Fn, _Args...>::type> >(); 
    _Create_and_push_task(_Promise, std::forward<_Fn>(fn), std::forward<_Args>(args)...);
    return;
}

inline ::std::size_t threadpool::size() const {
    return pqueue_->size();
}

inline ::std::tuple<::std::size_t, ::std::size_t, ::std::size_t> threadpool::statistics() const {
    return ::std::make_tuple(info_.total_submissions_, info_.total_rejected_, info_.total_completions_.load());
}

inline void threadpool::_Init() {
    for (::std::size_t i = 0; i < worker_capacity_; ++i) {
        if (i == 0) 
            pthreads_[i] = new _Leader(this);
        else if(i <= core_capacity_)
            pthreads_[i] = new _Worker(this, true);
        else
            pthreads_[i] = new _Worker(this, false);
        pthreads_[i]->start();
    }
    return;
}

inline void threadpool::_Close() {
    pthreads_[0]->stop();
    state_ = _State::TERMINATED;
    return;
}

template <typename _Fn, typename... _Args>
inline void threadpool::_Create_and_push_task(::std::shared_ptr<::std::promise<typename ::std::invoke_result<_Fn, _Args...>::type> > ppromise, _Fn&& fn, _Args&&... args) {
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