#pragma once

#include "lib/asio/Asio.hh"
#include "lib/core/Config.hh"
#include "lib/runtime/Thread.hh"

namespace hyperion {

class ApiServer : public Thread {
   public:
    explicit ApiServer(const Config& config);

    void cancel() override;

   private:
    void run() override;

    const Config& m_config;

    asio::io_context m_io;
    asio::ip::tcp::acceptor m_acceptor;
};

}  // namespace hyperion
