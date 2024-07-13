// header file for sw::_Queue_base and derived (internal)

#ifndef _SW_QUEUE_CORE_H_
#define _SW_QUEUE_CORE_H_

#include "../../include/sw_vals.h"
#include "./sw_task_core.hpp"
#include <deque>
#include <queue>
#include <memory>
#include <cstddef>
#include <mutex>

_SW_BEGIN

class _Task_base;
class _Queue_base;

enum handler_type {
	HANDLER_TYPE_IGNORE,
	HANDLER_TYPE_IGNORE_THROW,
	HANDLER_TYPE_DELETE_OLDEST
};

class _Reject_handler_base {
public:
	using task_type = ::std::unique_ptr<_Task_base>;
	using queue_type = ::std::weak_ptr<_Queue_base>;

	virtual void reject(task_type&&) = 0;
protected:
	queue_type pqueue_;
};

enum queue_type {
	QUEUE_TYPE_FIFO,
	QUEUE_TYPE_PRIORITY
};

class _Queue_base {
public:
	using size_type = ::std::size_t;
	using task_type = ::std::unique_ptr<_Task_base>;

	_Queue_base(size_type);

	virtual ~_Queue_base() = default;

	virtual void push(task_type&&) = 0;

	virtual task_type pop() = 0;

	virtual bool try_pop(task_type&) = 0;

	inline bool empty() const;

	inline _Queue_base::size_type size() const;

	inline void set_handler(::std::unique_ptr<_Reject_handler_base>&&);
protected:
	size_type size_, capacity_;
	::std::unique_ptr<_Reject_handler_base> phandler_;
};

class _Reject_handler_ignore : 
	public _Reject_handler_base {
public:
	void reject(task_type&&) override;
};

class _Reject_handler_ignore_throw : 
	public _Reject_handler_base {
public:
	void reject(task_type&&) override;
};

class _Reject_handler_delete_oldest : 
	public _Reject_handler_base {
public:
	void reject(task_type&&) override;
};

class _Queue :
	public _Queue_base {
public:
	_Queue(size_type);

	void push(task_type&&) override;

	task_type pop() override;

	bool try_pop(task_type&) override;
private:
	::std::deque<task_type> queue_;
	::std::mutex mutex_;
};

class _Queue_priority :
	public _Queue_base {
public:
	_Queue_priority(size_type);

	void push(task_type&& task) override;

	task_type pop() override;

	bool try_pop(task_type& task) override;
private:
	struct _Comparator {
		bool operator()(const task_type& lhs, const task_type& rhs) const {
			return lhs->get_priority() < rhs->get_priority();
		}
	};

	::std::priority_queue<task_type, ::std::vector<task_type>, _Comparator> queue_;
	::std::mutex mutex_;
};

_SW_END // _SW_BEGIN

#endif // _SW_QUEUE_CORE_H_
