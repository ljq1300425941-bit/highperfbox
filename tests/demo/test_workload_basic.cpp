#include "hp/workload/workload.hpp"
#include <cassert>
#include <cstdint>
#include <iostream>

using hp::workload::WorkloadConfig;
using hp::workload::WorkloadGenerator;
using hp::workload::WorkloadType;

static WorkloadConfig make_uniform_cfg(uint64_t key_space, uint64_t seed = 42) {
    WorkloadConfig cfg;
    cfg.type = WorkloadType::Uniform;
    cfg.key_space = key_space;
    cfg.seed = seed;
    return cfg;
}

static WorkloadConfig make_hotspot_cfg(
    uint64_t key_space,
    uint64_t seed = 42,
    double hot_key_ratio = 0.2,
    double hot_access_ratio = 0.8) {
    WorkloadConfig cfg;
    cfg.type = WorkloadType::Hotspot;
    cfg.key_space = key_space;
    cfg.seed = seed;
    cfg.hot_key_ratio = hot_key_ratio;
    cfg.hot_access_ratio = hot_access_ratio;
    return cfg;
}

int main() {
    {
        auto cfg = make_uniform_cfg(100, 42);
        WorkloadGenerator gen(cfg);
        auto seq = gen.generate(1000);

        assert(seq.size() == 1000);
        for (auto x : seq) {
            assert(x < 100);
        }
    }

    {
        auto cfg = make_hotspot_cfg(100, 42, 0.2, 0.8);
        WorkloadGenerator gen(cfg);
        auto seq = gen.generate(1000);

        assert(seq.size() == 1000);
        for (auto x : seq) {
            assert(x < 100);
        }
    }

    std::cout << "All workload basic tests passed.\n";
    return 0;
}