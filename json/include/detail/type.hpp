/***
* ==++==
* By Lisw
* 
* ==--==
* =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
* Parser
*
* Main public header file for json value type
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
***/

#pragma once

// std::map
#include <map>
// std::unordered_map
#include <unordered_map>
// std::int64_t, std::uint64_t, std::uint8_t
#include <cstdint>
// std::invalid_argument
#include <stdexcept>
// serialze
#include "./output/serializer.hpp"
// base64
#include "./conversions/base64.hpp"

namespace sw {

class json;
namespace detail {

struct value_t;

enum class type : std::uint8_t
{
    null,             ///< null value
    boolean,          ///< boolean value
    string,           ///< string value
    number_integer,   ///< number value (signed integer)
    number_unsigned,  ///< number value (unsigned integer)
    number_float,     ///< number value (floating-point)
    binary,           ///< binary array (ordered collection of bytes)
    array,            ///< array (ordered collection of values)
    object,           ///< object (set of unordered key/value pairs)
    ordered_object,   ///< object (set of ordered key/value pairs)
    discarded,        ///< discarded by the parser callback function
    uninitialized     ///< uninitialize value_t object
};

constexpr const char *type_names[] = {
    "null",
    "boolean",
    "string",
    "number_integer",
    "number_unsigned",
    "number_float",
    "binary",
    "array",
    "object",
    "ordered_object",
    "discarded"
};

using boolean_t = bool;
using string_t = std::string;
using number_t = union {
    std::int64_t number_integer;
    std::uint64_t number_unsigned;
    double number_float;
};
using binary_t = std::vector<std::uint8_t>;
using array_t = std::vector<value_t>;
using object_t = std::unordered_map<string_t, value_t>;
using ordered_object_t = std::map<string_t, value_t>;

template <typename _Tp>
_Tp make() {
    return _Tp();
}

template <>
boolean_t make<boolean_t>() {
    return false;
}

template <>
string_t make<string_t>() {
    return std::string();
}

template <>
number_t make<number_t>() {
    return {0};
}

template <>
binary_t make<binary_t>() {
    return {};
}

template <>
array_t make<array_t>() {
    return {};
}

// template <>
// object_t make<object_t>() {
//     return object_t();
// }

// template <>
// ordered_object_t make<ordered_object_t>() {
//     return ordered_object_t();
// }

/*

JSON type       | valueT type        | used type
----------------| -------------------|------------------------
null            | null               | *no value is stored*
boolean         | boolean_t          | @ref boolean_t
string          | string_t           | pointer to @ref string_t
number          | number_t           | @ref number_t
binary          | binary_t           | pointer to @ref binary_t
array           | array_t            | pointer to @ref array_t
object          | object_t           | pointer to @ref object_t
ordered_object  | ordered_object_t   | pointer to @ref ordered_object_t

*/
struct value_t {
public:
    value_t() : type_(type::uninitialized) {};

    explicit value_t(const boolean_t& b) : type_(type::boolean), boolean{b} {}

    explicit value_t(char *s) : type_(type::string), string(new string_t(s)) {}

    explicit value_t(const string_t& s) : type_(type::string), string(new string_t(s)) {}

    explicit value_t(const std::int64_t& n) : type_(type::number_integer) { number.number_integer = n; }

    explicit value_t(const std::uint64_t& n) : type_(type::number_unsigned) { number.number_unsigned = n; }

    explicit value_t(const double& n) : type_(type::number_float) { number.number_float = n; }

    explicit value_t(const binary_t& b) : type_(type::binary), binary(new binary_t(b)) {}

    explicit value_t(const array_t& a) : type_(type::array), array(new array_t(a)) {}

    explicit value_t(const object_t& o) : type_(type::object), object(new object_t(o)) {}

    explicit value_t(const ordered_object_t& o) : type_(type::ordered_object), ordered_object(new ordered_object_t(o)) {}
    
    value_t(const value_t& v) : type_(v.type_) {
        switch (type_) {
        case type::boolean:
            boolean = v.boolean;
            break;
        case type::string:
            string = new string_t(*v.string);
            break;
        case type::number_integer:
            number.number_integer = v.number.number_integer;
            break;
        case type::number_unsigned:
            number.number_unsigned = v.number.number_unsigned;
            break;
        case type::number_float:
            number.number_float = v.number.number_float;
            break;
        case type::binary:
            binary = new binary_t(*v.binary);
            break;
        case type::array:
            array = new array_t(*v.array);
            break;
        case type::object:
            object = new object_t(*v.object);
            break;
        case type::ordered_object:
            ordered_object = new ordered_object_t(*v.ordered_object);
            break;
        }
    }

    template <typename _Tp, typename = std::enable_if_t<std::is_same_v<json, _Tp> > >
    value_t& operator = (_Tp& v) {
        _Detor_helper();
        value_t _V = *reinterpret_cast<value_t*>(&v);
        type_ = _V.type_;
        switch (type_) {
        case type::boolean:
            boolean = _V.boolean;
            break;
        case type::string:
            string = new string_t(*_V.string);
            break;
        case type::number_integer:
            number.number_integer = _V.number.number_integer;
            break;
        case type::number_unsigned:
            number.number_unsigned = _V.number.number_unsigned;
            break;
        case type::number_float:
            number.number_float = _V.number.number_float;
            break;
        case type::binary:
            binary = new binary_t(*_V.binary);
            break;
        case type::array:
            array = new array_t(*_V.array);
            break;
        case type::object:
            object = new object_t(*_V.object);
            break;
        case type::ordered_object:
            ordered_object = new ordered_object_t(*_V.ordered_object);
            break;
        }
        return *this;
    }

    value_t& operator = (const value_t& v) {
        _Detor_helper();
        type_ = v.type_;
        switch (type_) {
        case type::boolean:
            boolean = v.boolean;
            break;
        case type::string:
            string = new string_t(*v.string);
            break;
        case type::number_integer:
            number.number_integer = v.number.number_integer;
            break;
        case type::number_unsigned:
            number.number_unsigned = v.number.number_unsigned;
            break;
        case type::number_float:
            number.number_float = v.number.number_float;
            break;
        case type::binary:
            binary = new binary_t(*v.binary);
            break;
        case type::array:
            array = new array_t(*v.array);
            break;
        case type::object:
            object = new object_t(*v.object);
            break;
        case type::ordered_object:
            ordered_object = new ordered_object_t(*v.ordered_object);
            break;
        }
        return *this;
    }

    value_t& operator = (const boolean_t& b) {
        _Detor_helper();
        type_ = type::boolean;
        boolean = b;
        return *this;
    }

    value_t& operator = (const char *s) {
        _Detor_helper();
        type_ = type::string;
        string = new string_t(s);
        return *this;
    }

    value_t& operator = (const string_t& s) {
        _Detor_helper();
        type_ = type::string;
        string = new string_t(s);
        return *this;
    }

    value_t& operator = (const std::int64_t& n) {
        _Detor_helper();
        type_ = type::number_integer;
        number.number_integer = n;
        return *this;
    }

    value_t& operator = (const std::uint64_t& n) {
        _Detor_helper();
        type_ = type::number_unsigned;
        number.number_unsigned = n;
        return *this;
    }

    value_t& operator = (const double& n) {
        _Detor_helper();
        type_ = type::number_float;
        number.number_float = n;
        return *this;
    }

    value_t& operator = (const binary_t& b) {
        _Detor_helper();
        type_ = type::binary;
        binary = new binary_t(b);
        return *this;
    }

    value_t& operator = (const array_t& a) {
        _Detor_helper();
        type_ = type::array;
        array = new array_t(a);
        return *this;
    }

    value_t& operator = (const object_t& o) {
        _Detor_helper();
        type_ = type::object;
        object = new object_t(o);
        return *this;
    }

    value_t& operator = (const ordered_object_t& o) {
        _Detor_helper();
        type_ = type::ordered_object;
        ordered_object = new ordered_object_t(o);
        return *this;
    }

    value_t& operator [] (const json_string& key) {
        if (type_ == type::uninitialized) {
            type_ = type::ordered_object;
            ordered_object = new ordered_object_t(ordered_object_t());
        }
        if (type_ == type::object) {
            auto& _Obj = *object;
            return _Get_or_create(_Obj, key);
        }
        if (type_ == type::ordered_object) {
            auto& _Obj = *ordered_object;
            return _Get_or_create(_Obj, key);
        }
        throw std::runtime_error("invalid type");
    }

    value_t& operator [] (std::size_t index) {
        if (type_ == type::uninitialized) {
            type_ = type::array;
            array = new array_t(array_t());
        }
        if (type_ == type::array) {
            auto& _Arr = *array;
            if (index >= _Arr.size()) {
                _Arr.resize(index + 1);
            }
            return _Arr[index];
        }
        throw std::runtime_error("invalid type");
    }

    template <typename _Tp, typename _Idx>
    value_t& _Get_or_create(_Tp& obj, const _Idx& key) {
        if (obj.find(key) != obj.end()) {
            return obj[key];
        }
        return obj.emplace(key, detail::value_t()).first->second;
    }

    bool operator == (const value_t& v) const {
        if (type_ != v.type_) return false;
        switch (type_) {
        case type::boolean:
            return boolean == v.boolean;
        case type::string:
            return *string == *(v.string);
        case type::number_integer:
            return number.number_integer == v.number.number_integer;
        case type::number_unsigned:
            return number.number_unsigned == v.number.number_unsigned;
        case type::number_float:
            return number.number_float == v.number.number_float;
        case type::binary:
            return *binary == *(v.binary);
        case type::array:
            return *array == *(v.array);
        case type::object:
            return *object == *(v.object);
        case type::ordered_object:
            return *ordered_object == *(v.ordered_object);
        }
        return false;
    }

    value_t(const type& t) : type_(t) {
        switch (t) {
        case type::boolean:
            boolean = make<boolean_t>();
            break;
        case type::string:
            string = new string_t(make<string_t>());
            break;
        case type::number_integer:
            number.number_integer = make<number_t>().number_integer;
            break;
        case type::number_unsigned:
            number.number_unsigned = make<number_t>().number_unsigned;
            break;
        case type::number_float:
            number.number_float = make<number_t>().number_float;
            break;
        case type::binary:
            binary = new binary_t(make<binary_t>());
            break;
        case type::array:
            array = new array_t(make<array_t>());
            break;
        case type::object:
            object = new object_t(object_t());
            break;
        case type::ordered_object:
            ordered_object = new ordered_object_t(ordered_object_t());
            break;
        }
    }

    void _Detor_helper() {
        if (type_ == type::uninitialized || type_ == type::null ||
            type_ == type::boolean || type_ == type::number_integer ||
            type_ == type::number_unsigned || type_ == type::number_float ||
            type_ == type::string && string == nullptr || 
            type_ == type::binary && binary == nullptr ||
            type_ == type::array && array == nullptr ||
            type_ == type::object && object == nullptr ||
            type_ == type::ordered_object && ordered_object == nullptr) {
            return;
        }
        switch (type_) {
        case type::string:
            delete string;
            break;
        case type::binary:
            delete binary;
            break;
        case type::array:
            for (auto& v : *array) {
                v._Detor_helper(); // put all v into a stack to avoid recursive call. (scheduled)
            }
            delete array;
            break;
        case type::object:
            for (auto& [k, v] : *object) {
                v._Detor_helper(); // put all v into a stack to avoid recursive call. (scheduled)
            }
            delete object;
            break;
        case type::ordered_object:
            for (auto& [k, v] : *ordered_object) {
                v._Detor_helper(); // put all v into a stack to avoid recursive call. (scheduled)
            }
            delete ordered_object;
            break;
        }
        return;
    }

    ~value_t() {
        _Detor_helper();
    }

    std::string type() {
        if (type_ == type::uninitialized) {
            type_ = type::null;
        }
        return type_names[static_cast<std::size_t>(type_)]; 
    }

    template <typename _Tp>
    _Tp& get() {
        if (type_ == type::uninitialized) {
            type_ = type::null;
        }
        switch (type_) {
        case type::null:
            if constexpr(std::is_pointer<_Tp>::value)
                return nullptr;
        case type::boolean:
            if constexpr(std::is_same<boolean_t, _Tp>::value)
                return boolean;
        case type::string:
            if constexpr(std::is_same<string_t, _Tp>::value)
                return *string;
        case type::number_integer:
            if constexpr(std::is_same<std::int64_t, _Tp>::value)
                return number.number_integer;
        case type::number_unsigned:
            if constexpr(std::is_same<std::uint64_t, _Tp>::value)
                return number.number_unsigned;
        case type::number_float:
            if constexpr(std::is_same<double, _Tp>::value)
                return number.number_float;
        case type::binary:
            if constexpr(std::is_same<binary_t, _Tp>::value)
                return *binary;
        case type::array:
            if constexpr(std::is_same<array_t, _Tp>::value)
                return *array;
        case type::object:
            if constexpr(std::is_same<object_t, _Tp>::value)
                return *object;
        case type::ordered_object:
            if constexpr(std::is_same<ordered_object_t, _Tp>::value)
                return *ordered_object;
        }
        throw std::invalid_argument("Type not supported for get function");
    }

    json_string serialize() {
        json_string _Ret;    ///< return value of type::array and type::object
        bool _First = true;  ///< decide wether to add comma in return value of type::array and type::object
        if (type_ == type::uninitialized) {
            type_ = type::null;
        }
        switch (type_) {
        case detail::type::null:
            return "null";
        case detail::type::boolean:
            return boolean ? "true" : "false";
        case detail::type::string:
            return "\"" + *string + "\"";
        case detail::type::number_integer:
            return std::to_string(number.number_integer);
        case detail::type::number_unsigned:
            return std::to_string(number.number_unsigned);
        case detail::type::number_float:
            return std::to_string(number.number_float);
        case detail::type::binary:
            return sw::base64_encode(*binary, sizeof(*binary) * 8);
        case detail::type::array:
            _Ret = "[";
            for (auto& v : *array) {
                if (!_First) _Ret += ", ";
                _Ret += v.serialize();
                _First = false;
            }
            _Ret += "]";
            return _Ret;
        case detail::type::object:
            _Ret = "{";
            for (auto& [k, v] : *object) {
                if (!_First) _Ret += ", ";
                _Ret += "{\"" + k + "\": " + v.serialize() + "}";
                _First = false;
            }
            _Ret += "}";
            return _Ret;
        case detail::type::ordered_object:
            _Ret = "{";
            for (auto& [k, v] : *ordered_object) {
                if (!_First) _Ret += ", ";
                _Ret += "{\"" + k + "\": " + v.serialize() + "}";
                _First = false;
            }
            _Ret += "}";
            return _Ret;
        }
        throw std::invalid_argument("Type not supported for serialize");
    }

    union {
        boolean_t boolean;
        string_t *string;
        number_t number;
        binary_t *binary;
        array_t *array;
        object_t *object;
        ordered_object_t *ordered_object;
    };
    
    sw::detail::type type_; 
};

};

};
