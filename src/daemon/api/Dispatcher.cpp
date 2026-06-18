#include "Dispatcher.hh"

#include "internal/api/cmd/Error.hh"
#include "internal/core/ServiceLocator.hh"

namespace hyperion {

Dispatcher::Dispatcher()
    : m_workspaceController(ServiceLocator<WorkspaceManager>::get()) {}

asio::awaitable<std::unique_ptr<api::ApiMessage>> Dispatcher::dispatch(
    const api::ApiMessage& msg
) {
    try {
        co_return co_await dispatchImpl(msg);
    } catch (const Exception& e) {
        log::error("Error handling message: {}", e.what());
        co_return std::make_unique<api::ErrorResponse>(e);
    }
}

#define CASE(Type, controller) \
    case Type::kindValue:      \
        co_return co_await controller.on(msg.as<Type>());

asio::awaitable<std::unique_ptr<api::ApiMessage>> Dispatcher::dispatchImpl(
    const api::ApiMessage& msg
) {
    switch (msg.kind()) {
        CASE(api::HealthRequest, (*this));
        CASE(api::WorkspaceCreateRequest, m_workspaceController);
        CASE(api::WorkspaceDeleteRequest, m_workspaceController);
        CASE(api::WorkspaceListRequest, m_workspaceController);

        default:
            break;
    }
    log::warn(
        "Received unhandled message of kind: {}", fmt::underlying(msg.kind())
    );
    co_return std::make_unique<api::ErrorResponse>(
        fmt::format("Unhandled message kind: {}", fmt::underlying(msg.kind()))
    );
}

asio::awaitable<std::unique_ptr<api::ApiMessage>> Dispatcher::on(
    const api::HealthRequest& req
) {
    log::debug("Received health request with uuid: {}", req.uuid());
    co_return std::make_unique<api::HealthResponse>(req.uuid());
}

}  // namespace hyperion
