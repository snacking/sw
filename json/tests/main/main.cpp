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
    sw::json j, j1, j3;
    sw::json j2(sw::type::array);
    j["menu1"]["menu2"]["menu3"] = "hello world";
    j["menu2"][0]["123"] = (double)2.0;
    j["name"] = "tsinghua";
    j["campus"] = (double)1.0;
    j["is_good"] = false;
    j["is_good"] = true;
    j["is_good1"][1] = false;
    j1["happy_index"] = (std::uint64_t)15;
    j1["sad_index"] = (std::uint64_t)11;
    j["details"] = j1;
    j2[3] = "masaka";
    j2[2] = "hehehe";
    j2[1] = "qweasd";
    j2[0] = (double)0.44;
    std::cout << j2.serialize() << std::endl;
    j["meaningless"] = j2;
    j3[1]["adsf"][1][32]["asdfds"] = (double)1;
    std::cout << j.serialize() << std::endl;
    std::cout << j3.serialize() << std::endl;
    return 0;
}