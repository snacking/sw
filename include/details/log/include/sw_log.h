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
#ifdef __cpp_lib_filesystem
#include <filesystem>
#endif // __cpp_lib_filesystem

#define DEFAULT_CONFIG_FILE "sw_log.properties"

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

    static const ::std::unordered_map<::std::string, level> _String_to_level;

    static ::std::string to_string(log_level::level);
    static level from_string(const ::std::string&);
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

    class _Loggername_fotmatter_item : public _Formatter_item {
    public:
        _Loggername_fotmatter_item(const ::std::string&);
        void format(::std::ostream&, ::std::shared_ptr<logger>, log_level::level, log_event::ptr) override;
    };

    class _Threadid_fotmatter_item : public _Formatter_item {
    public:
        _Threadid_fotmatter_item(const ::std::string&);
        void format(::std::ostream&, ::std::shared_ptr<logger>, log_level::level, log_event::ptr) override;
    };

    class _Coroutineid_fotmatter_item : public _Formatter_item {
    public:
        _Coroutineid_fotmatter_item(const ::std::string&);
        void format(::std::ostream&, ::std::shared_ptr<logger>, log_level::level, log_event::ptr) override;
    };

    class _Datetime_fotmatter_item : public _Formatter_item {
    public:
        _Datetime_fotmatter_item(const ::std::string&);
        void format(::std::ostream&, ::std::shared_ptr<logger>, log_level::level, log_event::ptr) override;
    private:
    };

    class _Filename_fotmatter_item : public _Formatter_item {
    public:
        _Filename_fotmatter_item(const ::std::string&);
        void format(::std::ostream&, ::std::shared_ptr<logger>, log_level::level, log_event::ptr) override;
    };

    class _Line_fotmatter_item : public _Formatter_item {
    public:
        _Line_fotmatter_item(const ::std::string&);
        void format(::std::ostream&, ::std::shared_ptr<logger>, log_level::level, log_event::ptr) override;
    };

    class _Newline_fotmatter_item : public _Formatter_item {
    public:
        _Newline_fotmatter_item(const ::std::string&);
        void format(::std::ostream&, ::std::shared_ptr<logger>, log_level::level, log_event::ptr) override;
    };

    class _Cstr_fotmatter_item : public _Formatter_item {
    public:
        _Cstr_fotmatter_item(const ::std::string&);
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

class stream_log_appender : public log_appender {
public:
    using ptr = ::std::shared_ptr<stream_log_appender>;

    explicit stream_log_appender(::std::ostream &);
    ~stream_log_appender() = default;

    void log(::std::shared_ptr<logger>, log_level::level, log_event::ptr) override;
private:
    ::std::ostream &out_;
};

class logger : public ::std::enable_shared_from_this<logger> {
public:
    using ptr = ::std::shared_ptr<logger>;

    explicit logger();
    explicit logger(const char *);
    explicit logger(const std::string &);
#ifdef __cpp_lib_filesystem
    explicit logger(const ::std::filesystem::path &);
#endif // __cpp_lib_filesystem
    void log(log_level::level, log_event::ptr);
    void debug(log_event::ptr);
    void info(log_event::ptr);
    void warn(log_event::ptr);
    void error(log_event::ptr);
    void fatal(log_event::ptr);
    void add_appender(log_appender::ptr);
    void delete_appender(log_appender::ptr);
    void set_name(const ::std::string &);
    void set_level(log_level::level);
    log_level::level get_level() const;
    const ::std::string& get_name() const;
private:
    template <typename _Pt>
    void _Read_config_file(const _Pt& fp);

    ::std::string name_;
    log_level::level level_;
    ::std::list<log_appender::ptr> appenders_;
};

_SW_END // _SW_BEGIN

#include "./sw_log.ipp"

#ifdef _SW_HEADER_ONLY_
    #include "../src/log.cc"
#endif // _SW_HEADER_ONLY_

#endif // _SW_LOG_H_
