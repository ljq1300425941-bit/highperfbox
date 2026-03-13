# HighPerfBox

## English Summary
HighPerfBox is a C++ performance engineering project focused on cache behavior analysis, workload modeling, and reproducible benchmarking.
The current version includes a generic cache interface, a single-threaded O(1) LRU cache, workload generators, correctness tests, and experiments on hit rate, CPU time, and throughput under different cache capacities and access patterns.

---

## 项目简介

HighPerfBox 是一个面向 C++ 性能工程训练的实验型项目，当前聚焦缓存策略、访问负载建模与 benchmark 评测，用于研究不同 workload 和容量配置下的缓存行为。

项目目标不是堆砌更多缓存算法，而是训练面向 AI Infra / 高性能研发场景的性能分析、实验设计与数据解释能力，逐步形成可复现、可量化、可追问的系统实验项目。

目前已完成：

- 基础工程骨架（CMake / include / src / tests / benchmark）
- Logger 与 Config 模块
- Google Benchmark 接入
- 通用缓存接口 `ICache`
- 单线程 O(1) LRU Cache
- Workload 生成器（`Uniform` / `Hotspot-like`）
- 基础正确性测试
- LRU 命中率实验

---

## 核心设计

### LRU Cache
当前实现采用经典组合：

- `std::unordered_map<Key, list::iterator>`：O(1) 查找
- `std::list<std::pair<Key, Value>>`：O(1) 顺序更新与尾部淘汰

支持：

- O(1) `get`
- O(1) `put`
- O(1) 访问顺序更新
- O(1) 淘汰最久未使用元素

### Workload Generator
当前支持两类访问负载：

- `Uniform`：均匀随机访问
- `Hotspot-like`：热点访问（约 80% 请求落在前 20% key 空间）

支持固定 `seed` 保证实验可复现，支持配置 `key_space` 控制候选 key 范围。

---

## 实验结果

### LRU 在不同容量下的命中率表现

| Capacity | Uniform | Hotspot-like |
| -------- | ------- | ------------ |
| 50       | 4.83%   | 17.69%       |
| 100      | 9.63%   | 34.54%       |
| 200      | 19.65%  | 62.76%       |
| 500      | 48.27%  | 86.93%       |

### LRU 在不同容量下的吞吐与 CPU Time 表现

| Capacity | Workload     | HitRate | CPU Time (ns) | Throughput (ops/sec) |
| -------- | ------------ | ------- | ------------- | -------------------- |
| 50       | Uniform      | 5.01%   | 4839926       | 20.6615M/s           |
| 50       | Hotspot-like | 17.19%  | 4226491       | 23.6603M/s           |
| 100      | Uniform      | 10.01%  | 4736331       | 21.1134M/s           |
| 100      | Hotspot-like | 34.29%  | 3797354       | 26.3341M/s           |
| 200      | Uniform      | 20.12%  | 4291465       | 23.3021M/s           |
| 200      | Hotspot-like | 63.83%  | 2286072       | 43.7432M/s           |
| 500      | Uniform      | 49.83%  | 2805374       | 35.6459M/s           |
| 500      | Hotspot-like | 89.72%  | 1240770       | 80.5951M/s           |

### 结论

- 缓存容量增大时，LRU 命中率持续上升。
- Hotspot-like 负载下命中率显著高于 Uniform，说明 LRU 对时间局部性更强的 workload 更敏感。
- 随着命中率提升，CPU Time 下降、Throughput 上升，说明缓存效果改善会直接带来整体处理效率提升。
- 当缓存容量逐渐接近热点集合大小时，性能收益会更加明显。

---

## 工程结构

```text
highperfbox/
├── include/        # 公共头文件
├── src/            # 源码实现
├── benchmark/      # benchmark 程序
├── tests/          # 正确性测试
├── docs/           # 实验报告与文档
├── third_party/    # 第三方依赖
└── CMakeLists.txt
```

------

## 快速开始

### 1.构建项目

```
```bash
mkdir -p build
cd build
cmake ..
cmake --build . --config Release
```

### 2.运行测试

```
./test_lru_basic
./test_workload_basic
./test_lru_workload
```

### 3.运行benchmark

```
./bench_lru_workload
```



------

## 实验可复现性

为了保证 benchmark 结果具有可比性，当前实验固定了以下关键参数：

\- 固定随机种子 `seed`
\- 固定候选 key 范围 `key_space`
\- 固定访问序列长度 `total_ops`
\- 使用统一的缓存容量配置：50 / 100 / 200 / 500
\- 使用统一的 workload 类型：Uniform / Hotspot-like

这样可以减少随机输入波动对结果的影响，使不同容量与不同 workload 下的命中率、CPU Time 和吞吐结果具有可比性。

------



## 当前进度

当前项目已完成单线程 LRU 缓存实现、Workload Generator、基础正确性测试，以及命中率、CPU Time 与 Throughput 三类核心实验结果，已经具备“缓存策略 + 访问负载 + benchmark + 数据分析”的基础实验链路。

目前项目的重点不是继续横向扩展更多缓存策略，而是把现有结果进一步整理为可复现、可量化、可解释、可追问的性能实验项目。

------



## 下一步计划

接下来将重点补充以下内容：

\- 继续完善 `docs/week2_lru_workload_analysis.md` 的实验解释与结论表达
\- 打磨 1 分钟 / 3 分钟项目口述稿
\- 整理高频项目追问清单，如 workload 影响、benchmark 可复现性、LRU 设计权衡等
\- 统一 README、docs、简历中的项目表述口径