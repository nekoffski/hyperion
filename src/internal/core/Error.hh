#pragma once

#include <fmt/core.h>

#include <stdexcept>
#include <string>

namespace hyperion {

enum class ErrorCode {
    none = 0,
    invalidArgument,
    badConfig,
    outOfRange,
    ioError,
    fileSystemError,
    fileNotFound,
    directoryNotFound,
    invalidEnumValue,
    duplicate,
    bufferOutOfScope,
    invalidMessagePreamble,
    invalidMessageSize,
    invalidMessageKind,
    childProcessFailed,
    deserializerNotFound,
    workspaceError,
    invalidConfiguration,
};

class Exception : public std::exception {
   public:
    template <typename... Args>
    explicit Exception(ErrorCode code, const std::string& fmt, Args&&... args)
        : m_code(code),
          m_message(
              fmt::format(fmt::runtime(fmt), std::forward<Args>(args)...)
          ) {}

    template <typename... Args>
    explicit Exception(const std::string& fmt, Args&&... args)
        : m_code(ErrorCode::none),
          m_message(
              fmt::format(fmt::runtime(fmt), std::forward<Args>(args)...)
          ) {}

    const std::string& message() const;
    ErrorCode code() const;
    const char* what() const noexcept override;

   private:
    ErrorCode m_code{ErrorCode::none};
    std::string m_message;
};

#define DEFINE_ERROR(name)                                                    \
    class name : public Exception {                                           \
       public:                                                                \
        template <typename... Args>                                           \
        explicit name(const std::string& fmt, Args&&... args)                 \
            : Exception(fmt, std::forward<Args>(args)...) {}                  \
        template <typename... Args>                                           \
        explicit name(ErrorCode code, const std::string& fmt, Args&&... args) \
            : Exception(code, fmt, std::forward<Args>(args)...) {}            \
    };

#define DEFINE_SUB_ERROR(name, base)                                          \
    class name : public base {                                                \
       public:                                                                \
        template <typename... Args>                                           \
        explicit name(const std::string& fmt, Args&&... args)                 \
            : base(fmt, std::forward<Args>(args)...) {}                       \
        template <typename... Args>                                           \
        explicit name(ErrorCode code, const std::string& fmt, Args&&... args) \
            : base(code, fmt, std::forward<Args>(args)...) {}                 \
    };

DEFINE_ERROR(RuntimeError);
DEFINE_ERROR(CoreError);
DEFINE_ERROR(NetError);
DEFINE_ERROR(ConfigError);
DEFINE_ERROR(MessagingError);
DEFINE_ERROR(WorkspaceError);
DEFINE_ERROR(KslError);

}  // namespace hyperion
