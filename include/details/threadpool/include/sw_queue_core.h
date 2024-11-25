// header file for sw::_Queue_base and derived (internal)

#ifndef _SW_QUEUE_CORE_H_
#define _SW_QUEUE_CORE_H_

#include "../../sw_vals.h"
#include "./sw_task_core.hpp"

#include <deque>
#include <queue>
#include <memory>
#include <cstddef>
#include <mutex>

_SW_BEGIN

class _Task_base;
class _Queue_base;

enum class handler_type : uint8_t {
	HANDLER_TYPE_IGNORE,
	HANDLER_TYPE_IGNORE_THROW,
	HANDLER_TYPE_DELETE_OLDEST
};

class _Reject_handler_base {
public:
	using uptr = ::std::unique_ptr<_Reject_handler_base>;
	using task_ptr = _Task_base::uptr;

	virtual void reject(task_ptr&&) = 0;
};

enum class queue_type : ::std::uint8_t {
	QUEUE_TYPE_FIFO,
	QUEUE_TYPE_PRIORITY
};

class _Queue_base {
public:
	friend class _Reject_handler_delete_oldest;

	using ptr = ::std::shared_ptr<_Queue_base>;
	using size_type = ::std::size_t;
	using task_ptr = _Task_base::uptr;
	using handler_ptr = _Reject_handler_base::uptr;

	_Queue_base(size_type);

	virtual ~_Queue_base() = default;

	virtual bool push(task_ptr&&) = 0;

	virtual task_ptr pop() = 0;

	virtual bool try_pop(task_ptr&) = 0;

	inline bool empty() const;

	inline size_type size() const;

	void set_handler(handler_ptr&&);
protected:
	::std::mutex mutex_;
	volatile size_type size_, capacity_;
	handler_ptr phandler_;
};

class _Reject_handler_ignore : 
	public _Reject_handler_base {
public:
	void reject(task_ptr&&) override;
};

class _Reject_handler_ignore_throw : 
	public _Reject_handler_base {
public:
	void reject(task_ptr&&) override;
};

class _Reject_handler_delete_oldest : 
	public _Reject_handler_base {
public:
	_Reject_handler_delete_oldest(_Queue_base::ptr);
	void reject(task_ptr&&) override;
private:
	_Queue_base::ptr pqueue_;
};

class _Queue :
	public _Queue_base {
public:
	using ptr = ::std::shared_ptr<_Queue>;

	_Queue(size_type);

	bool push(task_ptr&&) override;

	task_ptr pop() override;

	bool try_pop(task_ptr&) override;
private:
	::std::deque<task_ptr> queue_;
};

class _Queue_priority :
	public _Queue_base {
public:
	using ptr = ::std::shared_ptr<_Queue_priority>;

	_Queue_priority(size_type);

	bool push(task_ptr&& task) override;

	task_ptr pop() override;

	bool try_pop(task_ptr& task) override;
private:
	struct _Comparator {
		bool operator()(const task_ptr& lhs, const task_ptr& rhs) const {
			return lhs->get_priority() < rhs->get_priority();
		}
	};

	::std::priority_queue<task_ptr, ::std::vector<task_ptr>, _Comparator> queue_;
};

_SW_END // _SW_BEGIN

#include "./sw_queue_core.ipp"

#ifdef _SW_HEADER_ONLY_
	#include "../src/sw_queue_core.cc"
#endif // _SW_HEADER_ONLY_

#endif // _SW_QUEUE_CORE_H_
