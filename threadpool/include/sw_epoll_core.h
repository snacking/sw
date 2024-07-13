// header file for sw::_Epoll_wrapper (internal)

#ifndef _SW_EPOLL_CORE_H_
#define _SW_EPOLL_CORE_H_

#include "../../include/sw_vals.h"
#include <unistd.h>
#include <sys/epoll.h>
#include <cstddef>
#include <functional>
#include <cassert>
#include <cstdint>
#include <unordered_map>
#include <stdexcept>

_SW_BEGIN

class _Epoll_wrapper {
public:
	using size_type = ::std::size_t;
	using callback_type = ::std::function<void(int, ::std::uint32_t)>;

	_Epoll_wrapper(size_type);

	~_Epoll_wrapper();

	void add_fd(int, ::std::uint32_t, callback_type);

	void del_fd(int);

	void mod_fd(int, ::std::uint32_t);

	void run();

	inline void stop();
private:
	void _Init();

	void _Close();

	size_type max_events_;
	int epoll_fd_;
	::std::unordered_map<int, ::std::pair<::std::uint32_t, callback_type> > callbacks_;
	bool state_;
};

_SW_END // _SW_BEGIN

#endif // _SW_EPOLL_CORE_H_
