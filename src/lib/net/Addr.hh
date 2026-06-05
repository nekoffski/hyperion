#pragma once

#include "lib/core/Concepts.hh"
#include "lib/core/Core.hh"

namespace hyperion {

struct Addr {
    Str ip;
    u16 port;

    Str toString() const;
};

}  // namespace hyperion
