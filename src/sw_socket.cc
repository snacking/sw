// implementation file for sw::socket

#include "sw/sw_socket.hpp"

_SW_BEGIN

socket_error get_socket_error() _SW_NOEXCEPT {
    const int error = GET_SOCKET_ERROR();
    switch (error) {
#if defined(_WIN32) || defined(_M_X64)
        case WSAEINTR:            return socket_error::INTERRUPTED;
        case WSAEBADF:            return socket_error::BAD_FILE_DESCRIPTOR;
        case WSAEACCES:           return socket_error::ACCESS_DENIED;
        case WSAEINVAL:           return socket_error::INVALID_ARGUMENT;
        case WSAEMFILE:           return socket_error::TOO_MANY_OPEN_FILES;
        case WSAEWOULDBLOCK:      return socket_error::WOULD_BLOCK;
        case WSAEINPROGRESS:      return socket_error::IN_PROGRESS;
        case WSAEADDRINUSE:       return socket_error::ALREADY_IN_USE;
        case WSAECONNREFUSED:     return socket_error::CONNECTION_REFUSED;
        case WSAECONNRESET:       return socket_error::CONNECTION_RESET;
        case WSAECONNABORTED:     return socket_error::CONNECTION_ABORTED;
        case WSAENOTCONN:         return socket_error::NOT_CONNECTED;
        case WSAETIMEDOUT:        return socket_error::TIMED_OUT;
        case WSAEMSGSIZE:         return socket_error::MESSAGE_TOO_LONG;
        case WSAENETUNREACH:      return socket_error::NETWORK_UNREACHABLE;
        case WSAEHOSTUNREACH:     return socket_error::HOST_UNREACHABLE;
        case WSAEPROTONOSUPPORT:  return socket_error::PROTOCOL_ERROR;
        case WSAEAFNOSUPPORT:     return socket_error::ADDRESS_FAMILY_NOT_SUPPORTED;
        default:                  return socket_error::UNKNOWN_ERROR;
#elif defined(__linux__) // defined(_WIN32) || defined(_M_X64)
        case EINTR:               return socket_error::INTERRUPTED;
        case EBADF:               return socket_error::BAD_FILE_DESCRIPTOR;
        case EACCES:              return socket_error::ACCESS_DENIED;
        case EINVAL:              return socket_error::INVALID_ARGUMENT;
        case EMFILE:              return socket_error::TOO_MANY_OPEN_FILES;
        case EWOULDBLOCK:         return socket_error::WOULD_BLOCK;
        case EINPROGRESS:         return socket_error::IN_PROGRESS;
        case EADDRINUSE:          return socket_error::ALREADY_IN_USE;
        case ECONNREFUSED:        return socket_error::CONNECTION_REFUSED;
        case ECONNRESET:          return socket_error::CONNECTION_RESET;
        case ECONNABORTED:        return socket_error::CONNECTION_ABORTED;
        case ENOTCONN:            return socket_error::NOT_CONNECTED;
        case ETIMEDOUT:           return socket_error::TIMED_OUT;
        case EMSGSIZE:            return socket_error::MESSAGE_TOO_LONG;
        case ENETUNREACH:         return socket_error::NETWORK_UNREACHABLE;
        case EHOSTUNREACH:        return socket_error::HOST_UNREACHABLE;
        case EPROTONOSUPPORT:     return socket_error::PROTOCOL_ERROR;
        case EAFNOSUPPORT:        return socket_error::ADDRESS_FAMILY_NOT_SUPPORTED;
        default:                  return socket_error::UNKNOWN_ERROR;
#endif // defined(_WIN32) || defined(_M_X64)
    }
}

::std::string get_socket_error_string(socket_error error) _SW_NOEXCEPT {
    static const std::unordered_map<socket_error, ::std::string> error_strings = {
        {socket_error::SUNCCESS,                       "Operation succeeded"},
        {socket_error::INTERRUPTED,                   "Operation interrupted by signal"},
        {socket_error::BAD_FILE_DESCRIPTOR,           "Bad file descriptor"},
        {socket_error::ACCESS_DENIED,                 "Permission denied"},
        {socket_error::INVALID_ARGUMENT,              "Invalid argument"},
        {socket_error::TOO_MANY_OPEN_FILES,           "Too many open files"},
        {socket_error::WOULD_BLOCK,                   "Operation would block"},
        {socket_error::IN_PROGRESS,                   "Operation in progress"},
        {socket_error::ALREADY_IN_USE,                "Address already in use"},
        {socket_error::CONNECTION_REFUSED,            "Connection refused"},
        {socket_error::CONNECTION_RESET,              "Connection reset by peer"},
        {socket_error::CONNECTION_ABORTED,            "Software caused connection abort"},
        {socket_error::NOT_CONNECTED,                 "Socket is not connected"},
        {socket_error::TIMED_OUT,                     "Connection timed out"},
        {socket_error::MESSAGE_TOO_LONG,              "Message too long"},
        {socket_error::NETWORK_UNREACHABLE,           "Network is unreachable"},
        {socket_error::HOST_UNREACHABLE,              "Host is unreachable"},
        {socket_error::PROTOCOL_ERROR,                "Protocol not supported"},
        {socket_error::ADDRESS_FAMILY_NOT_SUPPORTED,  "Address family not supported"},
        {socket_error::UNKNOWN_ERROR,                 "Unknown socket error"}
    };
    return error_strings.at(error);
}

socket_exception::socket_exception(socket_error error_code, const ::std::string& msg)
    : ::std::runtime_error(msg), error_code_(error_code) {}

socket_error socket_exception::get_error_code() const _SW_NOEXCEPT {
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

socket::socket(socket&& other) _SW_NOEXCEPT {
    protocol_ = other.protocol_;
    blocking_ = other.blocking_;
    socket_ = other.socket_;
    other.socket_ = INVALID_SOCKET_VAL;
}

socket& socket::operator=(socket&& other) _SW_NOEXCEPT {
    if (this != &other) {
        protocol_ = other.protocol_;
        blocking_ = other.blocking_;
        socket_ = other.socket_;
        other.socket_ = INVALID_SOCKET_VAL;
    }
    return *this;
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
        if (!blocking_ && get_socket_error() == socket_error::WOULD_BLOCK) {
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
        if (!blocking_ && get_socket_error() == socket_error::WOULD_BLOCK) {
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

