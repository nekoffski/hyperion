#pragma once

#include "lib/api/Message.hh"

namespace hyperion::api {

class HealthRequest
    : public ApiMessageImpl<HealthRequest, ApiMessageKind::healthRequest> {
   public:
    explicit HealthRequest();

    void serializeImpl(PayloadWriter& writer) const override;
    void deserializeImpl(PayloadReader& reader) override;

    const std::string& uuid() const;
    std::string& uuid();

   private:
    std::string m_uuid;
};

class HealthResponse
    : public ApiMessageImpl<HealthResponse, ApiMessageKind::healthResponse> {
   public:
    explicit HealthResponse();
    explicit HealthResponse(const std::string& uuid);

    const std::string& uuid() const;
    std::string& uuid();

    void serializeImpl(PayloadWriter& writer) const override;
    void deserializeImpl(PayloadReader& reader) override;

   private:
    std::string m_uuid;
};

}  // namespace hyperion::api
