// implementation file for sw_log.h

#include "sw/sw_log.hpp"

_SW_BEGIN

const ::std::unordered_map<::std::string, log_level::level> log_level::_String_to_level = {
    {"DEBUG", level::DEBUG},
    {"INFO",  level::INFO},
    {"WARN",  level::WARN},
    {"ERROR", level::ERROR},
    {"FATAL", level::FATAL}
};

::std::string log_level::to_string(log_level::level level) _SW_NOEXCEPT {
    return _Level_to_string[static_cast<int>(level)];
}

log_level::level log_level::from_string(const ::std::string &str) _SW_NOEXCEPT {
    return log_level::_String_to_level.find(str) == log_level::_String_to_level.end() ? log_level::level::NONE : log_level::_String_to_level.at(str);
}

log_event::log_event(const char *file, const char *func, ::std::uint32_t line, const std::string &content) _SW_NOEXCEPT :
    file_(file), func_(func), line_(line), thread_id_(::std::this_thread::get_id()), content_(content) {
    ::std::time_t current_time = ::std::time(nullptr);
    time_ = ::std::localtime(&current_time);
}

const char *log_event::get_file() const _SW_NOEXCEPT {
    return file_;
}

::std::uint64_t log_event::get_elapsed() const _SW_NOEXCEPT {
    return sw_.elapsed();
}

::std::uint32_t log_event::get_line() const _SW_NOEXCEPT {
    return line_;
}

::std::thread::id log_event::get_thread_id() const _SW_NOEXCEPT {
    return thread_id_;
}

::std::string log_event::get_coroutine_id() const _SW_NOEXCEPT {
    return coroutine_id_;
}

::std::tm *log_event::get_time() const _SW_NOEXCEPT {
    return time_;
}

::std::string log_event::get_content() const _SW_NOEXCEPT {
    return content_;
}

pattern_log_formatter::pattern_log_formatter(const ::std::string &pattern) _SW_NOEXCEPT :
    pattern_(pattern) {
    _Parse_pattern();
}

::std::string pattern_log_formatter::format(logger::ptr logger, log_level::level level, log_event::ptr event) _SW_NOEXCEPT {
    ::std::stringstream ss;
    for (auto &p : items_) {
        p->format(ss, logger, level, event);
    }
    return ss.str();
}

pattern_log_formatter::_Formatter_item::_Formatter_item(const ::std::string& format) _SW_NOEXCEPT : format_(format) {}

pattern_log_formatter::_Message_fotmatter_item::_Message_fotmatter_item(const std::string &format) _SW_NOEXCEPT : _Formatter_item(format) {
}

void pattern_log_formatter::_Message_fotmatter_item::format(::std::ostream& os, logger::ptr logger, log_level::level level, log_event::ptr event) _SW_NOEXCEPT {
    os << event->get_content();
}

pattern_log_formatter::_Level_fotmatter_item::_Level_fotmatter_item(const std::string &format) _SW_NOEXCEPT : _Formatter_item(format) {
}

void pattern_log_formatter::_Level_fotmatter_item::format(::std::ostream& os, logger::ptr logger, log_level::level level, log_event::ptr event) _SW_NOEXCEPT {
    os << log_level::to_string(level);
}

pattern_log_formatter::_Elapsed_fotmatter_item::_Elapsed_fotmatter_item(const std::string &format) _SW_NOEXCEPT : _Formatter_item(format) {
}

void pattern_log_formatter::_Elapsed_fotmatter_item::format(::std::ostream& os, logger::ptr logger, log_level::level level, log_event::ptr event) _SW_NOEXCEPT {
    os << event->get_elapsed();
}

pattern_log_formatter::_Loggername_fotmatter_item::_Loggername_fotmatter_item(const std::string &format) _SW_NOEXCEPT : _Formatter_item(format) {
}

void pattern_log_formatter::_Loggername_fotmatter_item::format(::std::ostream& os, logger::ptr logger, log_level::level level, log_event::ptr event) _SW_NOEXCEPT {
    os << logger->get_name();
}

pattern_log_formatter::_Threadid_fotmatter_item::_Threadid_fotmatter_item(const std::string &format) _SW_NOEXCEPT : _Formatter_item(format) {
}

void pattern_log_formatter::_Threadid_fotmatter_item::format(::std::ostream& os, logger::ptr logger, log_level::level level, log_event::ptr event) _SW_NOEXCEPT {
    os << event->get_thread_id();
}

pattern_log_formatter::_Coroutineid_fotmatter_item::_Coroutineid_fotmatter_item(const std::string &format) _SW_NOEXCEPT : _Formatter_item(format) {
}

void pattern_log_formatter::_Coroutineid_fotmatter_item::format(::std::ostream& os, logger::ptr logger, log_level::level level, log_event::ptr event) _SW_NOEXCEPT {
    os << event->get_coroutine_id();
}

pattern_log_formatter::_Datetime_fotmatter_item::_Datetime_fotmatter_item(const std::string &format) _SW_NOEXCEPT : _Formatter_item(format) {
}

void pattern_log_formatter::_Datetime_fotmatter_item::format(::std::ostream& os, logger::ptr logger, log_level::level level, log_event::ptr event) _SW_NOEXCEPT {
    os << ::std::put_time(event->get_time(), format_.c_str());
}

pattern_log_formatter::_Filename_fotmatter_item::_Filename_fotmatter_item(const std::string &format) _SW_NOEXCEPT : _Formatter_item(format) {
}

void pattern_log_formatter::_Filename_fotmatter_item::format(::std::ostream& os, logger::ptr logger, log_level::level level, log_event::ptr event) _SW_NOEXCEPT {
    os << event->get_file();
}

pattern_log_formatter::_Line_fotmatter_item::_Line_fotmatter_item(const std::string &format) _SW_NOEXCEPT : _Formatter_item(format) {
}

void pattern_log_formatter::_Line_fotmatter_item::format(::std::ostream& os, logger::ptr logger, log_level::level level, log_event::ptr event) _SW_NOEXCEPT {
    os << event->get_line();
}

pattern_log_formatter::_Newline_fotmatter_item::_Newline_fotmatter_item(const std::string &format) _SW_NOEXCEPT : _Formatter_item(format) {
}

void pattern_log_formatter::_Newline_fotmatter_item::format(::std::ostream& os, logger::ptr logger, log_level::level level, log_event::ptr event) _SW_NOEXCEPT {
    os << ::std::endl;
}

pattern_log_formatter::_Cstr_fotmatter_item::_Cstr_fotmatter_item(const ::std::string& str) _SW_NOEXCEPT : _Formatter_item(str) {
}

void pattern_log_formatter::_Cstr_fotmatter_item::format(::std::ostream& os, logger::ptr logger, log_level::level level, log_event::ptr event) _SW_NOEXCEPT {
    os << format_;
}

void pattern_log_formatter::_Parse_pattern() _SW_NOEXCEPT {
    static ::std::unordered_map<::std::string, ::std::function<_Formatter_item::ptr(const ::std::string&)> > formatter_items = {
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
    enum state : ::std::uint8_t {
        C_STR,
        PER_SIGN,
        BRACE
    } fmt_status = C_STR;
    while (pos < len) {
        switch (pattern_[pos]) {
        case '%':
            switch (fmt_status) {
            case state::C_STR:
                if (!constant_str.empty()) {
                    items_.push_back(formatter_items["s"](constant_str));
                    constant_str = "";
                }
                fmt_status = state::PER_SIGN;
                break;
            case state::PER_SIGN:
                break;
            case state::BRACE:
                format_inside_brace.push_back(pattern_[pos]);
                break;
            }
            break;
        case '{':
            fmt_status = state::BRACE;
            break;
        case '}':
            if (formatter_items.find(current_format_specifier) != formatter_items.end()) {
                items_.push_back(formatter_items[current_format_specifier](format_inside_brace));
            }
            current_format_specifier = format_inside_brace = "";
            fmt_status = state::C_STR;
            break;
        default:
            switch (fmt_status) {
            case state::C_STR:
                if (!current_format_specifier.empty()) {
                    if (formatter_items.find(current_format_specifier) != formatter_items.end()) {
                        items_.push_back(formatter_items[current_format_specifier](""));
                    }
                    current_format_specifier = "";
                }
                constant_str += pattern_[pos];
                break;
            case state::PER_SIGN:
                current_format_specifier = pattern_[pos];
                if (pos == len - 1 && formatter_items.find(current_format_specifier) != formatter_items.end()) {
                    items_.push_back(formatter_items[current_format_specifier](""));
                }
                fmt_status = state::C_STR;
                break;
            case state::BRACE:
                format_inside_brace.push_back(pattern_[pos]);
                break;
            }
            break;
        }
        ++pos;
    }
    if (!constant_str.empty()) {
        items_.push_back(formatter_items["s"](constant_str));
    }
    return;
}

pattern_log_formatter::ptr log_appender::get_formatter() const _SW_NOEXCEPT {
    return pformatter_;
}

void log_appender::set_formatter(pattern_log_formatter::ptr pformatter) _SW_NOEXCEPT {
    pformatter_ = pformatter;
}

void log_appender::set_level(log_level::level level) _SW_NOEXCEPT {
    level_ = level;
}

log_level::level log_appender::get_level() const _SW_NOEXCEPT {
    return level_;
}

stream_log_appender::stream_log_appender(::std::ostream &out) _SW_NOEXCEPT : out_(out) {
}

void stream_log_appender::log(::std::shared_ptr<logger> logger, log_level::level level, log_event::ptr event) _SW_NOEXCEPT {
    if (level >= level_) {
        out_ << pformatter_->format(logger, level, event);
    }
}

fstream_log_appender::fstream_log_appender(const ::std::string &fp) _SW_DES(::std::runtime_error) : out_(fp) {
    if (!out_.is_open()) {
        _SW_THROW(::std::runtime_error("open file failed"));
    }
}

fstream_log_appender::~fstream_log_appender() _SW_NOEXCEPT {
    out_.close();
}

void fstream_log_appender::log(::std::shared_ptr<logger> logger, log_level::level level, log_event::ptr event) _SW_NOEXCEPT {
    if (level >= level_) {
        out_ << pformatter_->format(logger, level, event);
    }
}

rolling_fstream_log_appender::rolling_fstream_log_appender(const ::std::string &fp) _SW_DES(::std::runtime_error) : out_(fp) {
    if (!out_.is_open()) {
        _SW_THROW(::std::runtime_error("open file failed"));
    }
}

rolling_fstream_log_appender::~rolling_fstream_log_appender() _SW_NOEXCEPT {
    out_.close();
}

void rolling_fstream_log_appender::log(::std::shared_ptr<logger> logger, log_level::level level, log_event::ptr event) _SW_NOEXCEPT {
    if (level >= level_) {
        out_ << pformatter_->format(logger, level, event);
    }
}

logger::ptr logger::get_root_logger() _SW_NOEXCEPT {
    return get_logger("root");
}

logger::ptr logger::get_logger(const std::string &logger_name) _SW_NOEXCEPT {
    static bool configured = false;
    if (!configured) {
        logger::_Init_root_logger();
        logger::configure();
        configured = true;
    }
    if (logger::sploggers_.find(logger_name) == logger::sploggers_.end()) {
        logger *plogger = new logger(*get_root_logger().get());
        plogger->set_name(logger_name);
        logger::sploggers_[logger_name] = logger::ptr(plogger);
    }
    return logger::sploggers_[logger_name];
}

void logger::configure() _SW_NOEXCEPT {
    auto parser = _Properties_parser(DEFAULT_CONFIG_FILE);
}

void logger::configure(const char *fp) {
    auto parser = _Properties_parser(fp);
}

void logger::configure(const std::string &fp) _SW_DES(::std::runtime_error) {
    auto parser = _Properties_parser(fp);
}

#ifdef __cpp_lib_filesystem
void logger::configure(const ::std::filesystem::path &fp) _SW_DES(::std::runtime_error) {
    auto parser = _Properties_parser(fp);
}
#endif // __cpp_lib_filesystem

void logger::log(log_level::level level, log_event::ptr event) _SW_NOEXCEPT {
    if (level >= level_) {
        for (auto &pappender : pappenders_) {
            pappender.second->log(shared_from_this(), level, event);
        }
    }
}

void logger::debug(log_event::ptr event) _SW_NOEXCEPT {
    log(log_level::level::DEBUG, event);
}

void logger::info(log_event::ptr event) _SW_NOEXCEPT {
    log(log_level::level::INFO, event);
}

void logger::warn(log_event::ptr event) _SW_NOEXCEPT {
    log(log_level::level::WARN, event);
}

void logger::error(log_event::ptr event) _SW_NOEXCEPT {
    log(log_level::level::ERROR, event);
}

void logger::fatal(log_event::ptr event) _SW_NOEXCEPT {
    log(log_level::level::FATAL, event);
}

void logger::add_appender(const ::std::string &appender_name, log_appender::ptr pappender) _SW_NOEXCEPT {
    pappenders_[appender_name] = pappender;
}

void logger::delete_appender(const ::std::string &appender_name) _SW_NOEXCEPT {
    for (auto it = pappenders_.begin(); it != pappenders_.end(); ++it) {
        if (it->first == appender_name) {
            pappenders_.erase(it);
            break;
        }
    }
}

void logger::set_name(const ::std::string &name) _SW_NOEXCEPT {
    name_ = name;
}

void logger::set_level(log_level::level level) _SW_NOEXCEPT {
    level_ = level;
}

log_level::level logger::get_level() const _SW_NOEXCEPT {
    return level_;
}

const ::std::string& logger::get_name() const _SW_NOEXCEPT {
    return name_;
}

void logger::_Init_root_logger() _SW_NOEXCEPT {
    auto root_logger = ::std::shared_ptr<logger>(new logger("root"));
    auto root_logger_log_appender = ::std::make_shared<stream_log_appender>(std::cout);
    auto root_logger_log_formatter = ::std::make_shared<pattern_log_formatter>("[%d{%Y-%m-%d %H:%M:%S}] [%p] [%t] %m %n");
    root_logger_log_appender->set_formatter(root_logger_log_formatter);
    root_logger->add_appender("stdout", root_logger_log_appender);
    sploggers_["root"] = root_logger;
}


logger::logger(const std::string &name) _SW_NOEXCEPT : name_(name), level_(log_level::level::INFO) {
}

logger::logger(const logger &other) _SW_NOEXCEPT : name_(other.name_), level_(other.level_), pappenders_(other.pappenders_) {
}

bool logger::_Is_complete_logger() const _SW_NOEXCEPT {
    if (name_.empty() || pappenders_.empty()) {
        return false;
    }
    for (auto &pappender : pappenders_) {
        if (!pappender.second) {
            return false;
        }
        if (!pappender.second->get_formatter()) {
            return false;
        }
    }
    return true;
}

logger::_Properties_parser::_Properties_parser(const char *fp) _SW_DES(::std::runtime_error) {
    _Load_properties(fp);
    _Parse();
}

logger::_Properties_parser::_Properties_parser(const std::string &fp) _SW_DES(::std::runtime_error) {
    _Load_properties(fp);
    _Parse();
}

#ifdef __cpp_lib_filesystem
logger::_Properties_parser::_Properties_parser(const ::std::filesystem::path &fp) _SW_DES(::std::runtime_error) {
    _Load_properties(fp);
    _Parse();
}
#endif // __cpp_lib_filesystem

void logger::_Properties_parser::_Parse() _SW_NOEXCEPT {
    ::std::unordered_map<::std::string, appender_meta> appender_meta_cache;
    for (auto it = properties_.begin(); it != properties_.end(); ++it) {
        auto key_parse_state = state::INIT;
        auto &[key, value] = *it;
        auto key_elements = split(key, '.');
        auto key_elements_size = key_elements.size();
        ::std::string current_logger_name, current_log_appender_name;
        for (auto & key_element : key_elements) {
            key_element = trim(key_element);
            switch (key_parse_state) {
            case state::INIT:
                key_parse_state = _Parse_init(key_element, current_logger_name);
                break;
            case state::SW_LOG:
                key_parse_state = _Parse_sw_log(key_element, key_elements_size, current_logger_name, appender_meta_cache, value);
                break;
            case state::LOGGER:
                key_parse_state = _Parse_logger(key_element, key_elements_size, current_logger_name, current_log_appender_name, appender_meta_cache, value);
                break;
            case state::APPENDER:
                key_parse_state = _Parse_appender(key_element, key_elements_size, current_logger_name, current_log_appender_name, appender_meta_cache, value);
                break;
            case state::FORMATTER:
                key_parse_state = _Parse_formatter(key_element, key_elements_size, current_logger_name, current_log_appender_name, appender_meta_cache, value);
                break;
            }
            if (key_parse_state == state::FAILED) {
                break;
            }
            if (key_parse_state == state::FINISHED) {
                break;
            }
        }
    }
    for (auto it = logger::sploggers_.begin(); it != logger::sploggers_.end();) {
        if (!it->second->_Is_complete_logger()) {
            it = logger::sploggers_.erase(it);
        } else {
            ++it;
        }
    }
}

logger::_Properties_parser::state logger::_Properties_parser::_Parse_init(const ::std::string &element, ::std::string &current_logger_name) _SW_NOEXCEPT {
    if (element != "sw_log") {
        return state::FAILED;
    }
    current_logger_name = element;
    return state::SW_LOG;
}

logger::_Properties_parser::state logger::_Properties_parser::_Parse_sw_log(const ::std::string &element, 
    ::std::size_t size, ::std::string &current_logger_name, ::std::unordered_map<::std::string, appender_meta> &appender_meta_cache, 
        const ::std::string &value) _SW_NOEXCEPT {
    current_logger_name = element;
    ::std::string meta_key;
    switch (size) {
    case 0:
    case 1:
        return state::FAILED;
    case 2:
        logger::sploggers_[current_logger_name] = logger::ptr(new logger(current_logger_name));
        for (auto &current_appender_name : split(value, ',')) {
            current_appender_name = trim(current_appender_name);
            if (current_appender_name == "NONE") {
                return state::FAILED;
            } else if (log_level::from_string(current_appender_name) != log_level::level::NONE) {
                logger::sploggers_[current_logger_name]->set_level(log_level::from_string(current_appender_name));
            } else {
                logger::sploggers_[current_logger_name]->add_appender(current_appender_name, nullptr);
                meta_key = current_logger_name + " " + current_appender_name;
                appender_meta_cache[meta_key] = appender_meta();
            }
        }
        return state::FINISHED;
    default:
        if (logger::sploggers_.find(current_logger_name) == logger::sploggers_.end()) return state::FAILED;
        return state::LOGGER;
    }
    return state::FAILED;
}

logger::_Properties_parser::state logger::_Properties_parser::_Parse_logger(const ::std::string &element, 
    ::std::size_t size, const ::std::string &current_logger_name, ::std::string &current_log_appender_name, 
        ::std::unordered_map<::std::string, appender_meta> &appender_meta_cache, const ::std::string &value) _SW_NOEXCEPT {
    static const ::std::unordered_map<::std::string, appender_type> appender_types = {
        { "sw_log.stream_appender", appender_type::STREAM_APPENDER },
        { "sw_log.file_appender", appender_type::FSTREAM_APPENDER },
        { "sw_log.rolling_file_appender", appender_type::ROLLING_FSTREAM_APPENDER }
    };
    current_log_appender_name = element;
    auto meta_key = current_logger_name + " " + current_log_appender_name;
    switch (size) {
    case 0:
    case 1:
    case 2:
        return state::FAILED;
    case 3:
        if (appender_meta_cache.find(meta_key) == appender_meta_cache.end()) return state::FAILED;
        if (appender_types.find(value) == appender_types.end()) return state::FAILED;
        appender_meta_cache[meta_key].at = appender_types.at(value);
        return state::FINISHED;
    default:
        return state::APPENDER;
    }
    return state::FAILED;
}

logger::_Properties_parser::state logger::_Properties_parser::_Parse_appender(const ::std::string &element, 
    ::std::size_t size, const ::std::string &current_logger_name, ::std::string &current_log_appender_name, 
        ::std::unordered_map<::std::string, appender_meta> &appender_meta_cache, const ::std::string &value) _SW_NOEXCEPT {
    static const ::std::unordered_map<::std::string, ::std::ostream &> stream_types = {
        { "stdout", ::std::cout },
        { "stderr", ::std::cerr },
        { "stdlog", ::std::clog }
    };
    static const ::std::unordered_map<::std::string, formatter_type> formatter_types = {
        { "sw_log.pattern_layout", formatter_type::PATTERN_FORMATTER }
    };
    auto meta_key = current_logger_name + " " + current_log_appender_name;
    switch (size) {
    case 0:
    case 1:
    case 2:
    case 3:
        return state::FAILED;
    case 4:
        if (element == "target") {
            if (appender_meta_cache.find(meta_key) == appender_meta_cache.end() 
                || appender_meta_cache[meta_key].at != appender_type::STREAM_APPENDER) return state::FAILED;
            if (stream_types.find(value) == stream_types.end()) return state::FAILED;
            logger::sploggers_[current_logger_name]->add_appender(current_log_appender_name, log_appender::ptr(new stream_log_appender(stream_types.at(value))));
            return state::FINISHED;
        } else if (element == "file") {
            if (appender_meta_cache.find(meta_key) == appender_meta_cache.end() 
                || (appender_meta_cache[meta_key].at != appender_type::FSTREAM_APPENDER 
                    && appender_meta_cache[meta_key].at != appender_type::ROLLING_FSTREAM_APPENDER)) return state::FAILED;
            if (appender_meta_cache[meta_key].at == appender_type::FSTREAM_APPENDER) {
                logger::sploggers_[current_logger_name]->add_appender(current_log_appender_name, log_appender::ptr(new fstream_log_appender(value)));
            } else {
                logger::sploggers_[current_logger_name]->add_appender(current_log_appender_name, log_appender::ptr(new rolling_fstream_log_appender(value)));
            }
            return state::FINISHED;
        } else if (element == "threshold") {
            if (appender_meta_cache.find(meta_key) == appender_meta_cache.end()) return state::FAILED;
            if (log_level::from_string(value) == log_level::level::NONE) return state::FAILED;
            appender_meta_cache[meta_key].ll = log_level::from_string(value);
            return state::FINISHED;
        } else if (element == "layout") {
            if (appender_meta_cache.find(meta_key) == appender_meta_cache.end()) return state::FAILED;
            if (formatter_types.find(value) == formatter_types.end()) return state::FAILED;
            appender_meta_cache[meta_key].ft = formatter_types.at(value);
            return state::FINISHED;
        }
        break;
    default:
        return state::FORMATTER;
    }
    return state::FAILED;
}

logger::_Properties_parser::state logger::_Properties_parser::_Parse_formatter(const ::std::string &element, 
    ::std::size_t size, const ::std::string &current_logger_name, ::std::string &current_log_appender_name, 
        ::std::unordered_map<::std::string, appender_meta> &appender_meta_cache, const ::std::string &value) _SW_NOEXCEPT {
    auto meta_key = current_logger_name + " " + current_log_appender_name;
    switch (size) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
        return state::FAILED;
    case 5:
        if (element != "conversion_pattern") return state::FAILED;
        if (appender_meta_cache.find(meta_key) == appender_meta_cache.end()) return state::FAILED;
        if (logger::sploggers_[current_logger_name]->pappenders_[current_log_appender_name] == nullptr) return state::FAILED;
        if (appender_meta_cache[meta_key].ft != formatter_type::PATTERN_FORMATTER) return state::FAILED;
        logger::sploggers_[current_logger_name]->pappenders_[current_log_appender_name]->set_formatter(log_formatter::ptr(new pattern_log_formatter(value)));
        return state::FINISHED;
    default:
        return state::FAILED;
    }
    return state::FAILED;
}

::std::unordered_map<::std::string, logger::ptr> logger::sploggers_;

_SW_END
