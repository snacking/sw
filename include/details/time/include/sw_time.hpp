// header file for sw::time

#ifndef _SW_TIME_H_
#define _SW_TIME_H_

#include "../../sw_vals.h"

#include <chrono>
#include <cstdint>
#include <string>
#include <functional>

_SW_BEGIN

template <typename _Ct>
class stopwatch {
public:
    stopwatch() noexcept : 
        accumulated_duration_(0), start_(), paused_(false) {}

    ~stopwatch() = default;

    inline void start() {
        accumulated_duration_ = 0;
        start_ = _Ct::now();
        paused_ = false;
    }

    inline void pause() {
        accumulated_duration_ += elapsed();
        paused_ = true;
    }

    inline void resume() {
        start_ = _Ct::now();
        paused_ = false;
    }

    ::std::uint64_t elapsed() const {
        if (paused_) {
            return accumulated_duration_;
        }
        return accumulated_duration_ + ::std::chrono::duration_cast<::std::chrono::microseconds>
            (_Ct::now() - start_).count();
    }
private:
    ::std::uint64_t accumulated_duration_;
    typename _Ct::time_point start_;
    bool paused_;
};

template <typename _Ct, 
    typename _Cb = ::std::function<void(const ::std::string&, ::std::uint64_t)> >
class counter {
public:
    using stopwatch_type = stopwatch<_Ct>;
    using callback_type = _Cb;

    explicit counter(const std::string &key, callback_type &&callback) :
        key_(key), callback_(callback) {
        stopwatch_.start();
    }

    ~counter() {
        callback_(key_, stopwatch_.elapsed());
    }
private:
    stopwatch_type stopwatch_;
    callback_type callback_;
    ::std::string key_;
};

_SW_END

#endif // _SW_TIME_H_
