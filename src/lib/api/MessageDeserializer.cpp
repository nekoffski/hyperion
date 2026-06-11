#include "MessageDeserializer.hh"

#include "cmd/Ack.hh"
#include "cmd/Error.hh"
#include "cmd/Health.hh"
#include "cmd/Job.hh"
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
    registerMessage<JobCreateRequest>();
    registerMessage<JobStopRequest>();
    registerMessage<JobListRequest>();
    registerMessage<JobListResponse>();
    registerMessage<JobStatusRequest>();
    registerMessage<JobStatusResponse>();
}

}  // namespace hyperion::api
