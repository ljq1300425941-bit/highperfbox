#include <benchmark/benchmark.h>
#include <vector>

static void BM_VectorPush(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<int> v;
        for (int i = 0; i < state.range(0); ++i) {
            v.push_back(i);
        }
    }
}

BENCHMARK(BM_VectorPush)->Arg(1000)->Arg(10000)->Arg(100000);

BENCHMARK_MAIN();