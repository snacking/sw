/***
* ==++==
* By Lisw
* 
* ==--==
* =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
* type.cpp
*
* Test file for type.hpp
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
***/

#include "../../include/detail/type.hpp"
#include <iostream>
#include <cassert>

// #define _CRTDBG_MAP_ALLOC
// #include <stdlib.h>
// #include <crtdbg.h>

int main() {
    sw::detail::value_t null;
    sw::detail::value_t boolean(true);
    sw::detail::value_t c_str("asdf");
    sw::detail::value_t str(std::string("fdasfa"));
    sw::detail::value_t number((int64_t)1);
    sw::detail::value_t float_number(4.4);
    sw::detail::value_t array(sw::detail::type::array);
    sw::detail::value_t array2(sw::detail::type::array);
    sw::detail::value_t object(sw::detail::type::object);
    // sw::detail::value_t ordered_object(sw::detail::type::ordered_object);
    std::cout << "null.type(): " << null.type() << std::endl;
    std::cout << "boolean.type(): " << boolean.type() << std::endl;
    std::cout << "c_str.type(): " << c_str.type() << std::endl;
    std::cout << "str.type(): " << str.type() << std::endl;
    std::cout << "number.type(): " << number.type() << std::endl;
    std::cout << "float_number.type(): " << float_number.type() << std::endl;
    // std::cout << "array.type(): " << array.type() << std::endl;
    // std::cout << "object.type(): " << object.type() << std::endl;
    // std::cout << "ordered_object.type(): " << ordered_object.type() << std::endl;
    // std::cout << array.array.use_count() << std::endl;
    auto& array_value = array.get<sw::detail::array_t>();
    auto& array_value2 = array2.get<sw::detail::array_t>();
    auto& object_value = object.get<sw::detail::object_t>();
    // std::cout << array.array.use_count() << std::endl;
    array.array->emplace_back(null);
    array.array->emplace_back(boolean);
    array.array->emplace_back(c_str);
    array_value2.emplace_back(str);
    array_value2.emplace_back(number);
    array_value2.emplace_back(float_number);
    array.array->emplace_back(array_value2);
    object_value["this is a 64"] = (int64_t)1;
    object_value["nosense"] = (int64_t)12;
    array.array->emplace_back(object);
    // auto& object_value = object.get<sw::detail::object_t>();
    // auto& ordered_object_value = ordered_object.get<sw::detail::ordered_object_t>();
    // static_assert(std::is_same_v<decltype(array), sw::detail::value_t>, "is same.");
    std::cout << array.to_json() << std::endl;
    // for (const auto& value : array_value) {
        // std::cout << "array_value[].type(): " << value.type() << std::endl;
    // }

    // _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG); 
    // _CrtDumpMemoryLeaks();
    return 0;
}

