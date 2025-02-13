// header file for sw::time

#ifndef _SW_TIME_H_
#define _SW_TIME_H_

#include "sw_vals.h"

#include <chrono>
#include <cstdint>
#include <functional>
#include <string>

_SW_BEGIN

template <typename _Ct,
    typename _Dr = ::std::chrono::milliseconds>
class stopwatch {
public:
    stopwatch(bool auto_start = true) _SW_NOEXCEPT;

    ~stopwatch() _SW_NOEXCEPT = default;

    inline void start() _SW_NOEXCEPT;

    inline void pause() _SW_NOEXCEPT;

    inline void resume() _SW_NOEXCEPT;

    ::std::uint64_t elapsed() const _SW_NOEXCEPT;
private:
    ::std::uint64_t accumulated_duration_;
    typename _Ct::time_point start_;
    bool paused_;
};

template <typename _Ct, 
    typename _Dr = ::std::chrono::milliseconds,
        typename _Cb = ::std::function<void(const ::std::string&, ::std::uint64_t)> >
class counter {
public:
    using stopwatch_type = stopwatch<_Ct, _Dr>;
    using callback_type = _Cb;

    explicit counter(const std::string &, callback_type &&) _SW_NOEXCEPT;

    ~counter() _SW_NOEXCEPT;
private:
    stopwatch_type stopwatch_;
    callback_type callback_;
    ::std::string key_;
};

_SW_END

#include "internal/sw_time.ipp"

#endif // _SW_TIME_H_
