/***
* ==++==
* By Lisw
* 
* ==--==
* =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
* Input adapters
*
* Main public header file for input adapters
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
***/

#pragma once

// std::FILE
#include <cstdio>
// std::istream
#include <istream>
// assert
#include <cassert>
// std::char_traits
#include <iosfwd>

namespace sw {

namespace detail {

enum class input_format_t { json };

class file_input_adapter {
public:
    using char_type = char;

    explicit file_input_adapter(std::FILE* f) noexcept : file_(f) {
        assert(file_ != nullptr);
    }

    file_input_adapter(const file_input_adapter&) = delete;

    file_input_adapter(file_input_adapter&&) noexcept = default;

    file_input_adapter& operator=(const file_input_adapter&) = delete;

    file_input_adapter& operator=(file_input_adapter&&) = delete;

    ~file_input_adapter() = default;

    std::char_traits<char>::int_type get_character() noexcept {
        return std::fgetc(file_);
    }
private:
    std::FILE* file_;
};

class input_stream_adapter {
public:
    using char_type = char;

    ~input_stream_adapter() {
        if (is_ != nullptr) {
            is_->clear(is_->rdstate() & std::ios::eofbit);
        }
    }

    explicit input_stream_adapter(std::istream& i)
        : is_(&i), sb_(i.rdbuf()) {}

    input_stream_adapter(const input_stream_adapter&) = delete;

    input_stream_adapter& operator=(input_stream_adapter&) = delete;

    input_stream_adapter& operator=(input_stream_adapter&&) = delete;

    input_stream_adapter(input_stream_adapter&& rhs) noexcept
        : is_(rhs.is_), sb_(rhs.sb_) {
        rhs.is_ = nullptr;
        rhs.sb_ = nullptr;
    }

    std::char_traits<char>::int_type get_character() {
        auto res = sb_->sbumpc();
        if (res == std::char_traits<char>::eof()) {
            is_->clear(is_->rdstate() | std::ios::eofbit);
        }
        return res;
    }

private:
    std::istream* is_ = nullptr;
    std::streambuf* sb_ = nullptr;
};

}

}
