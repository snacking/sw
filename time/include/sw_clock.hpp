// header file for sw::logger

#ifndef _SW_CLOCK_H_
#define _SW_CLOCK_H_

#include "../../include/sw_vals.h"

#include <chrono>
#include <cstdint>
#include <memory>

_SW_BEGIN

template <typename _Ct>
class clock {
public:
    using ptr = ::std::shared_ptr<clock<_Ct> >;

    clock() noexcept : accumulated_duration_(0), start_(), paused_(false) {}
    ~clock() = default;

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
        return accumulated_duration_ + ::std::chrono::duration_cast<::std::chrono::microseconds>(
            _Ct::now() - start_).count();
    }
private:
    ::std::uint64_t accumulated_duration_;
    typename _Ct::time_point start_;
    bool paused_;
};

_SW_END

#endif // _SW_CLOCK_H_
