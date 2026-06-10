#include "MessageDeserializer.hh"

#include "cmd/Ack.hh"
#include "cmd/Error.hh"
#include "cmd/Health.hh"
#include "cmd/Workspace.hh"

namespace hyperion::api {

ApiMessageDeserializer::ApiMessageDeserializer() {
    registerMessage<HealthRequest>();
    registerMessage<HealthResponse>();
    registerMessage<ErrorResponse>();
    registerMessage<WorkspaceCreateRequest>();
    registerMessage<WorkspaceDeleteRequest>();
    registerMessage<WorkspaceListRequest>();
    registerMessage<Ack>();
    registerMessage<WorkspaceListResponse>();
}

}  // namespace hyperion::api
