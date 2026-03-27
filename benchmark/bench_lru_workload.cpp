#include <benchmark/benchmark.h>
#include <cstdint>
#include <string>
#include <vector>

#include "hp/benchmark/bench_case.hpp"
#include "hp/cache/lru_cache.hpp"
#include "hp/workload/workload.hpp"

static hp::workload::WorkloadConfig ToWorkloadConfig(const hp::bench::BenchCase& bc) {
    hp::workload::WorkloadConfig cfg;
    cfg.key_space = static_cast<uint64_t>(bc.key_space);
    cfg.seed = 42;
    cfg.hot_key_ratio = bc.hot_key_ratio;
    cfg.hot_access_ratio = bc.hot_access_ratio;
    cfg.zipf_alpha = bc.zipf_alpha;

    switch (bc.workload) {
        case hp::bench::WorkloadKind::Uniform:
            cfg.type = hp::workload::WorkloadType::Uniform;
            break;
        case hp::bench::WorkloadKind::Hotspot:
            cfg.type = hp::workload::WorkloadType::Hotspot;
            break;
        case hp::bench::WorkloadKind::Zipf:
            cfg.type = hp::workload::WorkloadType::Zipf;
            break;  
        default:
            throw std::runtime_error("unknown workload kind");
    }

    return cfg;
}

static void BM_LRU_Workload_Case(benchmark::State& state, const hp::bench::BenchCase& bc) {
    std::vector<uint64_t> seq;
    seq.reserve(static_cast<size_t>(bc.total_ops));

    hp::workload::WorkloadConfig cfg = ToWorkloadConfig(bc);
    hp::workload::WorkloadGenerator gen(cfg);

    for (int i = 0; i < bc.total_ops; ++i) {
        seq.push_back(gen.next());
    }

    for (auto _ : state) {
        state.PauseTiming();

        hp::cache::LRUCache<uint64_t, uint64_t> cache(static_cast<size_t>(bc.capacity));
        int hits = 0;
        int misses = 0;
        uint64_t value = 0;

        state.ResumeTiming();

        for (uint64_t key : seq) {
            if (cache.get(key, value)) {
                ++hits;
            } else {
                ++misses;
                cache.put(key, key);
            }
        }

        benchmark::DoNotOptimize(hits);
        benchmark::DoNotOptimize(misses);

        state.counters["HitRate"] =
            benchmark::Counter(
                static_cast<double>(hits) / static_cast<double>(hits + misses));

        state.counters["MissRate"] =
            benchmark::Counter(
                static_cast<double>(misses) / static_cast<double>(hits + misses));

        state.counters["OpsPerSec"] =
            benchmark::Counter(
                static_cast<double>(bc.total_ops),
                benchmark::Counter::kIsIterationInvariantRate);
    }
}

int main(int argc, char** argv) {
    benchmark::MaybeReenterWithoutASLR(argc, argv);

    auto register_case = [](const std::string& prefix, const hp::bench::BenchCase& bc) {
        std::string name = prefix + hp::bench::CaseName(bc);
        benchmark::RegisterBenchmark(name.c_str(), BM_LRU_Workload_Case, bc);
    };

    for (const auto& bc : hp::bench::DefaultBenchCases()) {
        register_case("LRU/", bc);
    }

    for (const auto& bc : hp::bench::KeySpaceSweepCases()) {
        register_case("LRU_KeySpaceSweep/", bc);
    }

    for (const auto& bc : hp::bench::HotspotIntensitySweepCases()) {
        register_case("LRU_HotspotSweep/", bc);
    }

    for (const auto& bc : hp::bench::ZipfAlphaSweepCases()) {
        register_case("LRU_ZipfSweep/", bc);
    }

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
    return 0;
}