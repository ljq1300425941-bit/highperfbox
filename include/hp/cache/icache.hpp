#pragma once
#include <cstddef>   // ← 加这个（保险）
#include "icache.hpp"
#include <unordered_map>
#include <list>

namespace hp::cache {

template<typename Key, typename Value>
class ICache {
public:
    virtual ~ICache() = default;

    // 命中返回 true，并写入 value
    virtual bool get(const Key& key, Value& value) = 0;

    // 插入或更新
    virtual void put(const Key& key, const Value& value) = 0;

    virtual size_t size() const = 0;
};

} // namespace hp::cache