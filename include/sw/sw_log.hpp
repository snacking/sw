// header file for sw::logger

#ifndef _SW_LOG_H_
#define _SW_LOG_H_

#include "internal/sw_vals.h"

#include "sw_properties.hpp"
#include "sw_time.hpp"

#include <array>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <ctime>
#ifdef __cpp_lib_filesystem
#include <filesystem>
#endif // __cpp_lib_filesystem
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <list>
#include <memory>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <vector>

#define _SW_LOG_VER 1

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
        NONE
    };

    static ::std::string to_string(log_level::level) _SW_NOEXCEPT;

    static level from_string(const ::std::string&) _SW_NOEXCEPT;
};

class log_event {
public:
    using ptr = ::std::shared_ptr<log_event>;

    log_event(const char*, const char*, ::std::uint32_t, const std::string&) _SW_NOEXCEPT;

#define EVENT(content) \
    ::std::make_shared<sw::log_event>(__FILE__, __FUNC_NAME__, __LINE__, content)

    const char* get_file() const _SW_NOEXCEPT;

    const char* get_func() const _SW_NOEXCEPT;

    ::std::uint64_t get_elapsed() const _SW_NOEXCEPT;

    ::std::uint32_t get_line() const _SW_NOEXCEPT;

    ::std::thread::id get_thread_id() const _SW_NOEXCEPT;
    
    ::std::string get_coroutine_id() const _SW_NOEXCEPT;

    ::std::tm* get_time() const _SW_NOEXCEPT;

    ::std::string get_content() const _SW_NOEXCEPT;
private:
    const char* file_, *func_;
    stopwatch<::std::chrono::steady_clock> sw_;
    ::std::uint32_t line_;
    ::std::thread::id thread_id_;
    ::std::string coroutine_id_;
    ::std::tm* time_;
    ::std::string content_;
};

class log_formatter {
public:
    using ptr = ::std::shared_ptr<log_formatter>;

    virtual ~log_formatter() _SW_NOEXCEPT = default;

    virtual ::std::string format(::std::shared_ptr<logger>, log_level::level, log_event::ptr) = 0;
};

class pattern_log_formatter : public log_formatter {
public:
    pattern_log_formatter(const ::std::string&) _SW_NOEXCEPT;

    ::std::string format(::std::shared_ptr<logger>, log_level::level, log_event::ptr) _SW_NOEXCEPT override;
private:
    class _Formatter_item {
    public:
        using ptr = ::std::shared_ptr<_Formatter_item>;
        
        _Formatter_item(const ::std::string&) _SW_NOEXCEPT;

        virtual ~_Formatter_item() _SW_NOEXCEPT = default;

        virtual void format(::std::ostream&, ::std::shared_ptr<logger>, log_level::level, log_event::ptr) = 0;
    protected:
        const ::std::string format_;
    };

    class _Message_fotmatter_item : public _Formatter_item {
    public:
        _Message_fotmatter_item(const ::std::string&) _SW_NOEXCEPT;

        void format(::std::ostream&, ::std::shared_ptr<logger>, log_level::level, log_event::ptr) _SW_NOEXCEPT override;
    };
    
    class _Level_fotmatter_item : public _Formatter_item {
    public:
        _Level_fotmatter_item(const ::std::string&) _SW_NOEXCEPT;

        void format(::std::ostream&, ::std::shared_ptr<logger>, log_level::level, log_event::ptr) _SW_NOEXCEPT override;
    };
    
    class _Elapsed_fotmatter_item : public _Formatter_item {
    public:
        _Elapsed_fotmatter_item(const ::std::string&) _SW_NOEXCEPT;

        void format(::std::ostream&, ::std::shared_ptr<logger>, log_level::level, log_event::ptr) _SW_NOEXCEPT override;
    };

    class _Loggername_fotmatter_item : public _Formatter_item {
    public:
        _Loggername_fotmatter_item(const ::std::string&) _SW_NOEXCEPT;

        void format(::std::ostream&, ::std::shared_ptr<logger>, log_level::level, log_event::ptr) _SW_NOEXCEPT override;
    };

    class _Threadid_fotmatter_item : public _Formatter_item {
    public:
        _Threadid_fotmatter_item(const ::std::string&) _SW_NOEXCEPT;

        void format(::std::ostream&, ::std::shared_ptr<logger>, log_level::level, log_event::ptr) _SW_NOEXCEPT override;
    };

    class _Coroutineid_fotmatter_item : public _Formatter_item {
    public:
        _Coroutineid_fotmatter_item(const ::std::string&) _SW_NOEXCEPT;

        void format(::std::ostream&, ::std::shared_ptr<logger>, log_level::level, log_event::ptr) _SW_NOEXCEPT override;
    };

    class _Datetime_fotmatter_item : public _Formatter_item {
    public:
        _Datetime_fotmatter_item(const ::std::string&) _SW_NOEXCEPT;

        void format(::std::ostream&, ::std::shared_ptr<logger>, log_level::level, log_event::ptr) _SW_NOEXCEPT override;
    };

    class _Filename_fotmatter_item : public _Formatter_item {
    public:
        _Filename_fotmatter_item(const ::std::string&) _SW_NOEXCEPT;

        void format(::std::ostream&, ::std::shared_ptr<logger>, log_level::level, log_event::ptr) _SW_NOEXCEPT override;
    };

    class _Line_fotmatter_item : public _Formatter_item {
    public:
        _Line_fotmatter_item(const ::std::string&) _SW_NOEXCEPT;

        void format(::std::ostream&, ::std::shared_ptr<logger>, log_level::level, log_event::ptr) _SW_NOEXCEPT override;
    };

    class _Newline_fotmatter_item : public _Formatter_item {
    public:
        _Newline_fotmatter_item(const ::std::string&) _SW_NOEXCEPT;

        void format(::std::ostream&, ::std::shared_ptr<logger>, log_level::level, log_event::ptr) _SW_NOEXCEPT override;
    };

    class _Cstr_fotmatter_item : public _Formatter_item {
    public:
        _Cstr_fotmatter_item(const ::std::string&) _SW_NOEXCEPT;

        void format(::std::ostream&, ::std::shared_ptr<logger>, log_level::level, log_event::ptr) _SW_NOEXCEPT override;
    };

    void _Parse_pattern() _SW_NOEXCEPT;

    const ::std::string pattern_;
    ::std::vector<_Formatter_item::ptr> items_;
};

enum class formatter_type : ::std::uint8_t {
    PATTERN_FORMATTER,
    NONE
};

class log_appender {
public:
    using ptr = ::std::shared_ptr<log_appender>;

    virtual ~log_appender() _SW_NOEXCEPT = default;

    virtual void log(::std::shared_ptr<logger>, log_level::level, log_event::ptr) = 0;

    log_formatter::ptr get_formatter() const _SW_NOEXCEPT;

    void set_formatter(log_formatter::ptr) _SW_NOEXCEPT;

    void set_level(log_level::level) _SW_NOEXCEPT;

    log_level::level get_level() const _SW_NOEXCEPT;
protected:
    log_level::level level_;
    log_formatter::ptr pformatter_;
};

class stream_log_appender : public log_appender {
public:
    explicit stream_log_appender(::std::ostream&) _SW_NOEXCEPT;
    
    ~stream_log_appender() _SW_NOEXCEPT = default;

    void log(::std::shared_ptr<logger>, log_level::level, log_event::ptr) _SW_NOEXCEPT override;
private:
    ::std::ostream& out_;
};

class fstream_log_appender : public log_appender {
public:
    explicit fstream_log_appender(const ::std::string&) _SW_DES(::std::runtime_error);
    
    ~fstream_log_appender() _SW_NOEXCEPT;

    void log(::std::shared_ptr<logger>, log_level::level, log_event::ptr) _SW_NOEXCEPT override;
private:
    ::std::ofstream out_;
};


class rolling_fstream_log_appender : public log_appender {
public:
    using size_type = ::std::size_t;

    explicit rolling_fstream_log_appender(const ::std::string&) _SW_DES(::std::runtime_error);
    
    ~rolling_fstream_log_appender() _SW_NOEXCEPT;

    void log(::std::shared_ptr<logger>, log_level::level, log_event::ptr) _SW_NOEXCEPT override;
private:
    size_type current_size_, max_size_;
    ::std::ofstream out_;
};

enum class appender_type : ::std::uint8_t {
    STREAM_APPENDER,
    FSTREAM_APPENDER,
    ROLLING_FSTREAM_APPENDER,
    NONE
};

class logger : public ::std::enable_shared_from_this<logger> {
public:
    using ptr = ::std::shared_ptr<logger>;

    static ptr get_root_logger() _SW_NOEXCEPT;

    static ptr get_logger(const std::string&) _SW_NOEXCEPT;

    static void configure() _SW_NOEXCEPT;

    static void configure(const char*) _SW_DES(::std::runtime_error);

    static void configure(const std::string&) _SW_DES(::std::runtime_error);

#ifdef __cpp_lib_filesystem
    static void configure(const ::std::filesystem::path&) _SW_DES(::std::runtime_error);
#endif // __cpp_lib_filesystem

    void log(log_level::level, log_event::ptr) _SW_NOEXCEPT;

    void debug(log_event::ptr) _SW_NOEXCEPT;

    void info(log_event::ptr) _SW_NOEXCEPT;

    void warn(log_event::ptr) _SW_NOEXCEPT;

    void error(log_event::ptr) _SW_NOEXCEPT;

    void fatal(log_event::ptr) _SW_NOEXCEPT;

    void add_appender(const ::std::string&, log_appender::ptr) _SW_NOEXCEPT;

    void delete_appender(const ::std::string&) _SW_NOEXCEPT;

    void set_name(const ::std::string&) _SW_NOEXCEPT;

    void set_level(log_level::level) _SW_NOEXCEPT;

    log_level::level get_level() const _SW_NOEXCEPT;

    const ::std::string& get_name() const _SW_NOEXCEPT;
private:
    class _Properties_parser {
    public:
        _Properties_parser(const char*) _SW_DES(::std::runtime_error);

        _Properties_parser(const std::string&) _SW_DES(::std::runtime_error);

#ifdef __cpp_lib_filesystem
        _Properties_parser(const ::std::filesystem::path&) _SW_DES(::std::runtime_error);
#endif // __cpp_lib_filesystem

        ~_Properties_parser() _SW_NOEXCEPT = default;
    private:
        enum class state {
            INIT,
            SW_LOG,
            LOGGER,
            APPENDER,
            FORMATTER,
            FAILED,
            FINISHED
        };
        
        struct appender_meta {
            appender_meta() _SW_NOEXCEPT : at(appender_type::NONE), ll(log_level::level::NONE), ft(formatter_type::NONE) {}
            appender_type at;
            log_level::level ll;
            formatter_type ft;
        };

        template <typename _Pt>
        void _Load_properties(_Pt) _SW_DES(::std::runtime_error);

        void _Parse() _SW_NOEXCEPT;

        state _Parse_init(const ::std::string&, ::std::string&) _SW_NOEXCEPT;

        state _Parse_sw_log(const ::std::string&, ::std::size_t, ::std::string&, ::std::unordered_map<::std::string, appender_meta> &, const ::std::string&) _SW_NOEXCEPT;

        state _Parse_logger(const ::std::string&, ::std::size_t, const ::std::string&, ::std::string&, ::std::unordered_map<::std::string, appender_meta> &,const ::std::string&) _SW_NOEXCEPT;

        state _Parse_appender(const ::std::string&, ::std::size_t, const ::std::string&, ::std::string&, ::std::unordered_map<::std::string, appender_meta> &,const ::std::string&) _SW_NOEXCEPT;

        state _Parse_formatter(const ::std::string&, ::std::size_t, const ::std::string&, ::std::string&, ::std::unordered_map<::std::string, appender_meta> &,const ::std::string&) _SW_NOEXCEPT;

        properties properties_;
    };

    static void _Init_root_logger() _SW_NOEXCEPT;

    explicit logger(const ::std::string&) _SW_NOEXCEPT;

    logger(const logger&) _SW_NOEXCEPT;

    bool _Is_complete_logger() const _SW_NOEXCEPT;

    static ::std::unordered_map<::std::string, ptr>& _Get_internal_loggers_map() _SW_NOEXCEPT;

    ::std::string name_;
    log_level::level level_;
    ::std::unordered_map<::std::string, log_appender::ptr> pappenders_;
};

_SW_END // _SW_BEGIN

#include "internal/sw_log.ipp"

#endif // _SW_LOG_H_
