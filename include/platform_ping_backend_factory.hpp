#pragma once

#include "platform_ping_backend.hpp"

#include <memory>

namespace pingstats {

/// Creates the appropriate PlatformPingBackend for the current operating system at runtime.
/// On unsupported platforms, returns a stub backend that always reports "unsuccessful".
std::unique_ptr<PlatformPingBackend> make_platform_ping_backend();

} // namespace pingstats

