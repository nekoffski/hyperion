#include "ApiMessageDeserializer.hh"

#include "cmd/Health.hh"

namespace hyperion {

ApiMessageDeserializer::ApiMessageDeserializer() {
    registerMessage<HealthRequest>();
    registerMessage<HealthResponse>();
}

}  // namespace hyperion
