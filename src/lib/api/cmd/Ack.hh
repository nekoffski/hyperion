#pragma once

#include "lib/api/Message.hh"

namespace hyperion::api {

class Ack : public ApiMessageImpl<Ack, ApiMessageKind::ack> {
   public:
    Ack();

   private:
    void serializeImpl(PayloadWriter& writer) const final override;
    void deserializeImpl(PayloadReader& reader) final override;
};

}  // namespace hyperion::api
