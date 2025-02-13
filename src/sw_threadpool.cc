// implementation file for sw::threadpool (internal)

#include "sw_threadpool.hpp"

_SW_BEGIN

threadpool_settings::threadpool_settings() : 
        worker_capacity(::std::thread::hardware_concurrency()), 
            core_capacity(0), queue_capacity(100), keepalive_time(2000), 
                queue(queue_type::QUEUE_TYPE_FIFO), handler(handler_type::HANDLER_TYPE_IGNORE) {
}

threadpool::ptr threadpool::create(threadpool_settings ts) _SW_NOEXCEPT {
    auto ptr =  threadpool::ptr(new threadpool(ts));
    ptr->_Init();
    return ptr;
}

threadpool::~threadpool() _SW_NOEXCEPT {
    if (state_ == _State::RUNNING) {
        _Close();
    }
}

void threadpool::shutdown() _SW_NOEXCEPT {
    state_ = _State::STOPPING;
    join();
    _Close();
    return;
}

::std::vector<threadpool::task_ptr> threadpool::shutdown_now() _SW_NOEXCEPT {
    state_ = _State::STOPPING;
    _Close();
    ::std::vector<threadpool::task_ptr> remains;
    while (!pqueue_->empty()) {
        auto task = pqueue_->pop();
        remains.emplace_back(::std::move(task));
    }
    return remains;
}

void threadpool::join() _SW_NOEXCEPT {
    ::std::unique_lock<::std::mutex> lock(mutex_);
    cv_.wait(lock, [this](){ return pqueue_->empty(); });
    return;
}

threadpool::threadpool(threadpool_settings ts) _SW_NOEXCEPT : 
    state_(_State::RUNNING), worker_capacity_(ts.worker_capacity), core_capacity_(ts.core_capacity), pthreads_(ts.worker_capacity), 
        pqueue_(_Queue_create_helper(ts.queue, ts.queue_capacity)), keepalive_time_(ts.keepalive_time) {
    assert(worker_capacity_ >= 2); // worker's number must be larger than one, since the first element of pthreads_ is leader
    assert(worker_capacity_ >= core_capacity_ + 1);
    pqueue_->set_handler(_Reject_handler_create_helper(ts.handler));
    pthreads_.resize(worker_capacity_);
}

threadpool::queue_ptr threadpool::_Queue_create_helper(queue_type type, ::std::size_t capacity) _SW_NOEXCEPT {
    switch (type) {
    case queue_type::QUEUE_TYPE_FIFO:
        return ::std::make_shared<_Queue>(capacity);
    case queue_type::QUEUE_TYPE_PRIORITY:
        return ::std::make_shared<_Queue_priority>(capacity);
    }
    return ::std::make_shared<_Queue>(capacity);
}

threadpool::handler_ptr threadpool::_Reject_handler_create_helper(handler_type type) _SW_NOEXCEPT {
    switch (type) {
    case handler_type::HANDLER_TYPE_IGNORE:
        return ::std::make_unique<_Reject_handler_ignore>();
    case handler_type::HANDLER_TYPE_IGNORE_THROW:
        return ::std::make_unique<_Reject_handler_ignore_throw>();
    case handler_type::HANDLER_TYPE_DELETE_OLDEST:
        return ::std::make_unique<_Reject_handler_delete_oldest>(pqueue_);
    }
    return ::std::make_unique<_Reject_handler_ignore>();
}

_SW_END // _SW_BEGIN
