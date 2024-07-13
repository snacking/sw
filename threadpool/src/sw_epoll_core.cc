// implementation file for sw::_Epoll_wrapper (internal)

#include "../include/sw_epoll_core.h"

_SW_BEGIN

_Epoll_wrapper::_Epoll_wrapper(size_type max_events = 1024) : max_events_(max_events), state_(false) {
    _Init();
}

_Epoll_wrapper::~_Epoll_wrapper() {
    _Close();
}

void _Epoll_wrapper::add_fd(int fd, ::std::uint32_t events, callback_type callback) {
    assert(fd >= 0);
    struct epoll_event _Ev;
    _Ev.data.fd = fd;
    _Ev.events = events | EPOLLET;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &_Ev) == -1) {
        throw ::std::runtime_error("add_fd failed");
    }
    callbacks_.insert_or_assign(fd, ::std::make_pair(events, callback));
    return;
}

void _Epoll_wrapper::del_fd(int fd) {
    assert(fd >= 0);
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr) == -1) {
        throw ::std::runtime_error("del_fd failed");
    }
    callbacks_.erase(fd);
    return;
}

void _Epoll_wrapper::mod_fd(int fd, ::std::uint32_t events) {
    assert(fd >= 0);
    auto _It = callbacks_.find(fd);
    if (_It == callbacks_.end()) {
        throw ::std::runtime_error("mod_fd failed");
    }
    struct epoll_event _Ev;
    _Ev.data.fd = fd;
    _Ev.events = events | EPOLLET;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &_Ev) == -1) {
        throw ::std::runtime_error("mod_fd failed");
    }
    _It->second.first = events;
    return;
}

void _Epoll_wrapper::run() {
    struct epoll_event _Epoll_events[max_events_];
    while (state_) {
        int _N = epoll_wait(epoll_fd_, _Epoll_events, max_events_, -1);
        if (_N < 0) {
            throw ::std::runtime_error("run failed");
        }
        for (auto i = 0; i < _N; ++i) {
            auto _It = callbacks_.find(_Epoll_events[i].data.fd);
            if (_It == callbacks_.end()) {
                throw ::std::runtime_error("No callback registered for fd");
            }
            const auto& _Callback_pair = _It->second;
            ::std::uint32_t _Events = _Callback_pair.first;
            callback_type _Callback = _Callback_pair.second;
            if ((_Epoll_events[i].events & _Events) != 0) {
                _Callback(_Epoll_events[i].data.fd, _Epoll_events[i].events);
            }
        }
    }
    return;
}

inline void _Epoll_wrapper::stop() {
    state_ = false;
    return;
}

void _Epoll_wrapper::_Init() {
    assert(max_events_ > 0);
    if ((epoll_fd_ = epoll_create1(EPOLL_CLOEXEC)) == -1) {
        throw ::std::runtime_error("init epoll failed");
    }
    state_ = true;
    return;
}

void _Epoll_wrapper::_Close() {
    close(epoll_fd_);
    return;
}

_SW_END // _SW_BEGIN
