#include <iostream>
#include <stdexcept>
#include <string>
#include <cmath>
#include "hp/workload/workload.hpp"

namespace {

void check(bool cond, const std::string& msg) {
    if (!cond) {
        throw std::runtime_error(msg);
    }
}

// 1. 同 seed + 同参数 -> 序列应该一致
void test_same_seed_same_sequence() {
    hp::workload::WorkloadGenerator g1(
        hp::workload::WorkloadType::Uniform, 1000, 42);
    hp::workload::WorkloadGenerator g2(
        hp::workload::WorkloadType::Uniform, 1000, 42);

    for (int i = 0; i < 10000; ++i) {
        int x1 = g1.next();
        int x2 = g2.next();
        check(x1 == x2, "same seed should generate same sequence");
    }

    std::cout << "[PASS] test_same_seed_same_sequence\n";
}

// 2. 所有 key 都必须落在 [0, key_space) 里
void test_key_in_range_uniform() {
    const int key_space = 1000;
    hp::workload::WorkloadGenerator g(
        hp::workload::WorkloadType::Uniform, key_space, 42);

    for (int i = 0; i < 100000; ++i) {
        int x = g.next();
        check(x >= 0 && x < key_space, "uniform key out of range");
    }

    std::cout << "[PASS] test_key_in_range_uniform\n";
}

void test_key_in_range_hotspot() {
    const int key_space = 1000;
    hp::workload::WorkloadGenerator g(
        hp::workload::WorkloadType::Hotspot, key_space, 42);

    for (int i = 0; i < 100000; ++i) {
        int x = g.next();
        check(x >= 0 && x < key_space, "hotspot key out of range");
    }

    std::cout << "[PASS] test_key_in_range_hotspot\n";
}

// 3. Hotspot 模式下，大约 80% 请求落在前 20% key 空间
void test_hotspot_ratio() {
    const int key_space = 1000;
    const int hot_boundary = key_space * 0.2;   // 前20% key
    const int total = 100000;

    hp::workload::WorkloadGenerator g(
        hp::workload::WorkloadType::Hotspot, key_space, 42);

    int hot_count = 0;
    for (int i = 0; i < total; ++i) {
        int x = g.next();
        if (x < hot_boundary) {
            ++hot_count;
        }
    }

    double ratio = static_cast<double>(hot_count) / total;

    // 不要卡太死，留一点随机波动空间
    check(ratio > 0.75 && ratio < 0.85,
          "hotspot ratio is not in expected range, got: " + std::to_string(ratio));

    std::cout << "[PASS] test_hotspot_ratio, ratio = " << ratio << "\n";
}

// 可选：Uniform 模式做个粗 sanity check
void test_uniform_not_too_biased() {
    const int key_space = 1000;
    const int hot_boundary = key_space * 0.2;
    const int total = 100000;

    hp::workload::WorkloadGenerator g(
        hp::workload::WorkloadType::Uniform, key_space, 42);

    int hot_count = 0;
    for (int i = 0; i < total; ++i) {
        int x = g.next();
        if (x < hot_boundary) {
            ++hot_count;
        }
    }

    double ratio = static_cast<double>(hot_count) / total;

    // Uniform 下，落在前20%区间的比例应接近 20%
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
        test_hotspot_ratio();
        test_uniform_not_too_biased();

        std::cout << "\nAll workload tests passed.\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n[FAIL] " << e.what() << "\n";
        return 1;
    }
}