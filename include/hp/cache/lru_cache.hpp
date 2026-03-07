#pragma once
#include <cstddef>
#include <utility>
#include <iterator>
#include <list>
#include <unordered_map>
#include "icache.hpp"

namespace hp::cache {

template<typename Key, typename Value>
class LRUCache : public ICache<Key, Value> {
public:
    explicit LRUCache(size_t capacity)
        : capacity_(capacity) {
        if (capacity_ > 0) {
            map_.reserve(capacity_);
        }
    }

    bool get(const Key& key, Value& value) override {
        auto it = map_.find(key);
        if (it == map_.end()) return false;

        items_.splice(items_.begin(), items_, it->second);
        value = it->second->second;
        return true;
    }

    void put(const Key& key, const Value& value) override {
        if (capacity_ == 0) return;

        auto it = map_.find(key);
        if (it != map_.end()) {
            it->second->second = value;
            items_.splice(items_.begin(), items_, it->second);
            return;
        }

        items_.emplace_front(key, value);
        map_.emplace(key, items_.begin());

        if (items_.size() > capacity_) {
            auto last = std::prev(items_.end());
            map_.erase(last->first);
            items_.pop_back();
        }
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