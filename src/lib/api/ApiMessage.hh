#pragma once

#include "lib/core/Concepts.hh"
#include "lib/core/Core.hh"
#include "lib/messaging/Message.hh"

namespace hyperion {

enum class ApiMessageKind {
    none = 0,
    healthRequest,
    healthResponse,
    error,
    count
};

using ApiMessage = Message<ApiMessageKind>;

template <typename T, ApiMessageKind Kind>
using ApiMessageImpl = MessageImpl<T, ApiMessageKind, Kind>;

}  // namespace hyperion
