// implementation file for sw::properties (internal)

#include "sw/sw_properties.hpp"

_SW_BEGIN

void properties::load(::std::istream& is) {
    ::std::string line, key, value;
    ::std::smatch match;
    ::std::regex find_kv_pattern(R"((\s*([^\s=]+)\s*)=(\s*(.*))\s*)");
    ::std::regex find_varible_pattern(R"(\$\{([^}]+)\})");
    auto replace_varible = [this, find_varible_pattern](::std::string &str) {
        ::std::smatch match;
        while (::std::regex_search(str, match, find_varible_pattern)) {
            if (contains_key(match[1])) {
                str = ::std::regex_replace(str, find_varible_pattern, get_property(match[1]), std::regex_constants::format_first_only);
            } else {
                break;
            }
        }
    };
    while (::std::getline(is, line)) {
        if (line.empty() || line[0] == '#') continue;
        if (::std::regex_search(line, match, find_kv_pattern)) {
            key = match[2].str();
            value = match[4].str();
            replace_varible(key);
            replace_varible(value);
        }
        set_property(key, value);
    }
}

void properties::store(::std::ostream& os) const {
    for (const auto& p : value_) {
        os << p.first << "=" << p.second << ::std::endl;
    }
}

::std::string properties::delete_property(const ::std::string& key) _SW_NOEXCEPT {
    static const ::std::string empty_str;
    if (contains_key(key)) {
        auto index = map_[key];
        auto old_value = value_[index].second;
        value_.erase(value_.begin() + index);
        return old_value;
    }
    return empty_str;
}

_SW_END // _SW_BEGIN
