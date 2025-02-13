// header file for sw::properties

#ifndef _SW_PROPERTIES_H_
#define _SW_PROPERTIES_H_

#include "sw_vals.h"

#include <cstdint>
#include <iostream>
#include <iterator>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#define _SW_PROPERTIES_VER 1

_SW_BEGIN

class properties {
public:
    using size_type = ::std::size_t;
    using value_type = ::std::vector<::std::pair<::std::string, ::std::string> >;
    using iterator = value_type::iterator;
    using const_iterator =  value_type::const_iterator;

    explicit properties() _SW_NOEXCEPT = default;

    ~properties() _SW_NOEXCEPT = default;

    void load(::std::istream &);

    void store(::std::ostream &) const;

    inline size_type size() const _SW_NOEXCEPT;

    inline bool empty() const _SW_NOEXCEPT;

    inline const ::std::string get_property(const ::std::string &) const;

    inline const ::std::string get_property(const ::std::string &, const ::std::string &) const _SW_NOEXCEPT;

    inline void set_property(const ::std::string &, const ::std::string &) _SW_NOEXCEPT;

    ::std::string delete_property(const ::std::string &) _SW_NOEXCEPT;

    inline ::std::string &operator [](const ::std::string &) _SW_NOEXCEPT;

    inline bool contains_key(const ::std::string &) const _SW_NOEXCEPT;

    inline void clear() _SW_NOEXCEPT;

    inline iterator begin() _SW_NOEXCEPT;

    inline iterator end() _SW_NOEXCEPT;

    inline const_iterator begin() const _SW_NOEXCEPT;

    inline const_iterator end() const _SW_NOEXCEPT;
private:
    ::std::unordered_map<::std::string, ::std::size_t> map_;
    value_type value_;
};

::std::vector<::std::string> split(const ::std::string &, char) _SW_NOEXCEPT;

::std::string trim(const ::std::string &) _SW_NOEXCEPT;

_SW_END // _SW_BEGIN

#include "internal/sw_properties.ipp"

#endif // _SW_PROPERTIES_H_
