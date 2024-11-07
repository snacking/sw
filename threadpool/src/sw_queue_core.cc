// implementation file for sw::_Queue_base and derived (internal)

#include "../include/sw_queue_core.h"

_SW_BEGIN

void _Reject_handler_ignore::reject(task_type&&) {
    return;
}

void _Reject_handler_ignore_throw::reject(task_type&&) {
    return;
}

void _Reject_handler_delete_oldest::reject(task_type&&) {
    return;
}

_Queue_base::_Queue_base(size_type capacity = 0) : 
    size_(0), capacity_(capacity) {}

bool _Queue_base::empty() const {
    return size_ == 0;
}

_Queue_base::size_type _Queue_base::size() const {
    return size_;
}

void _Queue_base::set_handler(::std::unique_ptr<_Reject_handler_base>&& handler) {
    phandler_ = ::std::move(handler);
}

_Queue::_Queue(size_type capacity = 0) : _Queue_base(capacity) {}

void _Queue::push(task_type&& task) {
    ::std::lock_guard<::std::mutex> _Lock(mutex_);
    if (size() >= capacity_) {
        if (phandler_) {
            phandler_->reject(::std::move(task));
        }
    }
    ++size_;
    queue_.emplace_back(::std::move(task));
    return;
}

_Queue_base::task_type _Queue::pop() {
    ::std::lock_guard<::std::mutex> _Lock(mutex_);
    if (empty()) {
        throw ::std::runtime_error("queue is empty");
    }
    --size_;
    auto _Task = ::std::move(queue_.front());
    queue_.pop_front();
    return _Task;
}

bool _Queue::try_pop(task_type& task) {
    ::std::lock_guard<::std::mutex> _Lock(mutex_);  
    if (queue_.empty()) {  
        return false;  
    }  
    --size_;
    task = ::std::move(queue_.front());  
    queue_.pop_front();  
    return true; 
}

_Queue_priority::_Queue_priority(size_type capacity = 0) : 
    _Queue_base(capacity) {}

void _Queue_priority::push(task_type&& task) {
    ::std::lock_guard<::std::mutex> _Lock(mutex_);
    if (size() >= capacity_) {
        if (phandler_) {
            phandler_->reject(::std::move(task));
            throw ::std::runtime_error("queue is full");
        } else {
            return;
        }
    }
    ++size_;
    queue_.emplace(::std::move(task));
    return;
}

_Queue_priority::task_type _Queue_priority::pop() {
    ::std::lock_guard<::std::mutex> _Lock(mutex_);
    if (empty()) {
        throw ::std::runtime_error("queue is empty");
    }
    --size_;
    task_type _Task;
    auto& _Top = queue_.top();
    // _Task.swap(_Top);
    queue_.pop();
    return _Task;
}

bool _Queue_priority::try_pop(task_type& task) {
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

_SW_END // _SW_BEGIN
