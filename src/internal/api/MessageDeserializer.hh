#pragma once

#include "Message.hh"
#include "internal/core/Singleton.hh"
#include "internal/messaging/MessageDeserializer.hh"

namespace hyperion::api {

class ApiMessageDeserializer
    : public Singleton<ApiMessageDeserializer>,
      public hyperion::MessageDeserializer<ApiMessageKind> {
   public:
    explicit ApiMessageDeserializer();
};

}  // namespace hyperion::api
