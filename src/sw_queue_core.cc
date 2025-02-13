// implementation file for sw::_Queue_base and derived (internal)

#include "sw_queue_core.hpp"

_SW_BEGIN

void _Reject_handler_ignore::reject(_Queue_base::task_ptr&&) _SW_NOEXCEPT {
    return;
}

void _Reject_handler_ignore_throw::reject(_Queue_base::task_ptr&&) _SW_DES(::std::runtime_error) {
    _SW_THROW(::std::runtime_error("queue is full"));
    return;
}

_Reject_handler_delete_oldest::_Reject_handler_delete_oldest(_Queue_base::ptr pqueue) _SW_NOEXCEPT :
    pqueue_(pqueue) {}

void _Reject_handler_delete_oldest::reject(_Queue_base::task_ptr&&) _SW_DES(::std::runtime_error) {
    ::std::lock_guard<::std::mutex> _Lock(pqueue_->mutex_);
    while (pqueue_->size() >= pqueue_->capacity_ - 1) pqueue_->pop();
    _SW_THROW(::std::runtime_error("queue is full"));
    return;
}

_Queue_base::_Queue_base(size_type capacity = 0) _SW_NOEXCEPT : 
    size_(0), capacity_(capacity) {}

void _Queue_base::set_handler(_Reject_handler_base::uptr&& phandler)  _SW_NOEXCEPT {
    phandler_ = ::std::move(phandler);
}

_Queue::_Queue(size_type capacity = 0) _SW_NOEXCEPT : _Queue_base(capacity) {}

bool _Queue::push(_Queue_base::task_ptr&& task) _SW_NOEXCEPT {
    ::std::lock_guard<::std::mutex> _Lock(mutex_);
    if (size() >= capacity_) {
        if (phandler_) {
            phandler_->reject(::std::move(task));
        }
        return false;
    }
    ++size_;
    queue_.emplace_back(::std::move(task));
    return true;
}

_Queue_base::_Queue_base::task_ptr _Queue::pop() _SW_DES(::std::runtime_error) {
    ::std::lock_guard<::std::mutex> _Lock(mutex_);
    if (empty()) {
        _SW_THROW(::std::runtime_error("queue is empty"));
    }
    --size_;
    auto _Task = ::std::move(queue_.front());
    queue_.pop_front();
    return _Task;
}

bool _Queue::try_pop(_Queue_base::task_ptr& task) _SW_NOEXCEPT {
    ::std::lock_guard<::std::mutex> _Lock(mutex_);  
    if (queue_.empty()) {  
        return false;  
    }  
    --size_;
    task = ::std::move(queue_.front());  
    queue_.pop_front();  
    return true; 
}

_Queue_priority::_Queue_priority(size_type capacity = 0) _SW_NOEXCEPT : 
    _Queue_base(capacity) {}

bool _Queue_priority::push(_Queue_base::task_ptr&& task) _SW_NOEXCEPT {
    ::std::lock_guard<::std::mutex> _Lock(mutex_);
    if (size() >= capacity_) {
        if (phandler_) {
            phandler_->reject(::std::move(task));
        }
        return false;
    }
    ++size_;
    queue_.emplace(::std::move(task));
    return true;
}

_Queue_priority::_Queue_base::task_ptr _Queue_priority::pop() _SW_DES(::std::runtime_error) {
    ::std::lock_guard<::std::mutex> _Lock(mutex_);
    if (empty()) {
        _SW_THROW(::std::runtime_error("queue is empty"));
    }
    --size_;
    _Queue_base::task_ptr _Task;
    auto& _Top = queue_.top();
    // _Task.swap(_Top);
    queue_.pop();
    return _Task;
}

bool _Queue_priority::try_pop(_Queue_base::task_ptr& task) _SW_NOEXCEPT {
    ::std::lock_guard<::std::mutex> _Lock(mutex_);  
    if (queue_.empty()) {  
        return false;  
    }
    --size_;
    auto& _Top = queue_.top();
    // task.swap(_Top);
    queue_.pop();  
    return true; 
}

bool _Queue_priority::_Comparator::operator()(const task_ptr& lhs, const task_ptr& rhs) const _SW_NOEXCEPT {
    return lhs->get_priority() < rhs->get_priority();
}

_SW_END // _SW_BEGIN
