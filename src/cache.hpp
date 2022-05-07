#ifndef CACHE_HPP
#define CACHE_HPP

#include <cassert>
#include <mutex>
#include <optional>
#include <vector>

template <typename KeyType, typename ValueType>
class Cache {
   public:
    [[nodiscard]] Cache(const std::size_t capacity) : m_capacity(capacity) {
    }

    [[nodiscard]] auto get(const KeyType key) -> std::optional<ValueType> {
        std::lock_guard lock(m_mutex);

        for (auto iter = m_store.begin(); iter != m_store.end(); ++iter) {
            if (iter->first == key) {
                auto obj = iter->second;
                m_store.erase(iter);
                return obj;
            }
        }

        return {};
    }

    auto push(const KeyType key, ValueType value) -> void {
        assert(value);

        if (m_capacity == 0) {
            return;
        }

        std::lock_guard lock(m_mutex);

        while (m_store.size() >= m_capacity) {
            m_store.erase(m_store.begin());
        }

        m_store.emplace_back(key, value);
    }

   private:
    std::mutex m_mutex;
    std::size_t m_capacity = 0;
    std::vector<std::pair<KeyType, ValueType>> m_store;
};

#endif
