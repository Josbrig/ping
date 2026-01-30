#include "platform_ping_backend_windows.hpp"

#include <chrono>
#include <cstring>
#include <stdexcept>
#include <string>
#include <system_error>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <icmpapi.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

namespace pingstats {

namespace {

struct IcmpEchoReply {
    ICMP_ECHO_REPLY reply;
    unsigned char data[64];
};

void throw_last_error(const char* msg) {
    const auto err = ::WSAGetLastError();
    throw std::system_error(err, std::system_category(), msg);
}

} // namespace

WindowsPingBackend::~WindowsPingBackend() {
    shutdown();
    if (wsa_started_) {
        ::WSACleanup();
    }
}

void WindowsPingBackend::initialize() {
    if (initialized_) {
        return;
    }

    WSADATA wsaData{};
    if (const auto res = ::WSAStartup(MAKEWORD(2, 2), &wsaData); res != 0) {
        throw std::system_error(res, std::system_category(), "WSAStartup failed");
    }
    wsa_started_ = true;

    icmp_handle_ = ::IcmpCreateFile();
    if (icmp_handle_ == INVALID_HANDLE_VALUE) {
        throw_last_error("IcmpCreateFile failed");
    }

    initialized_ = true;
}

void WindowsPingBackend::shutdown() {
    if (icmp_handle_ && icmp_handle_ != INVALID_HANDLE_VALUE) {
        ::IcmpCloseHandle(icmp_handle_);
        icmp_handle_ = nullptr;
    }
    initialized_ = false;
}

void WindowsPingBackend::resolve_host(std::string_view host, unsigned long& out_ipv4) const {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    if (::InetPtonA(AF_INET, std::string(host).c_str(), &addr.sin_addr) != 1) {
        addrinfo hints{};
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        addrinfo* result = nullptr;
        const auto rc = ::getaddrinfo(std::string(host).c_str(), nullptr, &hints, &result);
        if (rc != 0) {
            throw std::runtime_error("Failed to resolve host: " + std::string(host));
        }
        out_ipv4 = reinterpret_cast<sockaddr_in*>(result->ai_addr)->sin_addr.S_un.S_addr;
        ::freeaddrinfo(result);
    } else {
        out_ipv4 = addr.sin_addr.S_un.S_addr;
    }
}

PlatformPingBackend::PingResult WindowsPingBackend::send_ping(std::string_view host, std::chrono::milliseconds timeout) {
    if (!initialized_) {
        throw std::runtime_error("WindowsPingBackend not initialized");
    }

    unsigned long dest_ipv4 = 0;
    resolve_host(host, dest_ipv4);

    constexpr char kPayload[] = "pingstats";
    IcmpEchoReply buffer{};

    const DWORD reply_size = sizeof(IcmpEchoReply);
    const auto start = std::chrono::high_resolution_clock::now();
    const auto reply_count = ::IcmpSendEcho(
        icmp_handle_,
        dest_ipv4,
        (LPVOID)kPayload,
        static_cast<WORD>(sizeof(kPayload)),
        nullptr,
        &buffer,
        reply_size,
        static_cast<DWORD>(timeout.count()));

    if (reply_count == 0) {
        const auto err = ::GetLastError();
        if (err == IP_REQ_TIMED_OUT) {
            return PingResult{false, 0.0};
        }
        throw std::system_error(static_cast<int>(err), std::system_category(), "IcmpSendEcho failed");
    }

    const auto end = std::chrono::high_resolution_clock::now();
    const auto rtt_ms = std::chrono::duration<double, std::milli>(end - start).count();

    const bool success = buffer.reply.Status == IP_SUCCESS;
    const double rtt = success ? static_cast<double>(buffer.reply.RoundTripTime) : 0.0;
    // Prefer API-provided RTT if available; fall back to measured.
    const double final_rtt = (buffer.reply.RoundTripTime > 0) ? rtt : rtt_ms;
    return PingResult{success, final_rtt};
}

} // namespace pingstats
