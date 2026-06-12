#include "Payload.hh"

namespace hyperion {

PayloadBufferView PayloadWriter::getBuffer() { return m_buffer; }

PayloadReader::PayloadReader(PayloadBufferView view) : m_buffer(view) {}

u8* PayloadReader::readImpl(u8 bytes) {
    if (m_buffer.size() < m_index + bytes) {
        throw PayloadError{
            ErrorCode::bufferOutOfScope,
            "PayloadReader buffer out of scope: {} < {}",
            m_buffer.size(),
            m_index + bytes,
        };
    }

    ON_SCOPE_EXIT { m_index += bytes; };
    return m_buffer.data() + m_index;
}

}  // namespace hyperion
