#pragma once
#include "icache.hpp"
#include <unordered_map>
#include <list>

namespace hp::cache {

template<typename Key, typename Value>
class LRUCache : public ICache<Key, Value> {
public:
    explicit LRUCache(size_t capacity)
        : capacity_(capacity) {}

    bool get(const Key&, Value&) override {
        return false;
    }

    void put(const Key&, const Value&) override {
        // empty for Day1
    }

    size_t size() const override {
        return items_.size();
    }

private:
    size_t capacity_;

    std::list<std::pair<Key, Value>> items_;
    std::unordered_map<
        Key,
        typename std::list<std::pair<Key, Value>>::iterator
    > map_;
};

} // namespace hp::cache