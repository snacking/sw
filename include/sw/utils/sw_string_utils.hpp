// header file for sw::STRING_UTILS

#ifndef _SW_STRING_UTILS_H_
#define _SW_STRING_UTILS_H_

#include "sw/internal/sw_vals.h"

#include <sstream>
#include <string>
#include <vector>

#define _SW_STRING_UTILS_VER 1

_SW_BEGIN

::std::vector<::std::string> split(const ::std::string&, char) _SW_NOEXCEPT;

::std::string trim(const ::std::string&) _SW_NOEXCEPT;

_SW_END // _SW_BEGIN

#endif // _SW_STRING_UTILS_H_
