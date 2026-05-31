#pragma once

#include <string>
#include <typeindex>

#include "Concepts.hh"

namespace hyperion {

struct RTTI : public virtual NonCopyable, public virtual NonMovable {
    virtual std::type_index getType() const = 0;

    template <typename T>
    bool is() const {
        return getType() == typeid(T);
    }
    template <typename T>
    T* as() {
        return static_cast<T*>(this);
    }
};

}  // namespace hyperion