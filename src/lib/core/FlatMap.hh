#pragma once

#include <algorithm>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

namespace hyperion {

template <typename K, typename V>
class FlatMap {
   public:
    using value_type = std::pair<K, V>;
    using iterator = typename std::vector<value_type>::iterator;
    using const_iterator = typename std::vector<value_type>::const_iterator;

    FlatMap() = default;

    V& operator[](const K& key) {
        if (auto it = find(key); it != end()) {
            return it->second;
        }
        return m_data.emplace_back(key, V{}).second;
    }

    V& at(const K& key) {
        if (auto it = find(key); it != end()) {
            return it->second;
        }
        throw std::out_of_range("FlatMap::at: key not found");
    }

    const V& at(const K& key) const {
        if (auto it = find(key); it != end()) {
            return it->second;
        }
        throw std::out_of_range("FlatMap::at: key not found");
    }

    std::optional<std::reference_wrapper<V>> get(const K& key) {
        if (auto it = find(key); it != end()) {
            return it->second;
        }
        return {};
    }

    std::optional<std::reference_wrapper<const V>> get(const K& key) const {
        if (auto it = find(key); it != end()) {
            return it->second;
        }
        return {};
    }

    bool insert(K key, V value) {
        if (contains(key)) {
            return false;
        }
        m_data.emplace_back(std::move(key), std::move(value));
        return true;
    }

    bool insertOrAssign(K key, V value) {
        if (auto it = find(key); it != end()) {
            it->second = std::move(value);
            return false;
        }
        m_data.emplace_back(std::move(key), std::move(value));
        return true;
    }

    bool erase(const K& key) {
        auto it = find(key);
        if (it == end()) {
            return false;
        }
        m_data.erase(it);
        return true;
    }

    bool contains(const K& key) const { return find(key) != end(); }

    std::size_t size() const { return m_data.size(); }
    bool empty() const { return m_data.empty(); }
    void clear() { m_data.clear(); }
    void reserve(std::size_t n) { m_data.reserve(n); }

    iterator begin() { return m_data.begin(); }
    iterator end() { return m_data.end(); }
    const_iterator begin() const { return m_data.begin(); }
    const_iterator end() const { return m_data.end(); }
    const_iterator cbegin() const { return m_data.cbegin(); }
    const_iterator cend() const { return m_data.cend(); }

   private:
    iterator find(const K& key) {
        return std::ranges::find_if(m_data, [&](const value_type& p) {
            return p.first == key;
        });
    }

    const_iterator find(const K& key) const {
        return std::ranges::find_if(m_data, [&](const value_type& p) {
            return p.first == key;
        });
    }

    std::vector<value_type> m_data;
};

}  // namespace hyperion
