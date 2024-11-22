// implementation file for sw_log.h

#include "../include/sw_log.h"

_SW_BEGIN

log_event::log_event(const char *file, const char *func, ::std::uint32_t line, const std::string &content) :
    file_(file), func_(func), line_(line), thread_id_(::std::this_thread::get_id()), content_(content) {
    ::std::time_t current_time = ::std::time(nullptr);
    time_ = ::std::localtime(&current_time);
}

::std::string log_level::to_string(log_level::level level) {
    return _Level_to_string[static_cast<int>(level)];
}

const ::std::unordered_map<::std::string, log_level::level> log_level::_String_to_level = {
    {"DEBUG", level::DEBUG},
    {"INFO",  level::INFO},
    {"WARN",  level::WARN},
    {"ERROR", level::ERROR},
    {"FATAL", level::FATAL}
};

log_level::level log_level::from_string(const ::std::string &str) {
    return log_level::_String_to_level.at(str);
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
    pattern_(pattern) {
    init();
}

void log_formatter::init() {
    static ::std::unordered_map<::std::string, ::std::function<_Formatter_item::ptr(const ::std::string&)> > s_formatter_items = {
#define ADD_ITEM(str, T) \
    {#str, [](const ::std::string& fmt) { return _Formatter_item::ptr(new T(fmt)); }}
        ADD_ITEM(m, _Message_fotmatter_item),
        ADD_ITEM(p, _Level_fotmatter_item),
        ADD_ITEM(r, _Elapsed_fotmatter_item),
        ADD_ITEM(c, _Loggername_fotmatter_item),
        ADD_ITEM(t, _Threadid_fotmatter_item),
        ADD_ITEM(g, _Coroutineid_fotmatter_item),
        ADD_ITEM(n, _Newline_fotmatter_item),
        ADD_ITEM(d, _Datetime_fotmatter_item),
        ADD_ITEM(f, _Filename_fotmatter_item),
        ADD_ITEM(l, _Line_fotmatter_item),
        ADD_ITEM(s, _Cstr_fotmatter_item)
#undef ADD_ITEM
    };
    ::std::string constant_str, format_inside_brace;
    ::std::string current_format_specifier;
    const ::std::size_t len = pattern_.size();
    ::std::size_t pos = 0;
    enum {
        C_STR,
        PER_SIGN,
        BRACE
    } fmt_status = C_STR;
    while (pos < len) {
        switch (pattern_[pos]) {
        case '%':
            switch (fmt_status) {
            case C_STR:
                if (!constant_str.empty()) {
                    items_.push_back(s_formatter_items["s"](constant_str));
                    constant_str = "";
                }
                fmt_status = PER_SIGN;
                break;
            case PER_SIGN:
                break;
            case BRACE:
                format_inside_brace.push_back(pattern_[pos]);
                break;
            }
            break;
        case '{':
            fmt_status = BRACE;
            break;
        case '}':
            fmt_status = C_STR;
            if (s_formatter_items.find(current_format_specifier) != s_formatter_items.end()) {
                items_.push_back(s_formatter_items[current_format_specifier](format_inside_brace));
            }
            current_format_specifier = format_inside_brace = "";
            break;
        default:
            switch (fmt_status) {
            case C_STR:
                if (!current_format_specifier.empty()) {
                    if (s_formatter_items.find(current_format_specifier) != s_formatter_items.end()) {
                        items_.push_back(s_formatter_items[current_format_specifier](""));
                    }
                    current_format_specifier = "";
                }
                constant_str += pattern_[pos];
                break;
            case PER_SIGN:
                current_format_specifier = pattern_[pos];
                if (pos == len - 1 && s_formatter_items.find(current_format_specifier) != s_formatter_items.end()) {
                    items_.push_back(s_formatter_items[current_format_specifier](""));
                }
                fmt_status = C_STR;
                break;
            case BRACE:
                format_inside_brace.push_back(pattern_[pos]);
                break;
            }
            break;
        }
        ++pos;
    }
    if (!constant_str.empty()) {
        items_.push_back(s_formatter_items["s"](constant_str));
    }
    return;
}

::std::string log_formatter::format(logger::ptr logger, log_level::level level, log_event::ptr event) {
    ::std::stringstream ss;
    for (auto &p : items_) {
        p->format(ss, logger, level, event);
    }
    return ss.str();
}

log_formatter::_Formatter_item::_Formatter_item(const ::std::string& format) : format_(format) {}

log_formatter::_Message_fotmatter_item::_Message_fotmatter_item(const std::string &format) : _Formatter_item(format) {
}

void log_formatter::_Message_fotmatter_item::format(::std::ostream& os, logger::ptr logger, log_level::level level, log_event::ptr event) {
    os << event->get_content();
}

log_formatter::_Level_fotmatter_item::_Level_fotmatter_item(const std::string &format) : _Formatter_item(format) {
}

void log_formatter::_Level_fotmatter_item::format(::std::ostream& os, logger::ptr logger, log_level::level level, log_event::ptr event) {
    os << log_level::to_string(level);
}

log_formatter::_Elapsed_fotmatter_item::_Elapsed_fotmatter_item(const std::string &format) : _Formatter_item(format) {
}

void log_formatter::_Elapsed_fotmatter_item::format(::std::ostream& os, logger::ptr logger, log_level::level level, log_event::ptr event) {
    os << event->get_elapsed();
}

log_formatter::_Loggername_fotmatter_item::_Loggername_fotmatter_item(const std::string &format) : _Formatter_item(format) {
}

void log_formatter::_Loggername_fotmatter_item::format(::std::ostream& os, logger::ptr logger, log_level::level level, log_event::ptr event) {
    os << logger->get_name();
}

log_formatter::_Threadid_fotmatter_item::_Threadid_fotmatter_item(const std::string &format) : _Formatter_item(format) {
}

void log_formatter::_Threadid_fotmatter_item::format(::std::ostream& os, logger::ptr logger, log_level::level level, log_event::ptr event) {
    os << event->get_thread_id();
}

log_formatter::_Coroutineid_fotmatter_item::_Coroutineid_fotmatter_item(const std::string &format) : _Formatter_item(format) {
}

void log_formatter::_Coroutineid_fotmatter_item::format(::std::ostream& os, logger::ptr logger, log_level::level level, log_event::ptr event) {
    os << event->get_coroutine_id();
}

log_formatter::_Datetime_fotmatter_item::_Datetime_fotmatter_item(const std::string &format) : _Formatter_item(format) {
}

void log_formatter::_Datetime_fotmatter_item::format(::std::ostream& os, logger::ptr logger, log_level::level level, log_event::ptr event) {
    os << ::std::put_time(event->get_time(), format_.c_str());
}

log_formatter::_Filename_fotmatter_item::_Filename_fotmatter_item(const std::string &format) : _Formatter_item(format) {
}

void log_formatter::_Filename_fotmatter_item::format(::std::ostream& os, logger::ptr logger, log_level::level level, log_event::ptr event) {
    os << event->get_file();
}

log_formatter::_Line_fotmatter_item::_Line_fotmatter_item(const std::string &format) : _Formatter_item(format) {
}

void log_formatter::_Line_fotmatter_item::format(::std::ostream& os, logger::ptr logger, log_level::level level, log_event::ptr event) {
    os << event->get_line();
}

log_formatter::_Newline_fotmatter_item::_Newline_fotmatter_item(const std::string &format) : _Formatter_item(format) {
}

void log_formatter::_Newline_fotmatter_item::format(::std::ostream& os, logger::ptr logger, log_level::level level, log_event::ptr event) {
    os << ::std::endl;
}

log_formatter::_Cstr_fotmatter_item::_Cstr_fotmatter_item(const ::std::string& str) : _Formatter_item(str) {
}

void log_formatter::_Cstr_fotmatter_item::format(::std::ostream& os, logger::ptr logger, log_level::level level, log_event::ptr event) {
    os << format_;
}

log_formatter::ptr log_appender::get_formatter() const {
    return pformatter_;
}

void log_appender::set_formatter(log_formatter::ptr pformatter) {
    pformatter_ = pformatter;
}

stream_log_appender::stream_log_appender(::std::ostream &out) : out_(out) {
}

void stream_log_appender::log(::std::shared_ptr<logger> logger, log_level::level level, log_event::ptr event) {
    if (level >= level_) {
        out_ << pformatter_->format(logger, level, event);
    }
}

logger::logger() {
    _Read_config_file(DEFAULT_CONFIG_FILE);
}

logger::logger(const char * fp) {
    _Read_config_file(fp);
}

logger::logger(const std::string &fp) {
    _Read_config_file(fp);
}
#ifdef __cpp_lib_filesystem
logger::logger(const ::std::filesystem::path &fp) {
    _Read_config_file(fp);
}
#endif // __cpp_lib_filesystem

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

void logger::add_appender(log_appender::ptr pappender) {
    appenders_.push_back(pappender);
}

void logger::delete_appender(log_appender::ptr pappender) {
    for (auto it = appenders_.begin(); it != appenders_.end(); ++it) {
        if (*it == pappender) {
            appenders_.erase(it);
            break;
        }
    }
}

void logger::set_name(const ::std::string &name) {
    name_ = name;
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
