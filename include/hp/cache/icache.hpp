#pragma once
#include <cstddef>

namespace hp::cache {

template<typename Key, typename Value>
class ICache {
public:
    virtual ~ICache() = default;

    virtual bool get(const Key& key, Value& value) = 0;
    virtual void put(const Key& key, const Value& value) = 0;
    virtual size_t size() const = 0;
};

} // namespace hp::cache