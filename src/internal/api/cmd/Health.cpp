#include "Health.hh"

#include "internal/core/Random.hh"

namespace hyperion::api {

HealthResponse::HealthResponse()
    : ApiMessageImpl<HealthResponse, ApiMessageKind::healthResponse>(
          "HealthResponse"
      ) {}

HealthResponse::HealthResponse(const std::string& uuid)
    : ApiMessageImpl<HealthResponse, ApiMessageKind::healthResponse>(
          "HealthResponse"
      ),
      m_uuid(uuid) {}

const std::string& HealthResponse::uuid() const { return m_uuid; }

std::string& HealthResponse::uuid() { return m_uuid; }

void HealthRequest::serializeImpl(PayloadWriter& writer) const {
    writer.write(m_uuid);
}

void HealthRequest::deserializeImpl(PayloadReader& reader) {
    reader.read(m_uuid);
}

const std::string& HealthRequest::uuid() const { return m_uuid; }

std::string& HealthRequest::uuid() { return m_uuid; }

HealthRequest::HealthRequest()
    : ApiMessageImpl<HealthRequest, ApiMessageKind::healthRequest>(
          "HealthRequest"
      ),
      m_uuid(RandomEngine::get().uuid()) {}

void HealthResponse::serializeImpl(PayloadWriter& writer) const {
    writer.write(m_uuid);
}

void HealthResponse::deserializeImpl(PayloadReader& reader) {
    reader.read(m_uuid);
}

}  // namespace hyperion::api
