#include "hp/workload/workload.hpp"
#include <cassert>
#include <cstdint>
#include <iostream>

using hp::workload::WorkloadGenerator;
using hp::workload::WorkloadType;

int main() {
    {
        WorkloadGenerator gen(WorkloadType::Uniform, 100, 42);
        auto seq = gen.generate(1000);

        assert(seq.size() == 1000);
        for (auto x : seq) {
            assert(x < 100);
        }
    }

    {
        WorkloadGenerator gen(WorkloadType::Hotspot, 100, 42);
        auto seq = gen.generate(1000);

        assert(seq.size() == 1000);
        for (auto x : seq) {
            assert(x < 100);
        }
    }

    std::cout << "All workload basic tests passed.\n";
    return 0;
}