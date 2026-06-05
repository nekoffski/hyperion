#pragma once

#include "lib/api/Session.hh"
#include "lib/api/cmd/Health.hh"
#include "lib/asio/Asio.hh"
#include "lib/core/Concepts.hh"
#include "lib/core/Config.hh"
#include "lib/core/Core.hh"
#include "lib/core/Singleton.hh"

namespace hyperion {

class DaemonClient : public UniqueInstance<DaemonClient> {
   public:
    explicit DaemonClient(asio::io_context& io, const Config& config);

    asio::awaitable<Opt<Str>> checkHealth();

   private:
    asio::awaitable<std::unique_ptr<api::ApiMessage>> sendMessage(
        const api::ApiMessage& message
    );

    const Config& m_config;
    TcpSession m_session;
};

}  // namespace hyperion
