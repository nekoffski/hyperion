#pragma once

#include "internal/core/Concepts.hh"
#include "internal/core/Core.hh"
#include "internal/messaging/Message.hh"

namespace hyperion::api {

enum class ApiMessageKind {
    none = 0,
    ack,
    healthRequest,
    healthResponse,
    errorResponse,
    workspaceListRequest,
    workspaceListResponse,
    workspaceCreateRequest,
    workspaceDeleteRequest,
    jobCreateRequest,
    jobStopRequest,
    jobListRequest,
    jobListResponse,
    jobStatusRequest,
    jobStatusResponse,
    count
};

using ApiMessage = Message<ApiMessageKind>;

template <typename T, ApiMessageKind Kind>
using ApiMessageImpl = MessageImpl<T, ApiMessageKind, Kind>;

}  // namespace hyperion::api
