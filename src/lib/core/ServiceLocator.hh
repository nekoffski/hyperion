#pragma once

#include <shared_mutex>

#include "Concepts.hh"
#include "Log.hh"

namespace hyperion {

template <typename T>
class ServiceLocator : public StaticClass {
   public:
    static T& get() {
        std::shared_lock lk{s_mutex};
        log::expect(
            s_instance, "Service {} is not registered", typeid(T).name()
        );
        return *s_instance;
    }

    static T* find() { return s_instance; }

    static void set(T* instance) {
        std::unique_lock lk{s_mutex};
        log::expect(
            instance, "Cannot set service {} to nullptr", typeid(T).name()
        );
        s_instance = instance;
    }

    static void clear() { s_instance = nullptr; }

   private:
    inline static T* s_instance{nullptr};
    inline static std::shared_mutex s_mutex;
};

}  // namespace hyperion
