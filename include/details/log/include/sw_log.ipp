// implementation file for sw::logger (template members) (internal)

_SW_BEGIN

template <typename _Pt>
void logger::_Read_config_file(const _Pt &fp) {
    static const ::std::string white_characters = " \t\n\r\f\v";
    auto trim = [&](const ::std::string &str) -> ::std::string {
        size_t first = str.find_first_not_of(white_characters);
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(white_characters);
        return str.substr(first, (last - first + 1));
    };

    static auto set_name = [this](const ::std::string &name) {
        this->set_name(name);
    };

    static auto set_level = [this](const ::std::string &level) {
        this->set_level(log_level::from_string(level));
    };

    ::std::unordered_map<::std::string, log_appender::ptr> appenders;
    static ::std::unordered_map<::std::string, log_appender::ptr> create_appender = {
        { "sw_log.stream_appender.stdout", ::std::make_shared<sw::stream_log_appender>(std::cout) },
        { "sw_log.stream_appender.stderr", ::std::make_shared<sw::stream_log_appender>(std::cerr) }
    };
    static auto add_appender = [&](const ::std::string &appender_names) {
        ::std::istringstream iss(appender_names);
        ::std::string appender_name;
        while (std::getline(iss, appender_name, ',')) {
            appender_name = trim(appender_name);
            appenders[appender_name] = create_appender["sw_log.stream_appender.stdout"]; // by default
        }
    };

    ::std::string current_appender;
    static auto set_appender = [&](const ::std::string &appender_type) {
        appenders[current_appender] = create_appender[appender_type];
    };

    static auto set_appender_formatter = [&](const ::std::string &format) {
        log_formatter::ptr pformatter = ::std::make_shared<log_formatter>(format);
        appenders[current_appender]->set_formatter(pformatter);
    };

    static ::std::unordered_map<::std::string, ::std::function<void(const ::std::string &)> > parser_fn = {
        { "sw_log.logger.name", set_name },
        { "sw_log.logger.level", set_level },
        { "sw_log.logger.appender.add", add_appender },
        { "sw_log.logger.appender.set", set_appender },
        { "sw_log.logger.appender.set.formatter.format", set_appender_formatter },
    };

    auto ifs = ::std::ifstream(fp);
    if (!ifs.is_open()) {
        REGISTER_SW_ERROR(1002, "file open failed");
        ifs.open(DEFAULT_CONFIG_FILE);
        if (!ifs.is_open()) {
            throw ::std::runtime_error("file open failed");
        }
    }
    while (!(ifs.eof() || ifs.fail())) {
        ::std::string line, key, value;
        ::std::getline(ifs, line);
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        ::std::istringstream iss(line);
        if (::std::getline(iss, key, '=') && ::std::getline(iss, value)) {
            key = trim(key);
            value = trim(value);
            if (parser_fn.find(key) == parser_fn.end()) {
                if (key.find("sw_log.logger.appender") == 0) {
                    ::std::istringstream iss(key);
                    ::std::string name;
                    ::std::size_t dot = 0;
                    bool parse_failed = false;
                    while(::std::getline(iss, name, '.')) {
                        if (dot == 3) {
                            current_appender = name;
                        }
                        if ((dot == 3 && appenders.find(current_appender) == appenders.end()) || (dot == 4 && name != "formatter") || (dot == 5 && name != "format") || dot > 5) {
                            parse_failed = true;
                            REGISTER_SW_ERROR(1003, "file parse failed");
                            continue;
                        }
                        ++dot;
                    }
                    if (dot == 4) {
                        key = "sw_log.logger.appender.set";
                    } else {
                        key = "sw_log.logger.appender.set.formatter.format";
                    }
                } else {
                    REGISTER_SW_ERROR(1003, "file parse failed");
                    continue;
                }
            }
            parser_fn[key](value);
        }
    }
    for (auto &appender : appenders) {
        this->add_appender(appender.second);
    }
    return;
}

_SW_END // _SW_BEGIN
