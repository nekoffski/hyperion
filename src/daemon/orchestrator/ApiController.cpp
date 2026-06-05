#include "ApiController.hh"

#include "lib/api/cmd/Error.hh"
#include "lib/api/cmd/Health.hh"

namespace hyperion {

ApiController::ApiController() {}

asio::awaitable<std::unique_ptr<api::ApiMessage>> ApiController::handleMessage(
    const api::ApiMessage& message
) {
    switch (message.kind()) {
        case api::ApiMessageKind::healthRequest:
            co_return co_await onHealthRequest(
                static_cast<const api::HealthRequest&>(message)
            );

        default:
            log::warn(
                "Received unhandled message of kind: {}",
                fmt::underlying(message.kind())
            );
            co_return std::make_unique<api::ErrorResponse>(fmt::format(
                "Unhandled message kind: {}", fmt::underlying(message.kind())
            ));
    }
}

asio::awaitable<std::unique_ptr<api::ApiMessage>>
ApiController::onHealthRequest(const api::HealthRequest& req) {
    log::debug("Received health request with uuid: {}", req.uuid());
    co_return std::make_unique<api::HealthResponse>(req.uuid());
}

}  // namespace hyperion
