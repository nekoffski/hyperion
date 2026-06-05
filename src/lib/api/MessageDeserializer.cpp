#include "MessageDeserializer.hh"

#include "cmd/Error.hh"
#include "cmd/Health.hh"

namespace hyperion::api {

ApiMessageDeserializer::ApiMessageDeserializer() {
    registerMessage<HealthRequest>();
    registerMessage<HealthResponse>();
    registerMessage<ErrorResponse>();
}

}  // namespace hyperion::api
