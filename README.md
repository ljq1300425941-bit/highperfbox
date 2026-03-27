# HighPerfBox

一个基于 **C++** 的缓存与性能实验项目，聚焦 **LRU Cache 实现、访问负载建模、benchmark 评测与结果分析**。

项目当前主要围绕以下问题展开：

- 如何实现一个时间复杂度为 **O(1)** 的 LRU Cache
- 不同访问分布下，LRU 的命中率和吞吐表现会有什么差异
- cache capacity、key space、访问偏斜程度会如何影响缓存收益

本项目当前已完成的重点是：**在可控、可复现的 synthetic workload 下，对 LRU 的行为进行系统化实验分析**。  
后续计划在此基础上继续扩展更贴近 AI serving 场景的轻量抽象。

---

## 项目亮点

- 基于 `unordered_map + doubly linked list` 实现 **O(1) LRU Cache**
- 支持三类访问负载建模：
  - `Uniform`
  - `Hotspot-like`
  - `Zipf-like`
- 基于 **Google Benchmark** 进行性能评测
- 支持多种参数扫描：
  - capacity sweep
  - key space sweep
  - hotspot intensity sweep
  - Zipf alpha sweep
- 输出 `HitRate / OpsPerSec / CPU Time` 等指标
- 提供实验结果整理与文档化分析，便于复现与后续扩展

---

## 当前已完成

- `ICache` 抽象接口
- 单线程 O(1) LRU Cache 实现
- Workload 生成器：
  - `Uniform`
  - `Hotspot-like`
  - `Zipf-like`
- benchmark：
  - Default Cases
  - Key Space Sweep
  - Hotspot Intensity Sweep
  - Zipf Alpha Sweep
- 基础正确性测试
- 实验结果整理与分析文档

---

## 实验结果概览

本项目围绕 `LRU` 缓存，在三类访问负载下进行了性能评测：

- `Uniform`
- `Hotspot-like`
- `Zipf-like`

实验结果表明，`LRU` 对访问局部性和分布偏斜程度非常敏感。

在相同 `key_space=1000` 条件下，不同 workload 的表现差异明显：

- `Uniform`：访问较均匀，复用机会少，命中率较低
- `Hotspot-like`：大量访问集中在热点区，命中率明显提升
- `Zipf-like`：头部 key 更集中，命中率和吞吐最高

例如，在 `capacity=200`、`key_space=1000` 时：

- `Uniform`：`20.12%` hit rate，`23.64M/s`
- `Hotspot-like`：`58.16%` hit rate，`38.86M/s`
- `Zipf-like (alpha=1.2)`：`84.20%` hit rate，`60.41M/s`

进一步的参数扫描还表明：

- cache capacity 增大时，命中率持续上升
- key space 增大时，固定容量下缓存效果明显下降
- 热点越集中，或 Zipf 的 `alpha` 越大，LRU 收益越明显

这些结果说明：

> **缓存效果不仅取决于容量大小，还高度依赖 workload 本身的局部性与偏斜结构。**

完整实验结果与分析见 [`docs/evaluation.md`](docs/evaluation.md)。

---

## 代表性实验结果

### Default Cases

| Benchmark                                      | CPU Time   | HitRate | OpsPerSec |
| ---------------------------------------------- | ---------- | ------- | --------- |
| cap_50_ks_1000_ops_100000_Uniform              | 4679340 ns | 5.01%   | 21.37M/s  |
| cap_50_ks_1000_ops_100000_Hotspot_hk_20_ha_80  | 4318969 ns | 15.71%  | 23.15M/s  |
| cap_50_ks_1000_ops_100000_Zipf_za_120          | 2582357 ns | 66.63%  | 38.72M/s  |
| cap_100_ks_1000_ops_100000_Uniform             | 4608385 ns | 10.01%  | 21.70M/s  |
| cap_100_ks_1000_ops_100000_Hotspot_hk_20_ha_80 | 3851635 ns | 31.28%  | 25.96M/s  |
| cap_100_ks_1000_ops_100000_Zipf_za_120         | 2401815 ns | 75.87%  | 41.64M/s  |
| cap_200_ks_1000_ops_100000_Uniform             | 4229598 ns | 20.12%  | 23.64M/s  |
| cap_200_ks_1000_ops_100000_Hotspot_hk_20_ha_80 | 2573227 ns | 58.16%  | 38.86M/s  |
| cap_200_ks_1000_ops_100000_Zipf_za_120         | 1655428 ns | 84.20%  | 60.41M/s  |
| cap_500_ks_1000_ops_100000_Uniform             | 2859559 ns | 49.83%  | 34.97M/s  |
| cap_500_ks_1000_ops_100000_Hotspot_hk_20_ha_80 | 1373661 ns | 87.26%  | 72.80M/s  |
| cap_500_ks_1000_ops_100000_Zipf_za_120         | 1117680 ns | 93.61%  | 89.47M/s  |

> 其余参数扫描（Key Space Sweep / Hotspot Intensity Sweep / Zipf Alpha Sweep）已整理到 [`docs/evaluation.md`](docs/evaluation.md)。

---

## 核心设计

### 1. LRU Cache

当前 LRU 实现采用：

- `unordered_map<Key, list iterator>`：用于 O(1) 定位节点
- `std::list<std::pair<Key, Value>>`：用于维护访问顺序

核心思路：

- 命中时：将节点移动到链表头部
- 插入时：写入头部
- 容量超限时：淘汰链表尾部（最久未使用元素）

因此，`get / put / erase` 的关键操作都能在平均意义下维持 **O(1)**。

---

### 2. Workload Generator

当前支持三类访问分布：

#### Uniform

所有 key 被均匀访问，局部性最弱。

#### Hotspot-like

通过以下参数控制热点访问模式：

- `hot_key_ratio`：前多少比例的 key 作为热点区
- `hot_access_ratio`：多少比例的访问落在热点区

该模型用于模拟“有热点，但热点区内部近似均匀”的访问模式。

#### Zipf-like

通过 `zipf_alpha` 控制长尾分布的偏斜程度：

- `alpha` 越大，头部 key 越集中
- `alpha` 越小，分布越平缓

该模型用于模拟更强的头部集中访问行为。

---

### 3. Benchmark 设计

当前 benchmark 的设计目标是：尽量只测缓存访问本身，而不是把无关开销混进去。

主要做法：

- 先预生成完整访问序列
- cache 初始化放在计时区外
- benchmark 计时区只执行缓存访问

这样做可以减少以下噪音：

- workload generator 自身开销
- cache 对象初始化开销
- benchmark case 之间的配置差异带来的额外影响

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
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
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

### 4.导入结果

```
./build/bench_lru_workload \
  --benchmark_format=json \
  --benchmark_out=results/bench_lru_workload.json
```

### 5.将benchmark JSON 转成 Markdown

```
python3 tools/bench_json_to_md.py \
  results/bench_lru_workload.json \
  -o results/bench_lru_workload.md \
  --title "LRU Benchmark Results" \
  --group-by-prefix \
  --simplify-name
```



------

## 实验可复现性

当前实验尽量保证结果可复现，主要通过以下方式控制变量：

- 固定随机种子
- 固定 `total_ops`
- 固定 benchmark case 参数
- 对每组 case 固定 workload 类型及其参数：
  - `Uniform`
  - `Hotspot-like`
  - `Zipf-like`
- 将访问序列生成放在计时区外
- 将 cache 初始化放在计时区外

因此，当前实验结果更适合用于：

- 不同 workload 之间的相对比较
- 参数变化趋势分析
- 缓存行为与局部性关系的验证

------

## 详细实验分析

更完整的实验结果与分析见：

- [`docs/evaluation.md`](docs/evaluation.md)

其中包含：

- Default Cases
- Key Space Sweep
- Hotspot Intensity Sweep
- Zipf Alpha Sweep
- 结果分析与阶段性结论
- 当前实验局限性说明



## 当前结论

基于当前实验，可以得到以下阶段性结论：

- `LRU` 对 workload 局部性和偏斜程度非常敏感
- `Uniform` 下命中率较低，主要受 `capacity / key_space` 比例影响
- `Hotspot-like` 下，热点访问会显著提升缓存收益
- `Zipf-like` 下，由于头部 key 更集中，LRU 命中率和吞吐通常更高
- 增大 cache capacity、减小 key space、增强访问偏斜，都会提升缓存效果

这说明：

> **缓存策略的效果不只取决于算法本身，也高度依赖 workload 的结构。**

------

## 局限性

当前项目仍然是一个简化实验框架，主要限制包括：

- 当前只实现了**单线程** LRU
- 当前只比较了一种缓存策略
- workload 为**人工构造的 synthetic workload**
- 当前 benchmark 聚焦缓存行为本身，还不是完整在线服务或 AI serving runtime 实验

不过，这也正是本项目当前阶段的价值所在：

- 先把缓存实现、workload 建模、benchmark 与结果分析做扎实
- 再在这个基础上逐步扩展更贴近真实系统场景的抽象





## 下一步计划

后续准备在当前缓存实验框架上继续推进以下方向：

- 补充更贴近在线服务场景的 workload 建模
- 引入轻量级 serving 抽象指标，如 `prefill/decode` 成本估计
- 扩展更贴近共享上下文场景的 workload
- 完善结果输出与实验自动化脚本
- 继续提升文档完整性与面试表达一致性