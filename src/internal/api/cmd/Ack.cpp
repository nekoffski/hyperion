#include "Ack.hh"

namespace hyperion::api {

Ack::Ack() : ApiMessageImpl<Ack, ApiMessageKind::ack>("Ack") {}

void Ack::serializeImpl(PayloadWriter&) const {}

void Ack::deserializeImpl(PayloadReader&) {}

}  // namespace hyperion::api
