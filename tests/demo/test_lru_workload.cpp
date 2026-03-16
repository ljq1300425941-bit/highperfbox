#include "hp/cache/lru_cache.hpp"
#include "hp/workload/workload.hpp"
#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>

struct CacheStats {
    uint64_t requests = 0;
    uint64_t hits = 0;
    uint64_t misses = 0;

    double hit_rate() const {
        if (requests == 0) return 0.0;
        return static_cast<double>(hits) / static_cast<double>(requests);
    }
};

static hp::workload::WorkloadConfig make_uniform_cfg(uint64_t key_space, uint64_t seed = 42) {
    hp::workload::WorkloadConfig cfg;
    cfg.type = hp::workload::WorkloadType::Uniform;
    cfg.key_space = key_space;
    cfg.seed = seed;
    return cfg;
}

static hp::workload::WorkloadConfig make_hotspot_cfg(
    uint64_t key_space,
    uint64_t seed = 42,
    double hot_key_ratio = 0.2,
    double hot_access_ratio = 0.8) {
    hp::workload::WorkloadConfig cfg;
    cfg.type = hp::workload::WorkloadType::Hotspot;
    cfg.key_space = key_space;
    cfg.seed = seed;
    cfg.hot_key_ratio = hot_key_ratio;
    cfg.hot_access_ratio = hot_access_ratio;
    return cfg;
}

CacheStats run_lru_experiment(
    const hp::workload::WorkloadConfig& cfg,
    size_t sequence_len,
    size_t cache_capacity) {

    hp::workload::WorkloadGenerator gen(cfg);
    auto seq = gen.generate(sequence_len);

    hp::cache::LRUCache<uint64_t, uint64_t> cache(cache_capacity);

    CacheStats stats;
    uint64_t value = 0;

    for (auto key : seq) {
        ++stats.requests;
        if (cache.get(key, value)) {
            ++stats.hits;
        } else {
            ++stats.misses;
            cache.put(key, key);
        }
    }

    return stats;
}

int main() {
    const uint64_t key_space = 1000;
    const size_t sequence_len = 10000;
    const std::vector<size_t> capacities = {50, 100, 200, 500};

    auto uniform_cfg = make_uniform_cfg(key_space, 42);
    auto hotspot_cfg = make_hotspot_cfg(key_space, 42, 0.2, 0.8);

    for (auto cap : capacities) {
        auto uniform_stats = run_lru_experiment(
            uniform_cfg,
            sequence_len,
            cap
        );

        auto hotspot_stats = run_lru_experiment(
            hotspot_cfg,
            sequence_len,
            cap
        );

        std::cout << "Capacity = " << cap << "\n";
        std::cout << "  Uniform hit rate: " << uniform_stats.hit_rate() << "\n";
        std::cout << "  Hotspot hit rate: " << hotspot_stats.hit_rate() << "\n";

        assert(uniform_stats.requests == sequence_len);
        assert(hotspot_stats.requests == sequence_len);
        assert(uniform_stats.hits + uniform_stats.misses == uniform_stats.requests);
        assert(hotspot_stats.hits + hotspot_stats.misses == hotspot_stats.requests);
    }

    std::cout << "All LRU workload tests passed.\n";
    return 0;
}