// implementation file for sw::_Thread_base and derived (internal)

#include "../include/sw_thread.h"
#include "../include/sw_threadpool.hpp"

_SW_BEGIN

_Thread_base::_Thread_base(_Thread_base::threadpool_ptr ptp) : 
	state_(_State::IDLE), ptp_(::std::move(ptp)) {}

void _Thread_base::start() {
}

void _Thread_base::stop() {
}

_Thread_base::_State _Thread_base::state() {
    ::std::lock_guard<::std::mutex> _Lock(mutex_);
    return state_;
}

_Leader::_Leader(_Thread_base::threadpool_ptr ptp) : 
	_Thread_base(::std::move(ptp)) {}

void _Leader::start() {
    thread_ = ::std::thread(&_Leader::run, this);
    id_ = thread_.get_id();
    state_ = _State::RUNNING;
    return;
}

void _Leader::stop() {
    state_ = _State::STOPPING;
    auto ptp = ptp_.lock();
    for (int i = 1; i < ptp->pthreads_.size(); ++i) {
        // awake workers in sleeping state EVERY CICLE, to avoid dead lock. or fall into sleeping while stopping
        ptp->cv_.notify_all();
        ptp->pthreads_[i]->stop();
    }
    thread_.join();
    state_ = _State::STOPPED;
    return;
}

void _Leader::run() {
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

_Worker::_Worker(_Thread_base::threadpool_ptr ptp, bool is_core) : 
	_Thread_base(::std::move(ptp)), is_core_(is_core) {}

void _Worker::start() {
    thread_ = ::std::thread(&_Worker::run, this);
    id_ = thread_.get_id();
    state_ = _State::IDLE;
    return;
}

void _Worker::stop() {
    ::std::lock_guard<::std::mutex> _Lock(mutex_);
    state_ = _State::STOPPING;
    thread_.join();
    state_ = _State::STOPPED;
    return;
}

void _Worker::sleep() {
    ::std::lock_guard<::std::mutex> _Lock(mutex_);
    state_ = _State::SLEEPING;
}

void _Worker::run() {
    ::std::unique_ptr<_Task_base> _Task;
    auto ptp = ptp_.lock();
    auto sw = sw::stopwatch<::std::chrono::steady_clock>();
    while (state_ != _State::STOPPING && state_ != _State::STOPPED) {
        if (ptp->keepalive_time_ >= 0 && !is_core_ && sw.elapsed() > ptp->keepalive_time_ * 1000) {
            {
                ::std::lock_guard<::std::mutex> _Lock(mutex_);
                state_ = _State::SLEEPING;
            }
            ::std::unique_lock<::std::mutex> _Q_lock(ptp->mutex_);
            ptp->cv_.wait(_Q_lock);
            {
                ::std::lock_guard<::std::mutex> _Lock(mutex_);
                state_ = _State::IDLE;
            }
            sw.start();
        } else if (ptp->pqueue_->try_pop(_Task)) {
            {
                ::std::lock_guard<::std::mutex> _Lock(mutex_);
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
