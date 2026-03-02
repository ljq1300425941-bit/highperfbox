#include <benchmark/benchmark.h>
#include <map>
#include <unordered_map>
#include <random>

static void BM_UnorderedMap_reserve(benchmark::State& state) {
    for (auto _ : state) {
        std::unordered_map<int, int> m;
        m.reserve(state.range(0)); // 很关键
        for (int i = 0; i < state.range(0); ++i) {
            m.emplace(i, i);
        }
        benchmark::DoNotOptimize(m);
    }
}

static void BM_UnorderedMap_noreserve(benchmark::State& state) {
    for (auto _ : state) {
        std::unordered_map<int, int> m;
        //m.reserve(state.range(0)); // 很关键
        for (int i = 0; i < state.range(0); ++i) {
            m.emplace(i, i);
        }
        benchmark::DoNotOptimize(m);
    }
}

BENCHMARK(BM_UnorderedMap_reserve)->Arg(1000)->Arg(10000)->Arg(100000);
BENCHMARK(BM_UnorderedMap_noreserve)->Arg(1000)->Arg(10000)->Arg(100000);

BENCHMARK_MAIN();