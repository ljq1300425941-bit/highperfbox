#include <benchmark/benchmark.h>
#include <vector>
#include "hp/workload/workload.hpp"
#include "hp/cache/lru_cache.hpp"
#include "hp/benchmark/bench_case.hpp"

static void BM_LRU_Workload_Case(benchmark::State& state, const hp::bench::BenchCase& bc) {
    std::vector<uint64_t> seq;;
    seq.reserve(bc.total_ops);

    hp::workload::WorkloadConfig cfg;
    cfg.type = (bc.workload == hp::bench::WorkloadKind::Uniform)
             ? hp::workload::WorkloadType::Uniform
             : hp::workload::WorkloadType::Hotspot;
    cfg.key_space = bc.key_space;
    cfg.seed = 42;
    cfg.hot_key_ratio = bc.hot_key_ratio;
    cfg.hot_access_ratio = bc.hot_access_ratio;

    hp::workload::WorkloadGenerator gen(cfg);

    for (int i = 0; i < bc.total_ops; ++i) {
        seq.push_back(gen.next());
    }

    for (auto _ : state) {
        state.PauseTiming();

        hp::cache::LRUCache<int, int> cache(bc.capacity);
        int hits = 0;
        int misses = 0;
        int value = 0;

        state.ResumeTiming();

        for (int key : seq) {
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

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
    return 0;
}