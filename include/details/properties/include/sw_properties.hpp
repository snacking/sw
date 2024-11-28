// header file for sw::properties

#ifndef _SW_PROPERTIES_H_
#define _SW_PROPERTIES_H_

#include "../../sw_vals.h"

#include <iostream>
#include <string>
#include <sstream>
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <iterator>
#include <regex>

#define _SW_PROPERTIES_VER 1

_SW_BEGIN

class properties {
public:
    using size_type = ::std::size_t;
    using value_type = ::std::vector<::std::pair<::std::string, ::std::string> >;
    using iterator = value_type::iterator;
    using const_iterator =  value_type::const_iterator;

    explicit properties() = default;

    ~properties() = default;

    void load(::std::istream &);

    void store(::std::ostream &) const;

    inline size_type size() const;

    inline bool empty() const;

    inline const ::std::string get_property(const ::std::string &) const;

    inline const ::std::string get_property(const ::std::string &, const ::std::string &) const;

    inline void set_property(const ::std::string &, const ::std::string &);

    ::std::string delete_property(const ::std::string &);

    inline ::std::string &operator [](const ::std::string &);

    inline bool contains_key(const ::std::string &) const;

    inline void clear();

    inline iterator begin();

    inline iterator end();

    inline const_iterator begin() const;

    inline const_iterator end() const;
private:
    ::std::unordered_map<::std::string, ::std::size_t> map_;
    value_type value_;
};

::std::vector<::std::string> split(const ::std::string &, char);

::std::string trim(const ::std::string &);

_SW_END // _SW_BEGIN

#include "./sw_properties.ipp"

#ifdef _SW_HEADER_ONLY
#include "../src/sw_properties.cc" 
#endif // _SW_HEADER_ONLY

#endif // _SW_PROPERTIES_H_
