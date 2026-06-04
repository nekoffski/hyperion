#include "Health.hh"

namespace hyperion {

HealthResponse::HealthResponse()
    : ApiMessageImpl<HealthResponse, ApiMessageKind::healthResponse>(
          "HealthResponse"
      ) {}

void HealthRequest::serialize(PayloadWriter& writer) const {}

void HealthRequest::deserialize(PayloadReader& reader) {}

HealthRequest::HealthRequest()
    : ApiMessageImpl<HealthRequest, ApiMessageKind::healthRequest>(
          "HealthRequest"
      ) {}

void HealthResponse::serialize(PayloadWriter& writer) const {}

void HealthResponse::deserialize(PayloadReader& reader) {}

}  // namespace hyperion
