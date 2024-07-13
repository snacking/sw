/***
* ==++==
* By Lisw
* 
* ==--==
* =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
* Main.cpp
*
* Test file for json.hpp
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
***/

#include "../../include/json.hpp"
#include <iostream>

int main() {
    sw::json j, j1;
    sw::json j2(sw::type::array);
    j["menu1"]["menu2"]["menu3"] = "hello world";
    // j[0]["123"] = (double)2.0;
    j["name"] = "tsinghua";
    j["campus"] = (double)1.0;
    j["is_good"] = false;
    j["is_good"] = true;
    j1["happy_index"] = (std::uint64_t)15;
    j1["sad_index"] = (std::uint64_t)11;
    j["details"] = j1;
    j2[3] = "masaka";
    j2[2] = "hehehe";
    j2[1] = "qweasd";
    j2[0] = (double)0.44;
    std::cout << j2.serialize() << std::endl;
    j["meaningless"] = j2;
    std::cout << j.serialize();
    return 0;
}