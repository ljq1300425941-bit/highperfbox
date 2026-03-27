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
    Hotspot,
    Zipf
};

struct WorkloadConfig {
    WorkloadType type{WorkloadType::Uniform};
    uint64_t key_space{1000};
    uint64_t seed{42};

    // Hotspot 参数
    double hot_key_ratio{0.2};      // 前多少比例的 key 作为热点区
    double hot_access_ratio{0.8};   // 多少比例的请求落在热点区

    //Zipf 参数
    double zipf_alpha{1.2};
};

class WorkloadGenerator {
public:
    explicit WorkloadGenerator(const WorkloadConfig& cfg)
        : cfg_(cfg),
          rng_(cfg.seed),
          full_dist_(0, 0),
          hot_dist_(0, 0),
          cold_dist_(0, 0),
          choose_hot_dist_(0.5) {
        validate_config();

        full_dist_ = std::uniform_int_distribution<uint64_t>(0, cfg_.key_space - 1);

        hot_key_count_ = std::max<uint64_t>(
            1, static_cast<uint64_t>(cfg_.key_space * cfg_.hot_key_ratio));
        hot_key_count_ = std::min<uint64_t>(hot_key_count_, cfg_.key_space);

        hot_dist_ = std::uniform_int_distribution<uint64_t>(0, hot_key_count_ - 1);

        has_cold_region_ = (hot_key_count_ < cfg_.key_space);
        if (has_cold_region_) {
            cold_dist_ = std::uniform_int_distribution<uint64_t>(
                hot_key_count_, cfg_.key_space - 1);
        }

        choose_hot_dist_ = std::bernoulli_distribution(cfg_.hot_access_ratio);

        build_zipf_distribution();
    }

    uint64_t next() {
        switch (cfg_.type) {
            case WorkloadType::Uniform:
                return next_uniform();
            case WorkloadType::Hotspot:
                return next_hotspot();
            case WorkloadType::Zipf:
                return next_zipf();
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

void validate_config() const {
        if (cfg_.key_space == 0) {
            throw std::invalid_argument("key_space must be > 0");
        }

        if (cfg_.hot_key_ratio <= 0.0 || cfg_.hot_key_ratio > 1.0) {
            throw std::invalid_argument("hot_key_ratio must be in (0, 1]");
        }

        if (cfg_.hot_access_ratio < 0.0 || cfg_.hot_access_ratio > 1.0) {
            throw std::invalid_argument("hot_access_ratio must be in [0, 1]");
        }

        if (cfg_.zipf_alpha <= 0.0) {
            throw std::invalid_argument("zipf_alpha must be > 0");
        }
    }

    void build_zipf_distribution() {
        zipf_weights_.clear();
        zipf_weights_.reserve(static_cast<size_t>(cfg_.key_space));

        for (uint64_t i = 0; i < cfg_.key_space; ++i) {
            // rank 从 1 开始，避免除零
            double weight = 1.0 / std::pow(static_cast<double>(i + 1), cfg_.zipf_alpha);
            zipf_weights_.push_back(weight);
        }

        zipf_dist_ = std::discrete_distribution<uint64_t>(
            zipf_weights_.begin(), zipf_weights_.end());
    }

    uint64_t next_uniform() {
        return full_dist_(rng_);
    }

    uint64_t next_hotspot() {
        if (choose_hot_dist_(rng_)) {
            return hot_dist_(rng_);
        }

        if (!has_cold_region_) {
            return hot_dist_(rng_);
        }

        return cold_dist_(rng_);
    }

    uint64_t next_zipf() {
        return zipf_dist_(rng_);
    }

private:
    WorkloadConfig cfg_;
    uint64_t hot_key_count_{1};
    bool has_cold_region_{false};

   std::mt19937_64 rng_;
    std::uniform_int_distribution<uint64_t> full_dist_;
    std::uniform_int_distribution<uint64_t> hot_dist_;
    std::uniform_int_distribution<uint64_t> cold_dist_;
    std::bernoulli_distribution choose_hot_dist_;

    std::vector<double> zipf_weights_;
    std::discrete_distribution<uint64_t> zipf_dist_;
};

} // namespace hp::workload