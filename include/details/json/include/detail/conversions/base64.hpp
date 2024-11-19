/***
* ==++==
* By Lisw
* 
* ==--==
* =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
* Parser
*
* Main public header file for Base64 encoding and decoding
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
***/

#pragma once

// std::vector
#include <vector>
// std::uint8_t
#include <cstdint>
// std::string
#include <string>
// std::remove
#include <algorithm>

# if __cplusplus >= 201703L
    # include <string_view>
# endif

namespace sw {

using byte = std::uint8_t;
using binary_t = std::vector<byte>;

constexpr static const char* base64_chars[2] = {
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    "+/",
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    "-_" 
};

constexpr static byte base64_lookup[256] = {
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  62, 255,  62, 255,  63,
     52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255, 255, 255, 255, 255,
    255,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
     25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35, 255, 255, 255, 255,  63,
    255,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,
     51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
};

static inline std::string insert_linebreaks(std::string str, std::size_t distance) {
    if (!str.length()) {
        return "";
    }

    size_t _Pos = distance;

    while (_Pos < str.length()) {
        str.insert(_Pos, "\n");
        _Pos += distance + 1;
    }

    return str;
}

// String here could be std::string or std::string_view (Require c++17 or above)

template <typename Binary>
std::string base64_encode(Binary bytes_to_encode, size_t in_len, bool url = false) {

    size_t len_encoded = (in_len + 2) / 3 * 4;

    unsigned char trailing_char = url ? '.' : '=';
    const char* base64_chars_ = base64_chars[url];

    std::string ret;
    ret.reserve(len_encoded);

    unsigned int _Pos = 0;

    while (_Pos < in_len) {
        ret.push_back(base64_chars_[(bytes_to_encode[_Pos + 0] & 0xfc) >> 2]);

        if (_Pos+1 < in_len) {
           ret.push_back(base64_chars_[((bytes_to_encode[_Pos + 0] & 0x03) << 4) + ((bytes_to_encode[_Pos + 1] & 0xf0) >> 4)]);

           if (_Pos+2 < in_len) {
              ret.push_back(base64_chars_[((bytes_to_encode[_Pos + 1] & 0x0f) << 2) + ((bytes_to_encode[_Pos + 2] & 0xc0) >> 6)]);
              ret.push_back(base64_chars_[  bytes_to_encode[_Pos + 2] & 0x3f]);
           }
           else {
              ret.push_back(base64_chars_[(bytes_to_encode[_Pos + 1] & 0x0f) << 2]);
              ret.push_back(trailing_char);
           }
        }
        else {

            ret.push_back(base64_chars_[(bytes_to_encode[_Pos + 0] & 0x03) << 4]);
            ret.push_back(trailing_char);
            ret.push_back(trailing_char);
        }

        _Pos += 3;
    }


    return ret;
}

template <typename String, unsigned int line_length>
static std::string encode_with_line_breaks(String s) {
    return insert_linebreaks(base64_encode(s, false), line_length);
}

template <typename String>
static std::string encode_pem(String s) {
  return encode_with_line_breaks<String, 64>(s);
}

template <typename String>
static std::string encode_mime(String s) {
  return encode_with_line_breaks<String, 76>(s);
}

template <typename String>
static std::string encode(String s, bool url) {
  return base64_encode(reinterpret_cast<const unsigned char*>(s.data()), s.length(), url);
}

template <typename String>
static std::string decode(String const& encoded_string, bool remove_linebreaks) {
    if (encoded_string.empty()) return std::string();

    if (remove_linebreaks) {
       std::string copy(encoded_string);
       copy.erase(std::remove(copy.begin(), copy.end(), '\n'), copy.end());
       return decode(copy, false);
    }

    size_t _Length_of_string = encoded_string.length();
    size_t _Pos = 0;

    size_t approx_length_of_decoded_string = _Length_of_string / 4 * 3;
    std::string ret;
    ret.reserve(approx_length_of_decoded_string);

    while (_Pos < _Length_of_string) {
       size_t _Pos_of_char_1 = base64_lookup[encoded_string.at(_Pos + 1)];
       ret.push_back(static_cast<std::string::value_type>( ( (base64_lookup[encoded_string.at(_Pos + 0)] ) << 2 ) + ( (_Pos_of_char_1 & 0x30 ) >> 4)));
       if ((_Pos + 2 < _Length_of_string)       &&
            encoded_string.at(_Pos+2) != '='   &&
            encoded_string.at(_Pos+2) != '.'       
          )
       {
          unsigned int _Pos_of_char_2 = base64_lookup[encoded_string.at(_Pos + 2) ];
          ret.push_back(static_cast<std::string::value_type>(((_Pos_of_char_1 & 0x0f) << 4) + ((_Pos_of_char_2 & 0x3c) >> 2)));
          if ((_Pos + 3 < _Length_of_string )     &&
               encoded_string.at(_Pos+3) != '='  &&
               encoded_string.at(_Pos+3) != '.'
             )
          {
             ret.push_back(static_cast<std::string::value_type>(((_Pos_of_char_2 & 0x03) << 6 ) + base64_lookup[encoded_string.at(_Pos + 3)]));
          }
       }

       _Pos += 4;
    }

    return ret;
}

std::string base64_encode(std::string const& s, bool url) {
   return encode(s, url);
}

std::string base64_encode_pem (std::string const& s) {
   return encode_pem(s);
}

std::string base64_encode_mime(std::string const& s) {
   return encode_mime(s);
}

#if __cplusplus >= 201703L

std::string base64_encode(std::string_view s, bool url) {
   return encode(s, url);
}

std::string base64_encode_pem(std::string_view s) {
   return encode_pem(s);
}

std::string base64_encode_mime(std::string_view s) {
   return encode_mime(s);
}

std::string base64_decode(std::string_view s, bool remove_linebreaks) {
   return decode(s, remove_linebreaks);
}

#endif

};
