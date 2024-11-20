// implementation file for sw::threadpool (internal)

#include "../include/sw_threadpool.hpp"

_SW_BEGIN

threadpool::threadpool() : 
        worker_capacity_(::std::thread::hardware_concurrency()), pthreads_(::std::thread::hardware_concurrency()), 
            pqueue_(_Queue_create_helper(QUEUE_TYPE_FIFO, 1024)), keepalive_time_(1000) {
                assert(worker_capacity_ >= 2);
                pqueue_->set_handler(_Reject_handler_create_helper(HANDLER_TYPE_IGNORE));
                pthreads_.resize(worker_capacity_);
                _Init();
            }

threadpool::threadpool(::std::size_t worker_capacity, queue_type queue, ::std::size_t queue_capacity, size_t keepalive_time, handler_type handler) : 
        worker_capacity_(worker_capacity), pthreads_(worker_capacity), pqueue_(_Queue_create_helper(queue, queue_capacity)),
            keepalive_time_(keepalive_time) {
                assert(worker_capacity_ >= 2);
                pqueue_->set_handler(_Reject_handler_create_helper(handler));
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
    while (!pqueue_->empty()) {
        ::std::this_thread::sleep_for(::std::chrono::milliseconds(10));
    }
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
