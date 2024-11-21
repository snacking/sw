// implementation file for sw_log.h

#include "../include/sw_log.h"

_SW_BEGIN

log_event::log_event(const std::string &content) :
    file_(__FILE__), func_(__FUNC_NAME__), line_(__LINE__), thread_id_(::std::this_thread::get_id()), content_(content) {
    auto now_time = ::std::chrono::system_clock::to_time_t(::std::chrono::system_clock::now());
    time_ = ::std::localtime(&now_time);
}

::std::string log_level::to_string(log_level::level level) {
    return _Level_to_string[static_cast<int>(level)];
}

const char *log_event::get_file() const {
    return file_;
}

::std::uint64_t log_event::get_elapsed() const {
    return sw_.elapsed();
}

::std::uint32_t log_event::get_line() const {
    return line_;
}

::std::thread::id log_event::get_thread_id() const {
    return thread_id_;
}

::std::string log_event::get_coroutine_id() const {
    return coroutine_id_;
}

::std::tm *log_event::get_time() const {
    return time_;
}

::std::string log_event::get_content() const {
    return content_;
}

log_formatter::log_formatter(const ::std::string &pattern) :
    pattern_(pattern) {}

void log_formatter::init() {
    ::std::vector<::std::tuple<::std::string, ::std::string, int> > items;
    ::std::string str;
    const ::std::size_t len = pattern_.size();
    for (std::size_t i = 0; i < len; ++i) {
        if ('%' != pattern_[i]) {
            str.push_back(pattern_[i]);
            continue;
        }
        ::std::size_t fmt_begin = 0;
        enum {
            NO_BRACE,
            BRACE_BEGIN,
            BRACE_END
        } fmt_status = NO_BRACE;
        while (i < len) {
            if (isspace(pattern_[i])) {
                break;
            }
            switch (fmt_status) {
            case NO_BRACE:
                if ('{' == pattern_[i]) {
                    fmt_status = BRACE_BEGIN;
                    continue;
                }
                break;
            case BRACE_BEGIN:
                if ('}' == pattern_[i]) {
                    fmt_status = BRACE_END;
                    continue;
                }
                break;
            case BRACE_END:
                break;
            }
        }
        switch (fmt_status) {
        case NO_BRACE:
            if ('{' == pattern_[i]) {
                fmt_status = BRACE_BEGIN;
                continue;
            }
            break;
        case BRACE_BEGIN:
            if ('}' == pattern_[i]) {
                fmt_status = BRACE_END;
                continue;
            }
            break;
        case BRACE_END:
            // items.emplace_back(::std::make_tuple());
            break;
        }
    }
    static ::std::unordered_map<::std::string, ::std::function<_Formatter_item::ptr(const ::std::string&)> > s_formatter_items = {
#define XX(str, T) \
    {#str, [](const ::std::string& fmt) { return _Formatter_item::ptr(new T(fmt)); }}
        XX(m, _Message_fotmatter_item),
        XX(p, _Level_fotmatter_item),
        XX(r, _Elapsed_fotmatter_item),
        XX(c, _LoggerName_fotmatter_item),
        XX(t, _ThreadId_fotmatter_item),
        XX(g, _CoroutineId_fotmatter_item),
        XX(n, _NewLine_fotmatter_item),
        XX(d, _DateTime_fotmatter_item),
        XX(f, _FileName_fotmatter_item),
        XX(l, _Line_fotmatter_item),
#undef XX
    };
}

::std::string log_formatter::format(logger::ptr logger, log_level::level level, log_event::ptr event) {
    ::std::stringstream ss;
    for (auto &p : items_) {
        p->format(ss, logger, level, event);
    }
    return ss.str();
}

log_formatter::_Formatter_item::_Formatter_item(const ::std::string& format) : format_(format) {}

void log_formatter::_Message_fotmatter_item::format(::std::ostream& os, logger::ptr logger, log_level::level level, log_event::ptr event) {
    os << event->get_content();
}

void log_formatter::_Level_fotmatter_item::format(::std::ostream& os, logger::ptr logger, log_level::level level, log_event::ptr event) {
    os << log_level::to_string(level);
}

void log_formatter::_Elapsed_fotmatter_item::format(::std::ostream& os, logger::ptr logger, log_level::level level, log_event::ptr event) {
    os << event->get_elapsed();
}

void log_formatter::_LoggerName_fotmatter_item::format(::std::ostream& os, logger::ptr logger, log_level::level level, log_event::ptr event) {
    os << logger->get_name();
}

void log_formatter::_ThreadId_fotmatter_item::format(::std::ostream& os, logger::ptr logger, log_level::level level, log_event::ptr event) {
    os << event->get_thread_id();
}

void log_formatter::_CoroutineId_fotmatter_item::format(::std::ostream& os, logger::ptr logger, log_level::level level, log_event::ptr event) {
    os << event->get_coroutine_id();
}

log_formatter::_DateTime_fotmatter_item::_DateTime_fotmatter_item(const std::string &format = "%Y:%m:%d %H:%M:%S") : _Formatter_item(format) {
}

void log_formatter::_DateTime_fotmatter_item::format(::std::ostream& os, logger::ptr logger, log_level::level level, log_event::ptr event) {
    os << ::std::put_time(event->get_time(), "%Y-%m-%d %H:%M:%S");
}

void log_formatter::_FileName_fotmatter_item::format(::std::ostream& os, logger::ptr logger, log_level::level level, log_event::ptr event) {
    os << event->get_file();
}

void log_formatter::_Line_fotmatter_item::format(::std::ostream& os, logger::ptr logger, log_level::level level, log_event::ptr event) {
    os << event->get_line();
}

void log_formatter::_NewLine_fotmatter_item::format(::std::ostream& os, logger::ptr logger, log_level::level level, log_event::ptr event) {
    os << ::std::endl;
}

log_formatter::_CStr_fotmatter_item::_CStr_fotmatter_item(const ::std::string& str) : _Formatter_item(str) {
}

void log_formatter::_CStr_fotmatter_item::format(::std::ostream& os, logger::ptr logger, log_level::level level, log_event::ptr event) {
    os << format_;
}


log_formatter::ptr log_appender::get_formatter() const {
    return pformatter_;
}

void log_appender::set_formatter(log_formatter::ptr formatter) {
    pformatter_ = formatter;
}

logger::logger(const std::string &name, log_level::level level) :
    name_(name), level_(level) {}

void logger::log(log_level::level level, log_event::ptr event) {
    if (level >= level_) {
        for (auto &appender : appenders_) {
            appender->log(shared_from_this(), level, event);
        }
    }
}

void logger::debug(log_event::ptr event) {
    log(log_level::level::DEBUG, event);
}

void logger::info(log_event::ptr event) {
    log(log_level::level::INFO, event);
}

void logger::warn(log_event::ptr event) {
    log(log_level::level::WARN, event);
}

void logger::error(log_event::ptr event) {
    log(log_level::level::ERROR, event);
}

void logger::fatal(log_event::ptr event) {
    log(log_level::level::FATAL, event);
}

void logger::add_appender(log_appender::ptr appender) {
    appenders_.push_back(appender);
}

void logger::delete_appender(log_appender::ptr appender) {
    for (auto it = appenders_.begin(); it != appenders_.end(); ++it) {
        if (*it == appender) {
            appenders_.erase(it);
            break;
        }
    }
}

void logger::set_level(log_level::level level) {
    level_ = level;
}

log_level::level logger::get_level() const {
    return level_;
}

const ::std::string& logger::get_name() const {
    return name_;
}

_SW_END
