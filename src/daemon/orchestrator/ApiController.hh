#pragma once

#include <memory>

#include "lib/api/Message.hh"
#include "lib/api/cmd/Health.hh"
#include "lib/asio/Asio.hh"
#include "lib/core/Concepts.hh"
#include "lib/core/Core.hh"

namespace hyperion {

class ApiController : public NonCopyable, public NonMovable {
   public:
    ApiController();

    asio::awaitable<std::unique_ptr<api::ApiMessage>> handleMessage(
        const api::ApiMessage& message
    );

   private:
    asio::awaitable<std::unique_ptr<api::ApiMessage>> onHealthRequest(
        const api::HealthRequest& req
    );
};
}  // namespace hyperion
