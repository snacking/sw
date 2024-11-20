// implementation file for sw::threadpool (internal)

#include "../include/sw_threadpool.hpp"

_SW_BEGIN

threadpool_settings::threadpool_settings() : 
    worker_capacity(::std::thread::hardware_concurrency()), core_capacity(0), queue_capacity(100), keepalive_time(10000), queue(QUEUE_TYPE_FIFO), handler(HANDLER_TYPE_IGNORE) { 
}

threadpool::threadpool(threadpool_settings ts) : 
        worker_capacity_(ts.worker_capacity), core_capacity_(ts.core_capacity), pthreads_(ts.worker_capacity), pqueue_(_Queue_create_helper(ts.queue, ts.queue_capacity)),
            keepalive_time_(ts.keepalive_time) {
                assert(worker_capacity_ >= 2); // worker's number must be larger than one, since the first element of pthread is leader
                assert(worker_capacity_ >= core_capacity_ + 1);
                pqueue_->set_handler(_Reject_handler_create_helper(ts.handler));
                pthreads_.resize(worker_capacity_);
                _Init();
            }

threadpool::~threadpool() {
    if (state_ == _State::RUNNING) {
        _Close();
    }
    for (int i = 0; i < pthreads_.size(); ++i) {
        delete pthreads_[i];
    }
    delete pqueue_;
}

void threadpool::shutdown() {
    state_ = _State::STOPPING;
    join();
    _Close();
    return;
}

::std::vector<::std::unique_ptr<_Task_base> > threadpool::shutdown_now() {
    state_ = _State::STOPPING;
    _Close();
    ::std::vector<::std::unique_ptr<_Task_base> > _Remains;
    while (!pqueue_->empty()) {
        auto _Task = pqueue_->pop();
        _Remains.emplace_back(::std::move(_Task));
    }
    return _Remains;
}

void threadpool::join() {
    while (!pqueue_->empty());
    return;
}

_Queue_base *threadpool::_Queue_create_helper(queue_type type, ::std::size_t capacity) {
    switch (type) {
    case QUEUE_TYPE_FIFO:
        return new _Queue(capacity);
    case QUEUE_TYPE_PRIORITY:
        return new _Queue_priority(capacity);
    }
    return new _Queue(capacity);
}

::std::unique_ptr<_Reject_handler_base> threadpool::_Reject_handler_create_helper(handler_type type) {
    switch (type) {
    case HANDLER_TYPE_IGNORE:
        return ::std::make_unique<_Reject_handler_ignore>();
    case HANDLER_TYPE_IGNORE_THROW:
        return ::std::make_unique<_Reject_handler_ignore_throw>();
    case HANDLER_TYPE_DELETE_OLDEST:
        return ::std::make_unique<_Reject_handler_delete_oldest>(pqueue_);
    }
    return ::std::make_unique<_Reject_handler_ignore>();
}

_SW_END // _SW_BEGIN
