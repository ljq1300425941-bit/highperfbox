#pragma once
#include <string>
#include <vector>

namespace hp::bench {

enum class WorkloadKind {
    Uniform = 0,
    Hotspot = 1,
};

struct BenchCase {
    int capacity;
    int key_space;
    int total_ops;
    WorkloadKind workload;
    double hot_key_ratio{0.2};
    double hot_access_ratio{0.8};
};

inline const char* ToString(WorkloadKind w) {
    return w == WorkloadKind::Uniform ? "Uniform" : "Hotspot";
}

inline std::string CaseName(const BenchCase& c) {
    std::string name =
        "cap_" + std::to_string(c.capacity) +
        "_ks_" + std::to_string(c.key_space) +
        "_ops_" + std::to_string(c.total_ops) +
        "_" + ToString(c.workload);

    if (c.workload == WorkloadKind::Hotspot) {
        name += "_hk_" + std::to_string(static_cast<int>(c.hot_key_ratio * 100));
        name += "_ha_" + std::to_string(static_cast<int>(c.hot_access_ratio * 100));
    }
    return name;
}

inline std::vector<BenchCase> DefaultBenchCases() {
    return {
        {50, 1000, 100000, WorkloadKind::Uniform},
        {50, 1000, 100000, WorkloadKind::Hotspot, 0.2, 0.8},
        {100, 1000, 100000, WorkloadKind::Uniform},
        {100, 1000, 100000, WorkloadKind::Hotspot, 0.2, 0.8},
        {200, 1000, 100000, WorkloadKind::Uniform},
        {200, 1000, 100000, WorkloadKind::Hotspot, 0.2, 0.8},
        {500, 1000, 100000, WorkloadKind::Uniform},
        {500, 1000, 100000, WorkloadKind::Hotspot, 0.2, 0.8},
    };
}

inline std::vector<BenchCase> KeySpaceSweepCases() {
    return {
        {200, 500,  100000, WorkloadKind::Uniform},
        {200, 500,  100000, WorkloadKind::Hotspot, 0.2, 0.8},
        {200, 1000, 100000, WorkloadKind::Uniform},
        {200, 1000, 100000, WorkloadKind::Hotspot, 0.2, 0.8},
        {200, 5000, 100000, WorkloadKind::Uniform},
        {200, 5000, 100000, WorkloadKind::Hotspot, 0.2, 0.8},
    };
}

inline std::vector<BenchCase> HotspotIntensitySweepCases() {
    return {
        {200, 1000, 100000, WorkloadKind::Hotspot, 0.3, 0.7},
        {200, 1000, 100000, WorkloadKind::Hotspot, 0.2, 0.8},
        {200, 1000, 100000, WorkloadKind::Hotspot, 0.1, 0.9},
    };
}

} // namespace hp::bench