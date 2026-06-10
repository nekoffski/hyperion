#pragma once

#include "lib/api/Message.hh"

namespace hyperion::api {

class HealthRequest
    : public ApiMessageImpl<HealthRequest, ApiMessageKind::healthRequest> {
   public:
    explicit HealthRequest();

    const std::string& uuid() const;
    std::string& uuid();

   private:
    void serializeImpl(PayloadWriter& writer) const final override;
    void deserializeImpl(PayloadReader& reader) final override;

    std::string m_uuid;
};

class HealthResponse
    : public ApiMessageImpl<HealthResponse, ApiMessageKind::healthResponse> {
   public:
    explicit HealthResponse();
    explicit HealthResponse(const std::string& uuid);

    const std::string& uuid() const;
    std::string& uuid();

   private:
    void serializeImpl(PayloadWriter& writer) const final override;
    void deserializeImpl(PayloadReader& reader) final override;

    std::string m_uuid;
};

}  // namespace hyperion::api
