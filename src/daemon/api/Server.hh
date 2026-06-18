#pragma once

#include "Dispatcher.hh"
#include "internal/core/Config.hh"
#include "internal/net/Asio.hh"
#include "internal/net/TcpServer.hh"
#include "internal/runtime/Thread.hh"

namespace hyperion {

class Server : public AsioContextOwner, public Thread, public TcpServer {
   public:
    explicit Server(const Config& config);
    ~Server() override;

    void cancel() override;

   private:
    void run() override;
    asio::awaitable<void> onClient(TcpSession& session) override;

    const Config& m_config;
    Dispatcher m_apiDispatcher;
};

}  // namespace hyperion
