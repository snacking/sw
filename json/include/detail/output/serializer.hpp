/***
* ==++==
* By Lisw
* 
* ==--==
* =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
* Parser
*
* Main public header file for serialize
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
***/

#pragma once

// std::enable_if, std::true_type, std::false_type, std::declval
#include <type_traits>
// static_assert
#include <cassert>
// std::string
#include <string>

namespace sw {

namespace detail {

using json_string = std::string;

};

};
