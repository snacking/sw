// header file for sw::_Queue_base and derived (internal)

#ifndef _SW_QUEUE_CORE_H_
#define _SW_QUEUE_CORE_H_

#include "sw/internal/sw_vals.h"
#include "sw/internal/sw_task_core.hpp"

#include <cstddef>
#include <deque>
#include <memory>
#include <mutex>
#include <queue>

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

	_Queue_base(size_type) _SW_NOEXCEPT;

	virtual ~_Queue_base() _SW_NOEXCEPT = default;

	virtual bool push(task_ptr&&) = 0;

	virtual task_ptr pop() = 0;

	virtual bool try_pop(task_ptr&) = 0;

	inline bool empty() const _SW_NOEXCEPT;

	inline size_type size() const _SW_NOEXCEPT;

	void set_handler(handler_ptr&&) _SW_NOEXCEPT;
protected:
	mutable ::std::mutex mutex_;
	volatile size_type size_, capacity_;
	handler_ptr phandler_;
};

class _Reject_handler_ignore : 
	public _Reject_handler_base {
public:
	void reject(task_ptr&&) _SW_NOEXCEPT override;
};

class _Reject_handler_ignore_throw : 
	public _Reject_handler_base {
public:
	void reject(task_ptr&&) _SW_DES(::std::runtime_error) override;
};

class _Reject_handler_delete_oldest : 
	public _Reject_handler_base {
public:
	_Reject_handler_delete_oldest(_Queue_base::ptr) _SW_NOEXCEPT;
	void reject(task_ptr&&)  _SW_DES(::std::runtime_error) override;
private:
	_Queue_base::ptr pqueue_;
};

class _Queue :
	public _Queue_base {
public:
	using ptr = ::std::shared_ptr<_Queue>;

	_Queue(size_type) _SW_NOEXCEPT;

	bool push(task_ptr&&) _SW_NOEXCEPT override;

	task_ptr pop() _SW_DES(::std::runtime_error) override;

	bool try_pop(task_ptr&) _SW_NOEXCEPT override;
private:
	::std::deque<task_ptr> queue_;
};

class _Queue_priority :
	public _Queue_base {
public:
	using ptr = ::std::shared_ptr<_Queue_priority>;

	_Queue_priority(size_type) _SW_NOEXCEPT;

	bool push(task_ptr&& task) _SW_NOEXCEPT override;

	task_ptr pop() _SW_DES(::std::runtime_error) override;

	bool try_pop(task_ptr& task) _SW_NOEXCEPT override;
private:
	struct _Comparator {
		bool operator()(const task_ptr&, const task_ptr&) const _SW_NOEXCEPT;
	};

	::std::priority_queue<task_ptr, ::std::vector<task_ptr>, _Comparator> queue_;
};

_SW_END // _SW_BEGIN

#include "sw/internal/sw_queue_core.ipp"

#endif // _SW_QUEUE_CORE_H_
