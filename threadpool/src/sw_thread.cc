// implementation file for sw::_Thread_base and derived (internal)

#include "../include/sw_thread.h"
#include "../include/sw_threadpool.hpp"

_SW_BEGIN

_Thread_base::_Thread_base(threadpool *ptp) : 
	state_(_State::IDLE), ptp_(ptp) {}

void _Thread_base::start() {
    assert(false);
}

void _Thread_base::stop() {
    assert(false);
}

_Thread_base::_State _Thread_base::state() const {
    return state_;
}

_Thread_base::thread_id _Thread_base::get_thread_id() const {
	return id_;
}

_Leader::_Leader(threadpool *ptp) : 
	_Thread_base(ptp) {}

void _Leader::start() {
    thread_ = ::std::thread(&_Leader::run, this);
    id_ = thread_.get_id();
    state_ = _State::RUNNING;
    return;
}

void _Leader::stop() {
    state_ = _State::STOPPING;
    for (int i = 1; i < ptp_->pthreads_.size(); ++i) {
        ptp_->pthreads_[i]->stop();
    }
    thread_.join();
    state_ = _State::STOPPED;
    return;
}

void _Leader::run() {
    while (state_ != _State::STOPPING && state_ != _State::STOPPED) {
        auto _Start = ::std::chrono::high_resolution_clock::now();
        while (::std::chrono::high_resolution_clock::now() - _Start < ::std::chrono::milliseconds(1000));
        for (auto &worker : ptp_->pthreads_) {
            if (worker->state() == _State::SLEEPING) {
                // dynamic adjust
            }
        }
    }
}

_Worker::_Worker(threadpool *ptp) : 
	_Thread_base(ptp) {}

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

void _Worker::run() {
    ::std::unique_ptr<_Task_base> _Task;
    auto _Start = ::std::chrono::high_resolution_clock::now();
    while (state_ != _State::STOPPING && state_ != _State::STOPPED) {
        if (::std::chrono::high_resolution_clock::now() - _Start > ::std::chrono::milliseconds(ptp_->keepalive_time_)) {
            ::std::lock_guard<::std::mutex> _Lock(mutex_);
            state_ = _State::SLEEPING;
            while (true); // to do: wait leader to wake me up
        } else if (ptp_->pqueue_->try_pop(_Task)) {
            {
                ::std::lock_guard<::std::mutex> _Lock(mutex_);
                state_ = _State::RUNNING;
                _Task->execute();
                state_ = _State::IDLE;  
            }
            _Start = ::std::chrono::high_resolution_clock::now();
            ptp_->info_.total_completions_.fetch_add(1);
            _Task.reset();
        }
    }
}

_SW_END // _SW_BEGIN
