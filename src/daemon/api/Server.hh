#pragma once

#include "Controller.hh"
#include "lib/core/Config.hh"
#include "lib/net/Asio.hh"
#include "lib/net/TcpServer.hh"
#include "lib/runtime/Thread.hh"

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
    Controller m_apiController;
};

}  // namespace hyperion
