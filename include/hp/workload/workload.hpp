#pragma once
#include <cstddef>
#include <cstdint>
#include <random>
#include <vector>
#include <stdexcept>

namespace hp::workload {

enum class WorkloadType {
    Uniform,
    Hotspot
};

class WorkloadGenerator {
public:
    WorkloadGenerator(WorkloadType type,
                      uint64_t key_space,
                      uint64_t seed = 42)
        : type_(type),
          key_space_(key_space),
          rng_(seed),
          dist_(0, key_space - 1) {
        if (key_space_ == 0) {
            throw std::invalid_argument("key_space must be > 0");
        }
    }

    uint64_t next() {
        switch (type_) {
            case WorkloadType::Uniform:
                return next_uniform();
            case WorkloadType::Hotspot:
                return next_hotspot();
            default:
                throw std::runtime_error("unknown workload type");
        }
    }

    std::vector<uint64_t> generate(size_t n) {
        std::vector<uint64_t> seq;
        seq.reserve(n);
        for (size_t i = 0; i < n; ++i) {
            seq.push_back(next());
        }
        return seq;
    }

private:
    uint64_t next_uniform() {
        return dist_(rng_);
    }

    uint64_t next_hotspot() {
        // 80% 访问落在前 20% 热点区 
        std::uniform_int_distribution<int> coin(1, 100);
        uint64_t hot_range = key_space_ / 5;
        if (hot_range == 0) hot_range = 1;

        if (coin(rng_) <= 80) {
            std::uniform_int_distribution<uint64_t> hot_dist(0, hot_range - 1);
            return hot_dist(rng_);
        } else {
            return dist_(rng_);
        }
    }

private:
    WorkloadType type_;
    uint64_t key_space_;
    std::mt19937_64 rng_;
    std::uniform_int_distribution<uint64_t> dist_;
};

} // namespace hp::workload