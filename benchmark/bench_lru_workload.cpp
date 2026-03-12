#include <benchmark/benchmark.h>
#include <vector>
#include "hp/workload/workload.hpp"
#include "hp/cache/lru_cache.hpp"

static void BM_LRU_Workload(benchmark::State& state) {
    const int capacity = state.range(0);
    const int key_space = state.range(1);
    const int total_ops = state.range(2);
    const int workload_type = state.range(3); // 0: Uniform, 1: Hotspot-like

    // 计时外：生成访问序列
    std::vector<int> seq;
    seq.reserve(total_ops);
    hp::workload::WorkloadGenerator gen(
        workload_type == 0 ? hp::workload::WorkloadType::Uniform : hp::workload::WorkloadType::Hotspot,
        key_space,
        /*seed=*/42
    );
    for (int i = 0; i < total_ops; ++i) {
        seq.push_back(gen.next());
    }

    for (auto _ : state) {
        state.PauseTiming();

        hp::cache::LRUCache<int, int> cache(capacity);
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
                static_cast<double>(total_ops),
                benchmark::Counter::kIsIterationInvariantRate);
    }
}

BENCHMARK(BM_LRU_Workload)
    ->Args({50, 1000, 100000, 0})
    ->Args({50, 1000, 100000, 1})
    ->Args({100, 1000, 100000, 0})
    ->Args({100, 1000, 100000, 1})
    ->Args({200, 1000, 100000, 0})
    ->Args({200, 1000, 100000, 1})
    ->Args({500, 1000, 100000, 0})
    ->Args({500, 1000, 100000, 1});

BENCHMARK_MAIN();