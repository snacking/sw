// implementation file for sw::_Thread_base and derived (internal)

#include "sw/internal/sw_thread.hpp"

#include "sw/sw_threadpool.hpp"

_SW_BEGIN

_Thread_base::_Thread_base(_Thread_base::threadpool_ptr ptp) _SW_NOEXCEPT : 
	state_(_State::INIT), ptp_(::std::move(ptp)) {}

_Thread_base::_State _Thread_base::state() _SW_NOEXCEPT {
    ::std::shared_lock<::std::shared_mutex> lock(mutex_);
    return state_;
}

_Leader::_Leader(_Thread_base::threadpool_ptr ptp) _SW_NOEXCEPT : 
	_Thread_base(::std::move(ptp)) {}

void _Leader::start() _SW_NOEXCEPT {
    {
        ::std::unique_lock<::std::shared_mutex> lock(mutex_);
        if (state_ != _State::INIT) {
            return;
        }
        state_ = _State::IDLE;
    }
    thread_ = ::std::thread(&_Leader::run, this);
    id_ = thread_.get_id();
    return;
}

void _Leader::stop() _SW_NOEXCEPT {
    {
        ::std::unique_lock<::std::shared_mutex> lock(mutex_);
        if (state_ != _State::RUNNING) {
            return;
        }
        state_ = _State::STOPPING;
    }
    auto ptp = ptp_.lock();
    for (::std::size_t i = 1; i < ptp->pthreads_.size(); ++i) {
        ptp->cv_.notify_all();
        ptp->pthreads_[i]->stop();
    }
    thread_.join();
    {
        ::std::unique_lock<::std::shared_mutex> lock(mutex_);
        state_ = _State::STOPPED;
    }
    return;
}

void _Leader::run() _SW_NOEXCEPT {
    {
        ::std::unique_lock<::std::shared_mutex> lock(mutex_);
        state_ = _State::RUNNING;
    }
    auto ptp = ptp_.lock();
    while (state_ != _State::STOPPING && state_ != _State::STOPPED) {
        ::std::unique_lock<::std::mutex> lock(ct_mutex_);
        ct_cv_.wait_for(lock, ::std::chrono::milliseconds(1000), [this] {
            return state_ == _State::STOPPING || state_ == _State::STOPPED;
        });
        if (state_ == _State::STOPPING || state_ == _State::STOPPED) {
            break;
        }
        for (auto &worker : ptp->pthreads_) {
            if (worker->state() == _State::SLEEPING) {
                // dynamic adjust
            }
        }
    }
    return;
}

_Worker::_Worker(_Thread_base::threadpool_ptr ptp, bool is_core) _SW_NOEXCEPT : 
	_Thread_base(::std::move(ptp)), is_core_(is_core) {}

void _Worker::start() _SW_NOEXCEPT {
    {
        ::std::unique_lock<::std::shared_mutex> lock(mutex_);
        if (state_ != _State::INIT) {
            return;
        }
        state_ = _State::IDLE;
    }
    thread_ = ::std::thread(&_Worker::run, this);
    id_ = thread_.get_id();
    return;
}

void _Worker::stop() _SW_NOEXCEPT {
    ::std::unique_lock<::std::shared_mutex> lock(mutex_);
    state_ = _State::STOPPING;
    thread_.join();
    state_ = _State::STOPPED;
    return;
}

void _Worker::sleep() _SW_NOEXCEPT {
    ::std::unique_lock<::std::shared_mutex> lock(mutex_);
    state_ = _State::SLEEPING;
}

void _Worker::run() _SW_NOEXCEPT {
    ::std::unique_ptr<_Task_base> _Task;
    auto ptp = ptp_.lock();
    auto sw = sw::stopwatch<::std::chrono::steady_clock>();
    while (state_ != _State::STOPPING && state_ != _State::STOPPED) {
        if (ptp->keepalive_time_ >= 0 && !is_core_ && sw.elapsed() > ptp->keepalive_time_ * 1000) {
            {
                ::std::unique_lock<::std::shared_mutex> lock(mutex_);
                state_ = _State::SLEEPING;
            }
            ::std::unique_lock<::std::mutex> q_lock(ptp->mutex_);
            ptp->cv_.wait(q_lock);
            {
                ::std::unique_lock<::std::shared_mutex> lock(mutex_);
                state_ = _State::IDLE;
            }
            sw.start();
        } else if (ptp->pqueue_->try_pop(_Task)) {
            {
                ::std::unique_lock<::std::shared_mutex> lock(mutex_);
                state_ = _State::RUNNING;
                _Task->execute();
                state_ = _State::IDLE;  
            }
            ptp->info_.total_completions_.fetch_add(1);
            sw.start();
            _Task.reset();
        }
    }
    return;
}

_SW_END // _SW_BEGIN
