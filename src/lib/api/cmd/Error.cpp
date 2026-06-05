#include "Error.hh"

namespace hyperion::api {

ErrorResponse::ErrorResponse(const Exception& e)
    : ApiMessageImpl<ErrorResponse, ApiMessageKind::errorResponse>(
          "ErrorResponse"
      ) {
    m_details = e.what();
    m_code = e.code();
}

ErrorResponse::ErrorResponse()
    : ApiMessageImpl<ErrorResponse, ApiMessageKind::errorResponse>(
          "ErrorResponse"
      ) {}

ErrorResponse::ErrorResponse(const std::string& details, ErrorCode code)
    : ApiMessageImpl<ErrorResponse, ApiMessageKind::errorResponse>(
          "ErrorResponse"
      ),
      m_details(details),
      m_code(code) {}

const std::string& ErrorResponse::details() const { return m_details; }

std::string& ErrorResponse::details() { return m_details; }

ErrorCode ErrorResponse::code() const { return m_code; }

ErrorCode& ErrorResponse::code() { return m_code; }

void ErrorResponse::serializeImpl(PayloadWriter& writer) const {
    writer.write(m_details);
    writer.write(static_cast<u16>(m_code));
}

void ErrorResponse::deserializeImpl(PayloadReader& reader) {
    reader.read(m_details);
    m_code = static_cast<ErrorCode>(reader.read<u16>());
}

}  // namespace hyperion::api
