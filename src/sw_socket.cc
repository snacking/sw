// implementation file for sw::socket

#include "sw/sw_socket.hpp"

_SW_BEGIN

socket_error get_socket_error() {
    const int error = GET_SOCKET_ERROR();
    switch (error) {
#if defined(_WIN32) || defined(_M_X64)
        case WSAEINTR:            return socket_error::interrupted;
        case WSAEBADF:            return socket_error::bad_file_descriptor;
        case WSAEACCES:           return socket_error::access_denied;
        case WSAEINVAL:           return socket_error::invalid_argument;
        case WSAEMFILE:           return socket_error::too_many_open_files;
        case WSAEWOULDBLOCK:      return socket_error::would_block;
        case WSAEINPROGRESS:      return socket_error::in_progress;
        case WSAEADDRINUSE:       return socket_error::already_in_use;
        case WSAECONNREFUSED:     return socket_error::connection_refused;
        case WSAECONNRESET:       return socket_error::connection_reset;
        case WSAECONNABORTED:     return socket_error::connection_aborted;
        case WSAENOTCONN:         return socket_error::not_connected;
        case WSAETIMEDOUT:        return socket_error::timed_out;
        case WSAEMSGSIZE:         return socket_error::message_too_long;
        case WSAENETUNREACH:      return socket_error::network_unreachable;
        case WSAEHOSTUNREACH:     return socket_error::host_unreachable;
        case WSAEPROTONOSUPPORT:  return socket_error::protocol_error;
        case WSAEAFNOSUPPORT:     return socket_error::address_family_not_supported;
        default:                  return socket_error::unknown_error;
#elif defined(__linux__) // defined(_WIN32) || defined(_M_X64)
        case EINTR:               return socket_error::interrupted;
        case EBADF:               return socket_error::bad_file_descriptor;
        case EACCES:              return socket_error::access_denied;
        case EINVAL:              return socket_error::invalid_argument;
        case EMFILE:              return socket_error::too_many_open_files;
        case EWOULDBLOCK:         return socket_error::would_block;
        case EINPROGRESS:         return socket_error::in_progress;
        case EADDRINUSE:          return socket_error::already_in_use;
        case ECONNREFUSED:        return socket_error::connection_refused;
        case ECONNRESET:          return socket_error::connection_reset;
        case ECONNABORTED:        return socket_error::connection_aborted;
        case ENOTCONN:            return socket_error::not_connected;
        case ETIMEDOUT:           return socket_error::timed_out;
        case EMSGSIZE:            return socket_error::message_too_long;
        case ENETUNREACH:         return socket_error::network_unreachable;
        case EHOSTUNREACH:        return socket_error::host_unreachable;
        case EPROTONOSUPPORT:     return socket_error::protocol_error;
        case EAFNOSUPPORT:        return socket_error::address_family_not_supported;
        default:                  return socket_error::unknown_error;
#endif // defined(_WIN32) || defined(_M_X64)
    }
}

::std::string get_socket_error_string(socket_error error) {
    static const std::unordered_map<socket_error, ::std::string> error_strings = {
        {socket_error::success,                       "Operation succeeded"},
        {socket_error::interrupted,                   "Operation interrupted by signal"},
        {socket_error::bad_file_descriptor,           "Bad file descriptor"},
        {socket_error::access_denied,                 "Permission denied"},
        {socket_error::invalid_argument,              "Invalid argument"},
        {socket_error::too_many_open_files,           "Too many open files"},
        {socket_error::would_block,                   "Operation would block"},
        {socket_error::in_progress,                   "Operation in progress"},
        {socket_error::already_in_use,                "Address already in use"},
        {socket_error::connection_refused,            "Connection refused"},
        {socket_error::connection_reset,              "Connection reset by peer"},
        {socket_error::connection_aborted,            "Software caused connection abort"},
        {socket_error::not_connected,                 "Socket is not connected"},
        {socket_error::timed_out,                     "Connection timed out"},
        {socket_error::message_too_long,              "Message too long"},
        {socket_error::network_unreachable,           "Network is unreachable"},
        {socket_error::host_unreachable,              "Host is unreachable"},
        {socket_error::protocol_error,                "Protocol not supported"},
        {socket_error::address_family_not_supported,  "Address family not supported"},
        {socket_error::unknown_error,                 "Unknown socket error"}
    };
    return error_strings.at(error);
}

socket_exception::socket_exception(socket_error error_code, const ::std::string& msg)
    : ::std::runtime_error(msg), error_code_(error_code) {}

socket_error socket_exception::get_error_code() const {
    return error_code_;
}

::std::string get_socket_error_platform_string(int error) {
#if defined(_WIN32) || defined(_M_X64)
    LPSTR buffer = nullptr;
    FormatMessageA(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
      nullptr, error, 0,
      (LPSTR)buffer, 0, nullptr
    );
    ::std::string msg(buffer);
    LocalFree(buffer);
    return msg;
#elif defined(__linux__) // defined(_WIN32) || defined(_M_X64)
    return strerror(error);
#endif // defined(_WIN32) || defined(_M_X64)
}

socket::socket(protocol_type protocol = protocol_type::TCP, bool blocking = true)
    : protocol_(protocol), blocking_(blocking) {
    socket_ = INVALID_SOCKET_VAL;
#if defined(_WIN32) || defined(_M_X64)
    static std::unique_ptr<int, void(*)(int*)> wsa_guard(nullptr, [](int*) {
      WSACleanup();
    });
    if (!wsa_guard) {
      WSADATA wsaData;
      if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        THROW_CURRENT_SOCKET_EXCEPTION;
      }
      wsa_guard.reset(reinterpret_cast<int*>(1));
    }
#endif // defined(_WIN32) || defined(_M_X64)
    int type = (protocol == protocol_type::TCP ? SOCK_STREAM : SOCK_DGRAM);
    socket_ = ::socket(AF_INET, type, 0);
    if (INVALID_SOCKET_VAL == socket_) {
      THROW_CURRENT_SOCKET_EXCEPTION;
    }
#if defined(_WIN32) || defined(_M_X64)
    u_long mode = blocking ? 0 : 1;
    if (ioctlsocket(socket_, FIONBIO, &mode) != 0) {
      THROW_CURRENT_SOCKET_EXCEPTION;
    }
#elif defined(__linux__) // defined(_WIN32) || defined(_M_X64)
    int flags = fcntl(socket_, F_GETFL, 0);
    if (flags == -1) THROW_CURRENT_SOCKET_EXCEPTION;
    flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
    if (fcntl(socket_, F_SETFL, flags) == -1) {
      THROW_CURRENT_SOCKET_EXCEPTION;
    }
#endif // defined(__linux__)
    return;
}

socket::socket(protocol_type protocol, bool blocking, socket_type socket)
    : protocol_(protocol), blocking_(blocking), socket_(socket) {}

socket::~socket() {
    if (INVALID_SOCKET_VAL != socket_) {
#if defined(_WIN32) || defined(_M_X64)
        closesocket(socket_);
#elif defined(__linux__) // defined(_WIN32) || defined(_M_X64)
        close(socket_);
#endif // defined(__linux__)
    }
    return;
}

void socket::bind(const ::std::string& ip, ::std::uint16_t port) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
    if (::bind(socket_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        THROW_CURRENT_SOCKET_EXCEPTION;
    }
}

void socket::listen(int backlog = SOMAXCONN) {
    if (::listen(socket_, 1) < 0) {
        THROW_CURRENT_SOCKET_EXCEPTION;
    }
}

void socket::connect(const ::std::string& ip, ::std::uint16_t port) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
    if (::connect(socket_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        THROW_CURRENT_SOCKET_EXCEPTION;
    }
}

socket::size_type socket::send(void* buffer, size_type length) {
    int flags = 0;
#if defined(_WIN32) || defined(_M_X64)
    int result = ::send(socket_, static_cast<const char*>(buffer), length, flags);
#elif defined(__linux__) // defined(_WIN32) || defined(_M_X64)
    int result = ::send(socket_, buffer, length, flags);
#endif // defined(__linux__)
    if (result < 0) {
        if (!blocking_ && get_socket_error() == socket_error::would_block) {
             return 0;
        }
        THROW_CURRENT_SOCKET_EXCEPTION;
    }
    return static_cast<::std::size_t>(result);
}

socket::size_type socket::receive(void* buffer, size_type max_length) {
    int flags = 0;
#if defined(_WIN32) || defined(_M_X64)
    int result = ::recv(socket_, static_cast<char*>(buffer), max_length, flags);
#elif defined(__linux__) // defined(_WIN32) || defined(_M_X64)
    int result = ::recv(socket_, buffer, max_length, flags);
#endif // defined(__linux__)
    if (result < 0) {
        if (!blocking_ && get_socket_error() == socket_error::would_block) {
            return 0;
        }
        THROW_CURRENT_SOCKET_EXCEPTION;
    }
    return static_cast<::std::size_t>(result);
}

socket socket::accept() {
    sockaddr_in client_addr{};
    socklen_t addr_len = sizeof(client_addr);
    socket_type client_sock = ::accept(
        socket_,
        reinterpret_cast<sockaddr*>(&client_addr),
#if defined(_WIN32) || defined(_M_X64)
        reinterpret_cast<int*>(&addr_len)
#elif defined(__linux__) // defined(_WIN32) || defined(_M_X64)
        &addr_len
#endif // defined(__linux__)
    );
    if (INVALID_SOCKET_VAL == client_sock) {
        THROW_CURRENT_SOCKET_EXCEPTION;
    }
    return socket(protocol_type::TCP, blocking_, client_sock);
}

_SW_END // _SW_BEGIN

