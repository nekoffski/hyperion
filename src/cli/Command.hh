#pragma once

#include <CLI/CLI.hpp>
#include <vector>

#include "lib/core/Concepts.hh"
#include "lib/core/Core.hh"
#include "lib/net/Asio.hh"

namespace hyperion {

class Command : public NonCopyable {
   public:
    using Callback = std::function<asio::awaitable<void>(CLI::App&)>;
    using InitCallback = std::function<void(CLI::App&)>;

   private:
    struct Sub {
        Callback callback;
        CLI::App* cmd;
    };

   public:
    explicit Command(
        CLI::App& app, const std::string& name, const std::string& description
    );

    void addSubcommand(
        const std::string& name, const std::string& description,
        Callback&& callback, std::optional<InitCallback> init = std::nullopt
    );

    virtual ~Command() = default;

    bool isChosen() const;
    asio::awaitable<void> execute();

   protected:
    CLI::App* m_cmd;
    std::vector<Sub> m_subs;
};

}  // namespace hyperion
