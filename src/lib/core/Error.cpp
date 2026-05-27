#include "Error.hh"

namespace hyp {

const std::string& Exception::message() const {
    return m_message;
}

ErrorCode Exception::code() const {
    return m_code;
}

const char* Exception::what() const noexcept {
    return m_message.c_str();
}

}  // namespace hyp
