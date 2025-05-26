// header file for sw::socket

#ifndef _SOCKET_H_
#define _SOCKET_H_

#if defined(_WIN32) || defined(_M_X64)
    #define _WIN32_LEAN_AND_MEAN
    #define _WINSOCK_DEPRECATED_NO_WARNINGS
    #include <WinSock2.h>
    #include <WS2tcpip.h>
    #include <windows.h>
    #pragma comment(lib, "ws2_32.lib")
    #define GET_SOCKET_ERROR() WSAGetLastError()
    using SOCKET_TYPE = SOCKET;
    constexpr SOCKET_TYPE INVALID_SOCKET_VAL = INVALID_SOCKET;
#elif defined(__linux__) // defined(_WIN32) || defined(_M_X64)
    #include <arpa/inet.h>
    #include <cerrno>
    #include <cstring>
    #include <fcntl.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <unistd.h>
    #define GET_SOCKET_ERROR() errno
    using SOCKET_TYPE = int;
    constexpr SOCKET_TYPE INVALID_SOCKET_VAL = -1;
#endif // defined(__linux__)

#define THROW_CURRENT_SOCKET_EXCEPTION throw socket_exception(get_socket_error(), get_socket_error_string(get_socket_error()))

#include "sw/internal/sw_vals.h"

#include <cstddef>
#include <mutex>
#include <stdexcept>
#include <string>
#include <unordered_map>

_SW_BEGIN

enum class protocol_type : ::std::uint8_t {
    TCP = 0,
    UDP
};

enum class socket_error : ::std::uint8_t {
    success = 0,
    interrupted,
    bad_file_descriptor,
    access_denied,
    invalid_argument,
    too_many_open_files,
    would_block,
    in_progress,
    already_in_use,
    connection_refused,
    connection_reset,
    connection_aborted,
    not_connected,
    timed_out,
    message_too_long,
    network_unreachable,
    host_unreachable,
    protocol_error,
    address_family_not_supported,
    unknown_error
};

socket_error get_socket_error();

::std::string get_socket_error_string(socket_error);

::std::string get_socket_error_platform_string(int);

class socket_exception : public ::std::runtime_error {
public:
    socket_exception(socket_error, const ::std::string&);

    socket_error get_error_code() const;
private:
    socket_error error_code_;
};

class socket {
public:
    using socket_type = SOCKET_TYPE;
    using size_type = ::std::size_t;

    explicit socket(protocol_type, bool);
    explicit socket(protocol_type, bool, socket_type);
    ~socket();
    socket(const socket&) = delete;
    socket(socket&&);
    socket& operator=(socket&) = delete;
    socket& operator=(socket&&);

    void bind(const ::std::string&, ::std::uint16_t);
    void listen(int);
    void connect(const ::std::string&, ::std::uint16_t);
    size_type send(void*, size_type);
    size_type receive(void*, size_type);
    socket accept();
private:
    socket_type socket_;
    protocol_type protocol_;
    bool blocking_;
};

_SW_END // _SW_BEGIN

#endif // _SOCKET_H_
