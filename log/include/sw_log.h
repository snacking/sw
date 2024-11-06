// header file for sw::logger

#ifndef _SW_LOG_H_
#define _SW_LOG_H_

#include "../../include/sw_vals.h"
#include "../time/include/sw_clock.hpp"

#include <cstdint>
#include <string>
#include <memory>
#include <list>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <tuple>
#include <stdexcept>
#include <cctype>
#include <array>

_SW_BEGIN

struct log_level {
    enum class _Level {
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
    };

    static constexpr ::std::array<const char*, 5> _Level_to_string = {
        "DEBUG",
        "INFO",
        "WARN",
        "ERROR",
        "FATAL"
    };

    log_level(_Level);

    bool operator >= (const log_level&) const;

    ::std::string to_string() const;

    _Level level;
};

class log_event {
public:
    using ptr = ::std::shared_ptr<log_event>;

    const char *get_file() const;
    ::std::uint64_t get_elapsed() const;
    ::std::uint32_t get_line() const;
    ::std::uint32_t get_thread_id() const;
    ::std::uint32_t get_coroutine_id() const;
    ::std::uint64_t get_time() const;
    ::std::string get_content() const;
private:
    const char *file_;
    clock<::std::chrono::steady_clock>::ptr pclock_;
    ::std::uint32_t line_;
    ::std::uint32_t thread_id_;
    ::std::uint32_t coroutine_id_;
    ::std::uint64_t time_;
    ::std::string content_;
};

class log_formatter {
public:
    using ptr = ::std::shared_ptr<log_formatter>;

    log_formatter(const ::std::string&);

    void init();
    ::std::string format(log_event::ptr);
private:
    class _Formatter_item {
    public:
        using ptr = ::std::shared_ptr<_Formatter_item>;
        
        virtual ~_Formatter_item() = default;
        virtual void format(::std::ostream&, log_event::ptr) = 0;
    };

    class _Message_fotmatter_item : public _Formatter_item {
    public:
        void format(::std::ostream&, log_event::ptr);
    };
    
    class _Level_fotmatter_item : public _Formatter_item {
    public:
        void format(::std::ostream&, log_event::ptr);
    };
    
    class _fotmatter_item : public _Formatter_item {
    public:
        void format(::std::ostream&, log_event::ptr);
    };

    const ::std::string pattern_;
    ::std::vector<_Formatter_item::ptr> items_;
};

class log_appender {
public:
    using ptr = ::std::shared_ptr<log_appender>;

    virtual ~log_appender() = default;

    virtual void log(log_level, log_event::ptr) = 0;
    log_formatter::ptr get_formatter() const;
    void set_formatter(log_formatter::ptr);
protected:
    log_level level_;
    log_formatter::ptr pformatter_;
};

template <typename _St>
class stream_log_appender : public log_appender {
public:
    using ptr = ::std::shared_ptr<stream_log_appender<_St> >;

    explicit stream_log_appender(_St &out) noexcept : out_(out) {}
    ~stream_log_appender() = default;

    void log(log_level level, log_event::ptr event) override {
        if (level >= level_) {
            out_ << pformatter_->format(event);
        }
    }
private:
    _St &out_;
};

template <>
class stream_log_appender<::std::ofstream> : public log_appender {
public:
    using ptr = ::std::shared_ptr<stream_log_appender<::std::ofstream> >;

    explicit stream_log_appender(const ::std::string& filename) : 
        file_(filename, ::std::ios::app) {
            if (!file_.is_open()) {
                throw ::std::runtime_error("failed to open file: " + filename);
            }
    }
    ~stream_log_appender() {
        file_.close();
    }
    
    void log(log_level level, log_event::ptr event) override {
        if (level >= level_) {
            file_ << pformatter_->format(event);
        }
    }
private:
    ::std::ofstream file_;
};

using ostream_appender = stream_log_appender<::std::ostream>;
using ofstream_appender = stream_log_appender<::std::ofstream>;

class logger {
public:
    using ptr = ::std::shared_ptr<logger>;

    void log(log_level, log_event::ptr);
    void debug(log_event::ptr);
    void info(log_event::ptr);
    void warn(log_event::ptr);
    void error(log_event::ptr);
    void fatal(log_event::ptr);
    void add_appender(log_appender::ptr);
    void delete_appender(log_appender::ptr);
    void set_level(log_level);
    log_level get_level() const;
private:
    ::std::string name_;
    log_level level_;
    ::std::list<log_appender::ptr> appenders_;
};

_SW_END

#endif // _SW_LOG_H_
