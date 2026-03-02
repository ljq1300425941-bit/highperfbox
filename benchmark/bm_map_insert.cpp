#include <benchmark/benchmark.h>
#include <map>
#include <unordered_map>
#include <random>

static void BM_MapInsert(benchmark::State& state) {
    for (auto _ : state) {
        std::map<int, int> m;
        for (int i = 0; i < state.range(0); ++i) {
            m.emplace(i, i);
        }
        benchmark::DoNotOptimize(m);
    }
}

static void BM_UnorderedMapInsert(benchmark::State& state) {
    for (auto _ : state) {
        std::unordered_map<int, int> m;
        //m.reserve(state.range(0)); // 很关键
        for (int i = 0; i < state.range(0); ++i) {
            m.emplace(i, i);
        }
        benchmark::DoNotOptimize(m);
    }
}

BENCHMARK(BM_MapInsert)->Arg(1000)->Arg(10000)->Arg(100000);
BENCHMARK(BM_UnorderedMapInsert)->Arg(1000)->Arg(10000)->Arg(100000);

BENCHMARK_MAIN();