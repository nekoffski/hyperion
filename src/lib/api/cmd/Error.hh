#pragma once

#include "lib/api/Message.hh"
#include "lib/core/Error.hh"

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

    void serializeImpl(PayloadWriter& writer) const override;
    void deserializeImpl(PayloadReader& reader) override;

   private:
    std::string m_details;
    ErrorCode m_code{ErrorCode::none};
};

}  // namespace hyperion::api
