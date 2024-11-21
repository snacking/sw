// header file for sw::logger

#ifndef _SW_LOG_H_
#define _SW_LOG_H_

#include "../../sw_vals.h"

#include "../../time/include/sw_time.hpp"

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
#include <unordered_map>
#include <functional>
#include <thread>
#include <ctime>
#include <iomanip>

_SW_BEGIN

class logger;

struct log_level {
    enum class level {
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

    static ::std::string to_string(log_level::level);
};

class log_event {
public:
    using ptr = ::std::shared_ptr<log_event>;

    log_event(const char *, const char *, ::std::uint32_t, const std::string &);

#define EVENT(content) \ 
    ::std::make_shared<sw::log_event>(__FILE__, __FUNC_NAME__, __LINE__, content)

    const char *get_file() const;
    const char *get_func() const;
    ::std::uint64_t get_elapsed() const;
    ::std::uint32_t get_line() const;
    ::std::thread::id get_thread_id() const;
    ::std::string get_coroutine_id() const;
    ::std::tm *get_time() const;
    ::std::string get_content() const;
private:
    const char *file_, *func_;
    stopwatch<::std::chrono::steady_clock> sw_;
    ::std::uint32_t line_;
    ::std::thread::id thread_id_;
    ::std::string coroutine_id_;
    ::std::tm *time_;
    ::std::string content_;
};

class log_formatter {
public:
    using ptr = ::std::shared_ptr<log_formatter>;

    log_formatter(const ::std::string&);

    void init();
    ::std::string format(::std::shared_ptr<logger>, log_level::level, log_event::ptr);
private:
    class _Formatter_item {
    public:
        using ptr = ::std::shared_ptr<_Formatter_item>;
        
        _Formatter_item(const ::std::string&);
        virtual ~_Formatter_item() = default;
        virtual void format(::std::ostream&, ::std::shared_ptr<logger>, log_level::level, log_event::ptr) = 0;
    protected:
        const ::std::string format_;
    };

    class _Message_fotmatter_item : public _Formatter_item {
    public:
        _Message_fotmatter_item(const ::std::string&);
        void format(::std::ostream&, ::std::shared_ptr<logger>, log_level::level, log_event::ptr) override;
    };
    
    class _Level_fotmatter_item : public _Formatter_item {
    public:
        _Level_fotmatter_item(const ::std::string&);
        void format(::std::ostream&, ::std::shared_ptr<logger>, log_level::level, log_event::ptr) override;
    };
    
    class _Elapsed_fotmatter_item : public _Formatter_item {
    public:
        _Elapsed_fotmatter_item(const ::std::string&);
        void format(::std::ostream&, ::std::shared_ptr<logger>, log_level::level, log_event::ptr) override;
    };

    class _LoggerName_fotmatter_item : public _Formatter_item {
    public:
        _LoggerName_fotmatter_item(const ::std::string&);
        void format(::std::ostream&, ::std::shared_ptr<logger>, log_level::level, log_event::ptr) override;
    };

    class _ThreadId_fotmatter_item : public _Formatter_item {
    public:
        _ThreadId_fotmatter_item(const ::std::string&);
        void format(::std::ostream&, ::std::shared_ptr<logger>, log_level::level, log_event::ptr) override;
    };

    class _CoroutineId_fotmatter_item : public _Formatter_item {
    public:
        _CoroutineId_fotmatter_item(const ::std::string&);
        void format(::std::ostream&, ::std::shared_ptr<logger>, log_level::level, log_event::ptr) override;
    };

    class _DateTime_fotmatter_item : public _Formatter_item {
    public:
        _DateTime_fotmatter_item(const ::std::string&);
        void format(::std::ostream&, ::std::shared_ptr<logger>, log_level::level, log_event::ptr) override;
    private:
    };

    class _FileName_fotmatter_item : public _Formatter_item {
    public:
        _FileName_fotmatter_item(const ::std::string&);
        void format(::std::ostream&, ::std::shared_ptr<logger>, log_level::level, log_event::ptr) override;
    };

    class _Line_fotmatter_item : public _Formatter_item {
    public:
        _Line_fotmatter_item(const ::std::string&);
        void format(::std::ostream&, ::std::shared_ptr<logger>, log_level::level, log_event::ptr) override;
    };

    class _NewLine_fotmatter_item : public _Formatter_item {
    public:
        _NewLine_fotmatter_item(const ::std::string&);
        void format(::std::ostream&, ::std::shared_ptr<logger>, log_level::level, log_event::ptr) override;
    };

    class _CStr_fotmatter_item : public _Formatter_item {
    public:
        _CStr_fotmatter_item(const ::std::string&);
        void format(::std::ostream&, ::std::shared_ptr<logger>, log_level::level, log_event::ptr) override;
    private:
    };

    const ::std::string pattern_;
    ::std::vector<_Formatter_item::ptr> items_;
};

class log_appender {
public:
    using ptr = ::std::shared_ptr<log_appender>;

    virtual ~log_appender() = default;

    virtual void log(::std::shared_ptr<logger>, log_level::level, log_event::ptr) = 0;
    log_formatter::ptr get_formatter() const;
    void set_formatter(log_formatter::ptr);
protected:
    log_level::level level_;
    log_formatter::ptr pformatter_;
};

template <typename _St>
class stream_log_appender : public log_appender {
public:
    using ptr = ::std::shared_ptr<stream_log_appender<_St> >;

    explicit stream_log_appender(_St &out) noexcept : out_(out) {}
    ~stream_log_appender() = default;

    void log(::std::shared_ptr<logger> logger, log_level::level level, log_event::ptr event) override {
        if (level >= level_) {
            out_ << pformatter_->format(logger, level, event);
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
                REGISTER_SW_ERROR(4, "failed to open file");
                throw ::std::runtime_error("failed to open file: " + filename);
            }
    }
    ~stream_log_appender() {
        file_.close();
    }
    
    void log(::std::shared_ptr<logger> logger, log_level::level level, log_event::ptr event) override {
        if (level >= level_) {
            file_ << pformatter_->format(logger, level, event);
        }
    }
private:
    ::std::ofstream file_;
};

using ostream_appender = stream_log_appender<::std::ostream>;
using ofstream_appender = stream_log_appender<::std::ofstream>;

class logger : public ::std::enable_shared_from_this<logger> {
public:
    using ptr = ::std::shared_ptr<logger>;

    explicit logger(const char *);
    explicit logger(const std::string &);
#ifdef __cpp_lib_filesystem
    #include <filesystem>
    explicit logger(const ::std::filesystem::path &);
#endif // __cpp_lib_filesystem
#ifdef __cpp_lib_string_view
    #include <string_view>
    explicit logger(const ::std::string_view &);
#endif // __cpp_stringview

    void log(log_level::level, log_event::ptr);
    void debug(log_event::ptr);
    void info(log_event::ptr);
    void warn(log_event::ptr);
    void error(log_event::ptr);
    void fatal(log_event::ptr);
    void add_appender(log_appender::ptr);
    void delete_appender(log_appender::ptr);
    void set_level(log_level::level);
    log_level::level get_level() const;
    const ::std::string& get_name() const;
private:
    template <typename _Pt>
    void _Read_config_file(const _Pt&);

    ::std::string name_;
    log_level::level level_;
    ::std::list<log_appender::ptr> appenders_;
};

_SW_END

#include "./sw_log.ipp"

#ifdef _SW_HEADER_ONLY_
    #include "../src/log.cc"
#endif // _SW_HEADER_ONLY_

#endif // _SW_LOG_H_
