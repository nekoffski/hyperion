#pragma once

#include "lib/api/ApiMessage.hh"

namespace hyperion {

class HealthRequest
    : public ApiMessageImpl<HealthRequest, ApiMessageKind::healthRequest> {
   public:
    explicit HealthRequest();

    void serialize(PayloadWriter& writer) const override;
    void deserialize(PayloadReader& reader) override;
};

class HealthResponse
    : public ApiMessageImpl<HealthResponse, ApiMessageKind::healthResponse> {
   public:
    explicit HealthResponse();

    void serialize(PayloadWriter& writer) const override;
    void deserialize(PayloadReader& reader) override;
};

}  // namespace hyperion
