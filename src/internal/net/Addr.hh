#pragma once

#include "internal/core/Concepts.hh"
#include "internal/core/Core.hh"

namespace hyperion {

struct Addr {
    Str ip;
    u16 port;

    Str toString() const;
};

}  // namespace hyperion
