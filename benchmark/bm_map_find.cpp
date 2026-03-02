#include <benchmark/benchmark.h>
#include <map>
#include <unordered_map>
#include <vector>

static void BM_MapFind(benchmark::State& state) {
    std::map<int, int> m;
    for (int i = 0; i < state.range(0); ++i) {
        m.emplace(i, i);
    }

    for (auto _ : state) {
        for (int i = 0; i < state.range(0); ++i) {
            benchmark::DoNotOptimize(m.find(i));
        }
    }
}

static void BM_UnorderedMapFind(benchmark::State& state) {
    std::unordered_map<int, int> m;
    m.reserve(state.range(0));
    for (int i = 0; i < state.range(0); ++i) {
        m.emplace(i, i);
    }

    for (auto _ : state) {
        for (int i = 0; i < state.range(0); ++i) {
            benchmark::DoNotOptimize(m.find(i));
        }
    }
}

BENCHMARK(BM_MapFind)->Arg(1000)->Arg(10000)->Arg(100000);
BENCHMARK(BM_UnorderedMapFind)->Arg(1000)->Arg(10000)->Arg(100000);

BENCHMARK_MAIN();