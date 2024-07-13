/***
* ==++==
* By Lisw
* 
* ==--==
* =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
* Lexer
*
* Main public header file for lexer
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
***/

#pragma once

#include "../type.hpp"
// std::unordered_set
#include <unordered_set>
// std::size_t
#include <cstddef>

namespace sw {

namespace detail {

class _Lexer_base {
public:
    enum class token_type
    {
        uninitialized,    ///< indicating the scanner is uninitialized
        literal_true,     ///< the `true` literal
        literal_false,    ///< the `false` literal
        literal_null,     ///< the `null` literal
        value_string,     ///< a string -- use get_string() for actual value
        value_unsigned,   ///< an unsigned integer -- use get_number_unsigned() for actual value
        value_integer,    ///< a signed integer -- use get_number_integer() for actual value
        value_float,      ///< an floating point number -- use get_number_float() for actual value
        begin_array,      ///< the character for array begin `[`
        begin_object,     ///< the character for object begin `{`
        end_array,        ///< the character for array end `]`
        end_object,       ///< the character for object end `}`
        name_separator,   ///< the name separator `:`
        value_separator,  ///< the value separator `,`
        parse_error,      ///< indicating a parse error
        end_of_input,     ///< indicating the end of the input buffer
        literal_or_value  ///< a literal or the begin of a value (only for diagnostics)
    };
};

template <typename _InputAdapter>
class lexer :
    public _Lexer_base {
public:
    using char_type = typename _InputAdapter::char_type;
    using char_int_type = typename std::char_traits<char_type>::int_type;
    using token_type = typename _Lexer_base::token_type;
    using size_type = std::size_t;

    token_type scan() {
        _Skip_whitescape();
    }
private:
    void _Initialize() {
        next_unget_ = false;
        whitescape_set_.emplace(c_new_line_char);
        whitescape_set_.emplace(c_return_char);
        whitescape_set_.emplace(c_tab_char);
        whitescape_set_.emplace(c_whitescape_char);
        current_ = std::char_traits<char_type>::eof();
        position_.chars_read_current_line = 0;
        position_.chars_read_total = 0;
        position_.lines_read = 0;
    }

    char_int_type _Get() {
        ++position_.chars_read_total;
        ++position_.chars_read_current_line;
        if (next_unget_) {
            next_unget_ = false;
        } 
        else {
            current_ = ia_.get_character();
        }
        if (current_ == '\n') {
            position_.chars_read_current_line = 0;
            ++position_.lines_read;
        }
        return current_;
    }

    void _Unget() {
        next_unget_ = true;
        return;
    }

    inline bool _Skip_bom() {
        if (get() == 0xEF) {
            return get() == 0xBB && get() == 0xBF;
        }
        unget();
        return true;
    }

    inline void _Skip_whitescape() {
        do {
            _Get();
        } while (whitescape_set_.find(current_) != whitescape_set_.end());
    }

    const char_int_type c_decmial_point_char_ = '.';
    const char_int_type c_return_char = '\r';
    const char_int_type c_new_line_char = '\n';
    const char_int_type c_tab_char = '\t';
    const char_int_type c_whitescape_char = ' ';
    std::unordered_set<char_type> whitescape_set_;

    _InputAdapter ia_;
    bool next_unget_;
    char_int_type current_;
    number_t number_value_;
    string_t string_value_;
    struct position_t {
        size_type chars_read_total;
        size_t chars_read_current_line;
        size_t lines_read;

        constexpr operator size_t () const {
            return chars_read_total;
        }
    } position_;

};


};


};
