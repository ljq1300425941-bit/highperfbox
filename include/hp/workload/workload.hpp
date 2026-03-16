#pragma once
#include <cstddef>
#include <cstdint>
#include <random>
#include <vector>
#include <stdexcept>
#include <algorithm>

namespace hp::workload {

enum class WorkloadType {
    Uniform,
    Hotspot
};

struct WorkloadConfig {
    WorkloadType type{WorkloadType::Uniform};
    uint64_t key_space{1000};
    uint64_t seed{42};

    // Hotspot 参数
    double hot_key_ratio{0.2};      // 前多少比例的 key 作为热点区
    double hot_access_ratio{0.8};   // 多少比例的请求落在热点区
};

class WorkloadGenerator {
public:
    explicit WorkloadGenerator(const WorkloadConfig& cfg)
        : cfg_(cfg),
          rng_(cfg.seed),
          full_dist_(0, cfg.key_space - 1) {
        if (cfg_.key_space == 0) {
            throw std::invalid_argument("key_space must be > 0");
        }

        if (cfg_.hot_key_ratio <= 0.0 || cfg_.hot_key_ratio > 1.0) {
            throw std::invalid_argument("hot_key_ratio must be in (0, 1]");
        }

        if (cfg_.hot_access_ratio < 0.0 || cfg_.hot_access_ratio > 1.0) {
            throw std::invalid_argument("hot_access_ratio must be in [0, 1]");
        }

        hot_key_count_ = std::max<uint64_t>(
            1, static_cast<uint64_t>(cfg_.key_space * cfg_.hot_key_ratio));

        if (hot_key_count_ > cfg_.key_space) {
            hot_key_count_ = cfg_.key_space;
        }

        hot_dist_ = std::uniform_int_distribution<uint64_t>(0, hot_key_count_ - 1);

        // 冷区是 [hot_key_count_, key_space)
        has_cold_region_ = (hot_key_count_ < cfg_.key_space);
        if (has_cold_region_) {
            cold_dist_ = std::uniform_int_distribution<uint64_t>(
                hot_key_count_, cfg_.key_space - 1);
        }
    }

    uint64_t next() {
        switch (cfg_.type) {
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
        return full_dist_(rng_);
    }

    uint64_t next_hotspot() {
        std::bernoulli_distribution choose_hot(cfg_.hot_access_ratio);

        if (choose_hot(rng_)) {
            return hot_dist_(rng_);
        }

        // 没有冷区时只能落在热点区
        if (!has_cold_region_) {
            return hot_dist_(rng_);
        }

        return cold_dist_(rng_);
    }

private:
    WorkloadConfig cfg_;
    uint64_t hot_key_count_{1};
    bool has_cold_region_{false};

    std::mt19937_64 rng_;
    std::uniform_int_distribution<uint64_t> full_dist_;
    std::uniform_int_distribution<uint64_t> hot_dist_;
    std::uniform_int_distribution<uint64_t> cold_dist_;
};

} // namespace hp::workload