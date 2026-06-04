#pragma once

#include "ApiMessage.hh"
#include "lib/core/Singleton.hh"
#include "lib/messaging/MessageDeserializer.hh"

namespace hyperion {

class ApiMessageDeserializer : public Singleton<ApiMessageDeserializer>,
                               public MessageDeserializer<ApiMessageKind> {
   public:
    explicit ApiMessageDeserializer();
};

}  // namespace hyperion
