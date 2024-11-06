// implementation file for sw_log.h

#include "../../include/sw_vals.h"
#include "../include/sw_log.h"

_SW_BEGIN

log_level::log_level(log_level::_Level l) : level(l) {}

bool log_level::operator >= (const log_level& l) const {
    return level >= l.level;
}

::std::string log_level::to_string() const {
    return _Level_to_string[static_cast<int>(level)];
}

const char* log_event::get_file() const {
    return file_;
}

::std::uint64_t log_event::get_elapsed() const {
    return pclock_->elapsed();
}

::std::uint32_t log_event::get_line() const {
    return line_;
}

::std::uint32_t log_event::get_thread_id() const {
    return thread_id_;
}

::std::uint32_t log_event::get_coroutine_id() const {
    return coroutine_id_;
}

::std::uint64_t log_event::get_time() const {
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
            items.emplace_back(::std::make_tuple());
            break;
        }
    }
}

::std::string log_formatter::format(log_event::ptr event) {
    ::std::stringstream ss;
    for (auto &p : items_) {
        p->format(ss, event);
    }
    return ss.str();
}

log_formatter::ptr log_appender::get_formatter() const {
    return pformatter_;
}

void log_appender::set_formatter(log_formatter::ptr formatter) {
    pformatter_ = formatter;
}

void logger::log(log_level level, log_event::ptr event) {
    if (level >= level_) {
        for (auto &appender : appenders_) {
            appender->log(level, event);
        }
    }
}

void logger::debug(log_event::ptr event) {
    log(log_level::_Level::DEBUG, event);
}

void logger::info(log_event::ptr event) {
    log(log_level::_Level::INFO, event);
}

void logger::warn(log_event::ptr event) {
    log(log_level::_Level::WARN, event);
}

void logger::error(log_event::ptr event) {
    log(log_level::_Level::ERROR, event);
}

void logger::fatal(log_event::ptr event) {
    log(log_level::_Level::FATAL, event);
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

void logger::set_level(log_level level) {
    level_ = level;
}

log_level logger::get_level() const {
    return level_;
}

_SW_END
