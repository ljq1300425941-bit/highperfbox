#include"hp/core/app.hpp"
#include"hp/cache/lru_cache.hpp"
#include <cassert>
#include <cstdint>

int main() {
    hp::cache::LRUCache<uint64_t, uint64_t> c(2);
    uint64_t v = 0;

    c.put(1, 10);
    c.put(2, 20);

    assert(c.get(1, v) && v == 10); // 1 becomes MRU
    c.put(3, 30);                   // evict LRU: key=2

    assert(!c.get(2, v));
    assert(c.get(1, v) && v == 10);
    assert(c.get(3, v) && v == 30);
}