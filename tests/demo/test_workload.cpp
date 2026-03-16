#include <iostream>
#include <stdexcept>
#include <string>
#include "hp/workload/workload.hpp"

namespace {

void check(bool cond, const std::string& msg) {
    if (!cond) {
        throw std::runtime_error(msg);
    }
}

hp::workload::WorkloadConfig make_uniform_cfg(uint64_t key_space, uint64_t seed = 42) {
    hp::workload::WorkloadConfig cfg;
    cfg.type = hp::workload::WorkloadType::Uniform;
    cfg.key_space = key_space;
    cfg.seed = seed;
    return cfg;
}

hp::workload::WorkloadConfig make_hotspot_cfg(uint64_t key_space,
                                              uint64_t seed,
                                              double hot_key_ratio,
                                              double hot_access_ratio) {
    hp::workload::WorkloadConfig cfg;
    cfg.type = hp::workload::WorkloadType::Hotspot;
    cfg.key_space = key_space;
    cfg.seed = seed;
    cfg.hot_key_ratio = hot_key_ratio;
    cfg.hot_access_ratio = hot_access_ratio;
    return cfg;
}

// 1. 同 seed + 同参数 -> 序列应该一致
void test_same_seed_same_sequence() {
    auto cfg = make_uniform_cfg(1000, 42);

    hp::workload::WorkloadGenerator g1(cfg);
    hp::workload::WorkloadGenerator g2(cfg);

    for (int i = 0; i < 10000; ++i) {
        uint64_t x1 = g1.next();
        uint64_t x2 = g2.next();
        check(x1 == x2, "same seed should generate same sequence");
    }

    std::cout << "[PASS] test_same_seed_same_sequence\n";
}

// 2. 所有 key 都必须落在 [0, key_space) 里
void test_key_in_range_uniform() {
    const uint64_t key_space = 1000;
    auto cfg = make_uniform_cfg(key_space, 42);

    hp::workload::WorkloadGenerator g(cfg);

    for (int i = 0; i < 100000; ++i) {
        uint64_t x = g.next();
        check(x < key_space, "uniform key out of range");
    }

    std::cout << "[PASS] test_key_in_range_uniform\n";
}

void test_key_in_range_hotspot() {
    const uint64_t key_space = 1000;
    auto cfg = make_hotspot_cfg(key_space, 42, 0.2, 0.8);

    hp::workload::WorkloadGenerator g(cfg);

    for (int i = 0; i < 100000; ++i) {
        uint64_t x = g.next();
        check(x < key_space, "hotspot key out of range");
    }

    std::cout << "[PASS] test_key_in_range_hotspot\n";
}

// 3. Hotspot 80/20: 约 80% 请求落在前 20% key 空间
void test_hotspot_ratio_80_20() {
    const uint64_t key_space = 1000;
    const uint64_t hot_boundary = static_cast<uint64_t>(key_space * 0.2);
    const int total = 100000;

    auto cfg = make_hotspot_cfg(key_space, 42, 0.2, 0.8);
    hp::workload::WorkloadGenerator g(cfg);

    int hot_count = 0;
    for (int i = 0; i < total; ++i) {
        uint64_t x = g.next();
        if (x < hot_boundary) {
            ++hot_count;
        }
    }

    double ratio = static_cast<double>(hot_count) / total;

    // 现在冷区不再走全空间，比例应更接近 0.80
    check(ratio > 0.78 && ratio < 0.82,
          "hotspot 80/20 ratio is not in expected range, got: " + std::to_string(ratio));

    std::cout << "[PASS] test_hotspot_ratio_80_20, ratio = " << ratio << "\n";
}

// 4. Hotspot 90/10: 约 90% 请求落在前 10% key 空间
void test_hotspot_ratio_90_10() {
    const uint64_t key_space = 1000;
    const uint64_t hot_boundary = static_cast<uint64_t>(key_space * 0.1);
    const int total = 100000;

    auto cfg = make_hotspot_cfg(key_space, 42, 0.1, 0.9);
    hp::workload::WorkloadGenerator g(cfg);

    int hot_count = 0;
    for (int i = 0; i < total; ++i) {
        uint64_t x = g.next();
        if (x < hot_boundary) {
            ++hot_count;
        }
    }

    double ratio = static_cast<double>(hot_count) / total;

    check(ratio > 0.88 && ratio < 0.92,
          "hotspot 90/10 ratio is not in expected range, got: " + std::to_string(ratio));

    std::cout << "[PASS] test_hotspot_ratio_90_10, ratio = " << ratio << "\n";
}

// 5. Uniform 模式做个粗 sanity check
void test_uniform_not_too_biased() {
    const uint64_t key_space = 1000;
    const uint64_t hot_boundary = static_cast<uint64_t>(key_space * 0.2);
    const int total = 100000;

    auto cfg = make_uniform_cfg(key_space, 42);
    hp::workload::WorkloadGenerator g(cfg);

    int hot_count = 0;
    for (int i = 0; i < total; ++i) {
        uint64_t x = g.next();
        if (x < hot_boundary) {
            ++hot_count;
        }
    }

    double ratio = static_cast<double>(hot_count) / total;

    check(ratio > 0.17 && ratio < 0.23,
          "uniform ratio is too biased, got: " + std::to_string(ratio));

    std::cout << "[PASS] test_uniform_not_too_biased, ratio = " << ratio << "\n";
}

} // namespace

int main() {
    try {
        test_same_seed_same_sequence();
        test_key_in_range_uniform();
        test_key_in_range_hotspot();
        test_hotspot_ratio_80_20();
        test_hotspot_ratio_90_10();
        test_uniform_not_too_biased();

        std::cout << "\nAll workload tests passed.\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n[FAIL] " << e.what() << "\n";
        return 1;
    }
}