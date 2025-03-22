// implementation file for sw_string_utils (internal)

#include "sw/utils/sw_string_utils.hpp"

_SW_BEGIN

::std::vector<::std::string> split(const ::std::string& str, char delimiter) _SW_NOEXCEPT {
    ::std::vector<::std::string> elements;
    ::std::istringstream iss(str);
    ::std::string token;
    while (::std::getline(iss, token, delimiter)) {
        elements.push_back(token);
    }
    return elements;
}

::std::string trim(const ::std::string& str) _SW_NOEXCEPT {
    static const ::std::string whitespace_string = " \t\n\r\f\v";
    auto first = str.find_first_not_of(whitespace_string);
    return (first == ::std::string::npos) ? "" : str.substr(first, str.find_last_not_of(whitespace_string) - first + 1);
}

_SW_END // _SW_BEGIN
