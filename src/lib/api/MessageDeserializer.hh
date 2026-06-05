#pragma once

#include "Message.hh"
#include "lib/core/Singleton.hh"
#include "lib/messaging/MessageDeserializer.hh"

namespace hyperion::api {

class ApiMessageDeserializer
    : public Singleton<ApiMessageDeserializer>,
      public hyperion::MessageDeserializer<ApiMessageKind> {
   public:
    explicit ApiMessageDeserializer();
};

}  // namespace hyperion::api
