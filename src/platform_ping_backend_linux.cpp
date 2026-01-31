#include "platform_ping_backend_linux.hpp"

#if !defined(__linux__)
#error "LinuxPingBackend is only available on Linux builds"
#endif

#include <arpa/inet.h>
#include <chrono>
#include <cstddef>
#include <cstring>
#include <netdb.h>
#include <netinet/ip_icmp.h>
#include <stdexcept>
#include <string>
#include <string_view>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

namespace pingstats {

namespace {

constexpr int kIcmpProtocol = IPPROTO_ICMP;
constexpr std::size_t kIcmpHeaderSize = sizeof(icmphdr);
constexpr std::size_t kPayloadSize = 56; // bytes, klassisch wie ping(8)
constexpr std::size_t kPacketSize = kIcmpHeaderSize + kPayloadSize;

std::uint16_t checksum(const void* data, std::size_t len) {
    const std::uint16_t* buf = static_cast<const std::uint16_t*>(data);
    std::uint32_t sum = 0;
    std::size_t nwords = len / 2;
    for (std::size_t i = 0; i < nwords; ++i) {
        sum += buf[i];
    }
    if (len & 1U) {
        sum += static_cast<const std::uint8_t*>(data)[len - 1];
    }
    while (sum >> 16U) {
        sum = (sum & 0xFFFFU) + (sum >> 16U);
    }
    return static_cast<std::uint16_t>(~sum);
}

} // namespace

LinuxPingBackend::LinuxPingBackend() = default;

LinuxPingBackend::~LinuxPingBackend() {
    shutdown();
}

void LinuxPingBackend::initialize() {
    if (initialized_) {
        return;
    }
    sock_fd_ = ::socket(AF_INET, SOCK_RAW, kIcmpProtocol);
    if (sock_fd_ < 0) {
        throw std::runtime_error("Failed to create raw ICMP socket (need CAP_NET_RAW or root)");
    }

    identifier_ = static_cast<std::uint16_t>(::getpid() & 0xFFFF);
    sequence_ = 0;
    initialized_ = true;
}

void LinuxPingBackend::shutdown() {
    if (sock_fd_ >= 0) {
        ::close(sock_fd_);
        sock_fd_ = -1;
    }
    initialized_ = false;
}

PlatformPingBackend::PingResult LinuxPingBackend::send_ping(std::string_view host, std::chrono::milliseconds timeout) {
    if (!initialized_) {
        throw std::runtime_error("LinuxPingBackend not initialized");
    }

    // Resolve host
    struct addrinfo hints {};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = kIcmpProtocol;
    struct addrinfo* res = nullptr;
    const std::string host_str(host);
    if (const int err = ::getaddrinfo(host_str.c_str(), nullptr, &hints, &res); err != 0) {
        throw std::runtime_error(std::string("getaddrinfo failed: ") + ::gai_strerror(err));
    }

    // Build packet
    std::uint8_t packet[kPacketSize]{};
    auto* hdr = reinterpret_cast<icmphdr*>(packet);
    hdr->type = ICMP_ECHO;
    hdr->code = 0;
    hdr->un.echo.id = htons(identifier_);
    hdr->un.echo.sequence = htons(++sequence_);
    std::memset(packet + kIcmpHeaderSize, 0x42, kPayloadSize);
    hdr->checksum = 0;
    hdr->checksum = checksum(packet, kPacketSize);

    // Destination address
    sockaddr_in dest{};
    std::memcpy(&dest, res->ai_addr, sizeof(sockaddr_in));
    ::freeaddrinfo(res);

    // Send timestamp
    const auto t0 = std::chrono::steady_clock::now();
    const auto sent = ::sendto(sock_fd_, packet, kPacketSize, 0, reinterpret_cast<sockaddr*>(&dest), sizeof(dest));
    if (sent < 0 || static_cast<std::size_t>(sent) != kPacketSize) {
        throw std::runtime_error("sendto failed");
    }

    // Wait using select()
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sock_fd_, &readfds);
    timeval tv{};
    tv.tv_sec = static_cast<long>(timeout.count() / 1000);
    tv.tv_usec = static_cast<long>((timeout.count() % 1000) * 1000);
    const int ready = ::select(sock_fd_ + 1, &readfds, nullptr, nullptr, &tv);
    if (ready <= 0) {
        return PingResult{false, 0.0}; // timeout or error
    }

    // Buffer incoming packet
    std::uint8_t recv_buf[512]{};
    sockaddr_in recv_addr{};
    socklen_t recv_len = sizeof(recv_addr);
    const auto recvd = ::recvfrom(sock_fd_, recv_buf, sizeof(recv_buf), 0, reinterpret_cast<sockaddr*>(&recv_addr), &recv_len);
    if (recvd <= 0) {
        return PingResult{false, 0.0};
    }

    const auto t1 = std::chrono::steady_clock::now();
    const double rtt_ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

    // Minimal validation: ICMP Echo Reply
    if (static_cast<std::size_t>(recvd) < kIcmpHeaderSize + sizeof(iphdr)) {
        return PingResult{false, 0.0};
    }

    // Skip IP header
    auto* ip_hdr = reinterpret_cast<iphdr*>(recv_buf);
    const std::size_t ip_header_len = static_cast<std::size_t>(ip_hdr->ihl * 4);
    if (ip_header_len + kIcmpHeaderSize > static_cast<std::size_t>(recvd)) {
        return PingResult{false, 0.0};
    }
    auto* icmp_hdr = reinterpret_cast<icmphdr*>(recv_buf + ip_header_len);

    const bool is_reply = icmp_hdr->type == ICMP_ECHOREPLY && icmp_hdr->code == 0 &&
                          icmp_hdr->un.echo.id == htons(identifier_);

    if (!is_reply) {
        return PingResult{false, 0.0};
    }

    return PingResult{true, rtt_ms};
}

} // namespace pingstats

