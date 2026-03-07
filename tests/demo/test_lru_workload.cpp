#include "hp/cache/lru_cache.hpp"
#include "hp/workload/workload.hpp"
#include <cassert>
#include <cstdint>
#include <iostream>

struct CacheStats {
    uint64_t requests = 0;
    uint64_t hits = 0;
    uint64_t misses = 0;

    double hit_rate() const {
        if (requests == 0) return 0.0;
        return static_cast<double>(hits) / static_cast<double>(requests);
    }
};

CacheStats run_lru_experiment(
    hp::workload::WorkloadType type,
    uint64_t key_space,
    size_t sequence_len,
    size_t cache_capacity,
    uint64_t seed = 42) {

    hp::workload::WorkloadGenerator gen(type, key_space, seed);
    auto seq = gen.generate(sequence_len);

    hp::cache::LRUCache<uint64_t, uint64_t> cache(cache_capacity);

    CacheStats stats;
    uint64_t value = 0;

    for (auto key : seq) {
        stats.requests++;
        if (cache.get(key, value)) {
            stats.hits++;
        } else {
            stats.misses++;
            cache.put(key, key);
        }
    }

    return stats;
}


int main() {
    auto uniform_stats = run_lru_experiment(
        hp::workload::WorkloadType::Uniform,
        1000,
        10000,
        100
    );

    auto hotspot_stats = run_lru_experiment(
        hp::workload::WorkloadType::Hotspot,
        1000,
        10000,
        100
    );

    std::cout << "Uniform hit rate: " << uniform_stats.hit_rate() << "\n";
    std::cout << "Hotspot hit rate: " << hotspot_stats.hit_rate() << "\n";

    assert(uniform_stats.requests == 10000);
    assert(hotspot_stats.requests == 10000);
    assert(uniform_stats.hits + uniform_stats.misses == uniform_stats.requests);
    assert(hotspot_stats.hits + hotspot_stats.misses == hotspot_stats.requests);

    return 0;
}