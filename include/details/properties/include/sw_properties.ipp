// implementation file for sw::properties (inline members) (internal)

#include "../include/sw_properties.hpp"

_SW_BEGIN

inline properties::size_type properties::size() const {
    return value_.size();
}

inline bool properties::empty() const {
    return value_.empty();
}

inline const ::std::string properties::get_property(const ::std::string &key) const {
    return value_.at(map_.at(key)).second;
}

inline const ::std::string properties::get_property(const ::std::string &key, const ::std::string &default_value) const {
    return contains_key(key) ? value_.at(map_.at(key)).second : default_value;
}

inline void properties::set_property(const ::std::string &key, const ::std::string &value) {
    if (!contains_key(key)) {
        map_[key] = value_.size();
        value_.push_back(::std::make_pair(key, value));
    } else {
        value_[map_.at(key)].second = value;
    }
}

inline ::std::string &properties::operator [](const ::std::string &key) {
    return value_[map_[key]].second;
}

inline bool properties::contains_key(const ::std::string &key) const {
    return map_.find(key) != map_.end();
}

inline void properties::clear() {
    map_.clear();
    value_.clear();
}

inline properties::iterator properties::begin() {
    return value_.begin();
}

inline properties::iterator properties::end() {
    return value_.end();
}

inline properties::const_iterator properties::begin() const {
    return value_.cbegin();
}

inline properties::const_iterator properties::end() const {
    return value_.cend();
}

_SW_END // _SW_BEGIN
