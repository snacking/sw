/***
* ==++==
* By Lisw
* 
* ==--==
* =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
* Parser
*
* Main public header file for EasyJSON
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
***/

#pragma once

#include "./detail/type.hpp"
#include "./detail/input/parser.hpp"
#include "./detail/iterators/iterator.hpp"
#include "./detail/output/serializer.hpp"

namespace sw {

enum class type : std::uint8_t
{
    array,            ///< array (ordered collection of values)
    object,           ///< object (set of unordered key/value pairs)
    ordered_object,   ///< object (set of ordered key/value pairs)
};

class json {
public:
    using size_type = std::size_t;
    using value_type = detail::value_t;
    using reference = detail::value_t&;
    using iterator = int;
    using const_iterator = int;
    using reverse_iterator = int;
    using const_reverse_iterator = int;

    json() : value_(detail::type::ordered_object) {}

    json(const type& t) {
        switch (t) {
        case type::array:
            value_ = detail::value_t(detail::type::array);
            break;
        case type::object:
            value_ = detail::value_t(detail::type::object);
            break;
        case type::ordered_object:
            value_ = detail::value_t(detail::type::ordered_object);
            break;
        }
    }

    ///< wrapper function for deserialize a json string to a json object
    template <typename _Fn>
    static json deserialize(const detail::json_string& s, _Fn fn) {
        return fn(s);
    }

    detail::json_string serialize() {
        return value_.serialize();
    }

    json& operator = (const json& j) {
        value_ = j.value_;
        return *this;
    }

    bool operator == (const json& j) const {
        return value_ == j.value_;
    }

    bool operator != (const json& j) const {
        return !(value_ == j.value_);
    }

    reference operator [] (const detail::json_string& key) {
        if (value_.type_ == detail::type::object || value_.type_ == detail::type::ordered_object) {
            return value_[key];
        }
        throw std::runtime_error("invalid type");
    }

    reference operator [] (std::size_t index) {
        if (value_.type_ == detail::type::array) {
            return value_[index];
        }
        throw std::runtime_error("invalid type");
    }

    iterator begin() {
        return iterator();
    }

    iterator end() {
        return iterator();
    }
private:
    value_type value_;     ///< value inside
};

}
