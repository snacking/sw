// header file for sw::time

#ifndef _SW_TIME_H_
#define _SW_TIME_H_

#include "../../sw_vals.h"

#include <chrono>
#include <cstdint>
#include <string>
#include <functional>

_SW_BEGIN

template <typename _Ct,
    typename _Dr = ::std::chrono::milliseconds>
class stopwatch {
public:
    stopwatch(bool auto_start = true) _NOEXCEPT;

    ~stopwatch() _NOEXCEPT = default;

    inline void start() _NOEXCEPT;

    inline void pause() _NOEXCEPT;

    inline void resume() _NOEXCEPT;

    ::std::uint64_t elapsed() const _NOEXCEPT;
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

    explicit counter(const std::string &, callback_type &&) _NOEXCEPT;

    ~counter() _NOEXCEPT;
private:
    stopwatch_type stopwatch_;
    callback_type callback_;
    ::std::string key_;
};

_SW_END

#include "./sw_time.ipp"

#endif // _SW_TIME_H_
