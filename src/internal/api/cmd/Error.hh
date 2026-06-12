#pragma once

#include "internal/api/Message.hh"
#include "internal/core/Error.hh"

namespace hyperion::api {

class ErrorResponse
    : public ApiMessageImpl<ErrorResponse, ApiMessageKind::errorResponse> {
   public:
    explicit ErrorResponse(const Exception& e);
    explicit ErrorResponse();
    explicit ErrorResponse(
        const std::string& details, ErrorCode code = ErrorCode::none
    );

    const std::string& details() const;
    std::string& details();

    ErrorCode code() const;
    ErrorCode& code();

   private:
    void serializeImpl(PayloadWriter& writer) const final override;
    void deserializeImpl(PayloadReader& reader) final override;

    std::string m_details;
    ErrorCode m_code{ErrorCode::none};
};

}  // namespace hyperion::api
