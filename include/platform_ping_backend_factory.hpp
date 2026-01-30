#pragma once

#include "platform_ping_backend.hpp"

#include <memory>

namespace pingstats {

// Erzeugt zur Laufzeit das passende PlatformPingBackend für das aktuelle Betriebssystem.
// Auf nicht unterstützten Plattformen wird ein Stub-Backend zurückgegeben, das immer "unsuccessful" liefert.
std::unique_ptr<PlatformPingBackend> make_platform_ping_backend();

} // namespace pingstats

