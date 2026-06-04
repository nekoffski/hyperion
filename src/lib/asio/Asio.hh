#pragma once

#include <asio.hpp>
#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>

namespace hyperion {

class AsioContextOwner {
   protected:
    asio::io_context& io() { return m_io; }

   private:
    asio::io_context m_io;
};

}  // namespace hyperion
