#include "hp/cache/lru_cache.hpp"
#include <cassert>
#include <cstdint>
#include <iostream>

using hp::cache::LRUCache;

int main() {
    {
        LRUCache<uint64_t, uint64_t> c(0);
        uint64_t v = 0;
        c.put(1, 10);
        assert(c.size() == 0);
        assert(!c.get(1, v));
    }

    {
        LRUCache<uint64_t, uint64_t> c(2);
        uint64_t v = 0;
        c.put(1, 10);
        c.put(2, 20);
        assert(c.get(1, v));
        assert(v == 10);
        assert(c.get(2, v));
        assert(v == 20);
    }

    {
        LRUCache<uint64_t, uint64_t> c(2);
        uint64_t v = 0;
        c.put(1, 10);
        c.put(1, 99);
        assert(c.size() == 1);
        assert(c.get(1, v));
        assert(v == 99);
    }

    {
        LRUCache<uint64_t, uint64_t> c(2);
        uint64_t v = 0;
        c.put(1, 10);
        c.put(2, 20);
        assert(c.get(1, v));
        c.put(3, 30);
        assert(!c.get(2, v));
        assert(c.get(1, v));
        assert(v == 10);
        assert(c.get(3, v));
        assert(v == 30);
    }

    {
        LRUCache<uint64_t, uint64_t> c(3);
        uint64_t v = 0;
        c.put(1, 10);
        c.put(2, 20);
        c.put(3, 30);
        c.put(4, 40);
        assert(c.size() == 3);
        assert(!c.get(1, v));
        assert(c.get(2, v));
        assert(c.get(3, v));
        assert(c.get(4, v));
    }

    std::cout << "All LRU basic tests passed.\n";
    return 0;
}