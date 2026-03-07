# HighPerfBox

## English Summary
HighPerfBox is a C++ performance engineering project focused on cache behavior analysis, workload modeling, and reproducible benchmarking.  
The current version includes a generic cache interface, a single-threaded O(1) LRU cache, workload generators, correctness tests, and hit-rate experiments under different cache capacities.

---

## 项目简介

HighPerfBox 是一个面向 C++ 性能工程训练的实验型项目，当前聚焦缓存策略、访问负载建模与 benchmark 评测，用于研究不同 workload 和容量配置下的缓存行为。

目前已完成：

- 基础工程骨架（CMake / include / src / tests / benchmark）
- Logger 与 Config 模块
- Google Benchmark 接入
- 通用缓存接口 `ICache`
- 单线程 O(1) LRU Cache
- Workload 生成器（`Uniform` / `Hotspot`）
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
- `Hotspot`：热点访问（约 80% 请求落在前 20% key 空间）

支持固定 `seed` 保证实验可复现，支持配置 `key_space` 控制候选 key 范围。

---

## 实验结果

### LRU 在不同容量下的命中率表现

| Capacity | Uniform | Hotspot |
|----------|---------|---------|
| 50       | 4.83%   | 17.69%  |
| 100      | 9.63%   | 34.54%  |
| 200      | 19.65%  | 62.76%  |
| 500      | 48.27%  | 86.93%  |

### 结论
- 缓存容量增大时，LRU 命中率持续上升。
- Hotspot 负载下命中率显著高于 Uniform，说明 LRU 对时间局部性更强的 workload 更敏感。
- 当缓存容量逐渐接近热点集合大小时，命中率会出现更明显提升。

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