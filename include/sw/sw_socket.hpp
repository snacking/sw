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
    SUNCCESS = 0,
    INTERRUPTED,
    BAD_FILE_DESCRIPTOR,
    ACCESS_DENIED,
    INVALID_ARGUMENT,
    TOO_MANY_OPEN_FILES,
    WOULD_BLOCK,
    IN_PROGRESS,
    ALREADY_IN_USE,
    CONNECTION_REFUSED,
    CONNECTION_RESET,
    CONNECTION_ABORTED,
    NOT_CONNECTED,
    TIMED_OUT,
    MESSAGE_TOO_LONG,
    NETWORK_UNREACHABLE,
    HOST_UNREACHABLE,
    PROTOCOL_ERROR,
    ADDRESS_FAMILY_NOT_SUPPORTED,
    UNKNOWN_ERROR
};

socket_error get_socket_error() _SW_NOEXCEPT;

::std::string get_socket_error_string(socket_error) _SW_NOEXCEPT;

::std::string get_socket_error_platform_string(int);

class socket_exception : public ::std::runtime_error {
public:
    socket_exception(socket_error, const ::std::string&);

    socket_error get_error_code() const _SW_NOEXCEPT;
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
    socket(socket&&) _SW_NOEXCEPT;
    socket& operator=(socket&) = delete;
    socket& operator=(socket&&) _SW_NOEXCEPT;

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
