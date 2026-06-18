#pragma once

#include "Core.hh"

#ifdef _WIN32
#define HYPERION_PLATFORM_WINDOWS
#elif __APPLE__
#define HYPERION_PLATFORM_DARWIN
#else
#define HYPERION_PLATFORM_LINUX
#endif

namespace hyperion {

enum class OS {
    linux,
    windows,
    darwin,
};

inline constexpr OS os() {
#ifdef HYPERION_PLATFORM_WINDOWS
    return OS::windows;
#elif defined(HYPERION_PLATFORM_DARWIN)
    return OS::darwin;
#else
    return OS::linux;
#endif
}

Str toString(OS os);

}  // namespace hyperion
