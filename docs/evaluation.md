# 实验评测

## 实验目标

本项目旨在评估一个基于 `unordered_map + list` 实现的 O(1) LRU Cache 在不同访问分布下的性能表现，并分析以下因素对缓存收益的影响：

- workload 类型
- 访问局部性强弱
- 分布偏斜程度
- cache capacity
- key space 大小

当前实验主要覆盖三类访问负载：

- `Uniform`
- `Hotspot-like`
- `Zipf-like`

实验目标不仅是给出命中率数据，还希望回答一个更重要的问题：

> **LRU 在什么场景下表现更好，以及为什么会这样。**

---

## 实验设置

所有实验都基于同一个 LRU 实现，并使用统一的 benchmark 框架进行评测。

### 基本配置

- Cache policy：`LRU`
- 每组实验总操作数：`100000`
- `key_space`：按实验配置变化
- `capacity`：按实验配置变化
- 随机种子：固定
- 指标：
  - `HitRate`
  - `OpsPerSec`
  - `CPU Time`

### benchmark 方法说明

为了尽量减少无关开销对结果的干扰，benchmark 采用了如下方式：

1. **先生成完整访问序列，再进入计时区**
2. **缓存对象初始化不计入 benchmark 时间**
3. 计时区只关注缓存访问本身的执行代价

因此，当前结果更关注：

- workload 对缓存命中率的影响
- hit/miss 路径差异对吞吐的影响

而不是 workload generator 自身的性能。

---

## 实验结果

# LRU Benchmark Results

## Default Cases

| Benchmark | CPU Time | HitRate | OpsPerSec |
|-----------|----------|---------|-----------|
| cap_50_ks_1000_ops_100000_Uniform | 4679340 ns | 5.01% | 21.37M/s |
| cap_50_ks_1000_ops_100000_Hotspot_hk_20_ha_80 | 4318969 ns | 15.71% | 23.15M/s |
| cap_50_ks_1000_ops_100000_Zipf_za_120 | 2582357 ns | 66.63% | 38.72M/s |
| cap_100_ks_1000_ops_100000_Uniform | 4608385 ns | 10.01% | 21.70M/s |
| cap_100_ks_1000_ops_100000_Hotspot_hk_20_ha_80 | 3851635 ns | 31.28% | 25.96M/s |
| cap_100_ks_1000_ops_100000_Zipf_za_120 | 2401815 ns | 75.87% | 41.64M/s |
| cap_200_ks_1000_ops_100000_Uniform | 4229598 ns | 20.12% | 23.64M/s |
| cap_200_ks_1000_ops_100000_Hotspot_hk_20_ha_80 | 2573227 ns | 58.16% | 38.86M/s |
| cap_200_ks_1000_ops_100000_Zipf_za_120 | 1655428 ns | 84.20% | 60.41M/s |
| cap_500_ks_1000_ops_100000_Uniform | 2859559 ns | 49.83% | 34.97M/s |
| cap_500_ks_1000_ops_100000_Hotspot_hk_20_ha_80 | 1373661 ns | 87.26% | 72.80M/s |
| cap_500_ks_1000_ops_100000_Zipf_za_120 | 1117680 ns | 93.61% | 89.47M/s |

## Key Space Sweep

| Benchmark | CPU Time | HitRate | OpsPerSec |
|-----------|----------|---------|-----------|
| cap_200_ks_500_ops_100000_Uniform | 3280132 ns | 40.06% | 30.49M/s |
| cap_200_ks_500_ops_100000_Hotspot_hk_20_ha_80 | 1603003 ns | 84.26% | 62.38M/s |
| cap_200_ks_500_ops_100000_Zipf_za_120 | 1322623 ns | 89.85% | 75.61M/s |
| cap_200_ks_1000_ops_100000_Uniform | 4074311 ns | 20.12% | 24.54M/s |
| cap_200_ks_1000_ops_100000_Hotspot_hk_20_ha_80 | 2552485 ns | 58.16% | 39.18M/s |
| cap_200_ks_1000_ops_100000_Zipf_za_120 | 1699189 ns | 84.20% | 58.85M/s |
| cap_200_ks_5000_ops_100000_Uniform | 4913291 ns | 4.11% | 20.35M/s |
| cap_200_ks_5000_ops_100000_Hotspot_hk_20_ha_80 | 4494530 ns | 12.87% | 22.25M/s |
| cap_200_ks_5000_ops_100000_Zipf_za_120 | 2277122 ns | 75.28% | 43.92M/s |

## Hotspot Intensity Sweep

| Benchmark | CPU Time | HitRate | OpsPerSec |
|-----------|----------|---------|-----------|
| cap_200_ks_1000_ops_100000_Hotspot_hk_30_ha_70 | 4090549 ns | 33.39% | 24.45M/s |
| cap_200_ks_1000_ops_100000_Hotspot_hk_20_ha_80 | 2724016 ns | 58.16% | 36.71M/s |
| cap_200_ks_1000_ops_100000_Hotspot_hk_10_ha_90 | 1352597 ns | 91.12% | 73.93M/s |

## Zipf Alpha Sweep

| Benchmark | CPU Time | HitRate | OpsPerSec |
|-----------|----------|---------|-----------|
| cap_200_ks_1000_ops_100000_Zipf_za_80 | 3228737 ns | 52.38% | 30.97M/s |
| cap_200_ks_1000_ops_100000_Zipf_za_100 | 2314090 ns | 69.69% | 43.21M/s |
| cap_200_ks_1000_ops_100000_Zipf_za_120 | 1738571 ns | 84.20% | 57.52M/s |
| cap_200_ks_1000_ops_100000_Zipf_za_150 | 1093609 ns | 95.42% | 91.44M/s |

---

## 结果分析

### 1. 不同 workload 下的缓存收益差异明显

在固定 `key_space=1000` 的情况下，随着缓存容量从 `50` 提升到 `500`，三类 workload 的命中率都在上升，但增长幅度差异非常明显：

- `Uniform`：`5.01% -> 49.83%`
- `Hotspot-like`：`15.71% -> 87.26%`
- `Zipf-like (alpha=1.2)`：`66.63% -> 93.61%`

这说明 **LRU 对访问局部性和分布偏斜程度非常敏感**。

在 `Uniform` 分布下，访问较为分散，每个 key 被再次访问的机会相对较低，因此缓存复用空间有限，命中率主要受 `capacity / key_space` 比例影响。

而在 `Hotspot-like` 和 `Zipf-like` 分布下，访问会集中到更小的一部分 key 上，使得 LRU 更容易保留高频访问对象，从而显著提高命中率。

---

### 2. 在当前参数下，Zipf-like 明显优于 Hotspot-like

以 `capacity=200`、`key_space=1000` 为例：

- `Uniform`：`20.12%`
- `Hotspot-like`：`58.16%`
- `Zipf-like`：`84.20%`

这组结果说明：在当前设置下，`Zipf-like` 的缓存收益远高于 `Hotspot-like`。

原因在于两者的“偏斜方式”不同：

- `Hotspot-like` 是一种**两段式模型**：大量访问落在热点区，但热点区内部依然近似均匀；
- `Zipf-like` 是一种**长尾分布**：头部极少数 key 获得了更高频访问，后部 key 的访问概率迅速衰减。

这意味着，在 `Zipf-like` 中，最热的一小部分 key 更容易长期留在缓存中，因此 LRU 的收益更高。

换句话说，不只是“有没有热点”重要，**热点是否进一步集中**同样会显著影响缓存命中率。

---

### 3. key space 扩大后，Uniform 退化最快，Zipf-like 抗性最强

在固定 `capacity=200` 下，当 `key_space` 从 `500` 增大到 `1000` 再到 `5000` 时：

- `Uniform`：`40.06% -> 20.12% -> 4.11%`
- `Hotspot-like`：`84.26% -> 58.16% -> 12.87%`
- `Zipf-like`：`89.85% -> 84.20% -> 75.28%`

这说明，当潜在 key 空间变大时，固定大小的缓存会显得“相对更小”，因此缓存效果整体会下降。

但三类 workload 的退化速度并不一样：

- `Uniform` 下降最快，因为访问本来就缺乏明显局部性；
- `Hotspot-like` 也会退化，因为热点区规模会随总 key 空间一起扩大；
- `Zipf-like` 退化最慢，因为即使总 key 数变大，大部分访问仍然高度集中在头部 key 上。

这个现象说明：**决定缓存收益的，不只是 key 空间总大小，而是访问是否持续集中在少量高频对象上。**

---

### 4. 偏斜程度增强时，LRU 命中率持续上升

#### Hotspot Intensity Sweep

在 `Hotspot-like` 负载下，随着热点区变小、热点访问比例变高：

- `(hk=30%, ha=70%)`：`33.39%`
- `(hk=20%, ha=80%)`：`58.16%`
- `(hk=10%, ha=90%)`：`91.12%`

这表明：当访问越来越集中在更小的一批热点 key 上时，LRU 更容易把这些 key 稳定保留在缓存中，命中率会显著上升。

#### Zipf Alpha Sweep

在 `Zipf-like` 负载下，随着 `alpha` 增大：

- `alpha=0.8`：`52.38%`
- `alpha=1.0`：`69.69%`
- `alpha=1.2`：`84.20%`
- `alpha=1.5`：`95.42%`

这说明 `Zipf` 分布越偏斜，头部 key 的访问越集中，LRU 越能利用这种局部性获得更高收益。

这两组实验共同说明：

> **LRU 不仅受 workload 类型影响，还高度依赖 workload 内部偏斜程度的强弱。**

---

### 5. 吞吐也会随着局部性增强而提升

除了命中率，`OpsPerSec` 也呈现出明显趋势。

例如在 `capacity=200`、`key_space=1000` 时：

- `Uniform`：`23.64M/s`
- `Hotspot-like`：`38.86M/s`
- `Zipf-like`：`60.41M/s`

这说明在当前实现中，随着 workload 局部性增强，吞吐也在同步提高。

原因是：

- `hit` 路径主要是查找和更新访问顺序
- `miss` 路径除了查找失败，还需要插入新 key，并在必要时执行淘汰

因此在当前 LRU 实现下，`miss` 通常比 `hit` 更重。  
当命中率提高时，平均单次操作的开销下降，于是整体吞吐也会更高。

需要注意的是，这里的吞吐结果来自当前实现和当前实验环境，更适合作为**经验性观察**，而不是抽象成绝对理论结论。

---

## 结论

综合这些实验，可以得到以下结论：

- cache capacity 增大时，命中率通常会上升
- key space 增大时，在固定容量下缓存效果会下降
- workload 局部性越强，LRU 收益越明显
- workload 偏斜越强，命中率和吞吐通常越高
- 与简单热点模型相比，`Zipf-like` 这类长尾高偏斜分布往往更能发挥 LRU 的优势

总体来看，这些实验说明：

> **LRU 的效果不仅取决于缓存容量本身，还高度依赖访问分布的结构。**

对于具有强局部性、强偏斜性的 workload，LRU 往往能取得非常显著的收益；而在访问高度分散的 workload 下，其效果会明显受限。

---

## 局限性

当前实验仍然是一个简化版本，主要有以下限制：

- 目前只实现了**单线程** LRU
- 当前只比较了一种缓存策略，没有引入 LFU、ARC 等其他策略
- workload 为**人工构造的 synthetic workload**，而不是真实线上流量
- 当前 benchmark 主要聚焦缓存行为本身，还不是完整的 AI serving runtime 实验

尽管如此，这套实验框架仍然适合用于：

- 学习和验证缓存策略基本行为
- 观察 workload 分布对缓存效果的影响
- 为后续扩展更贴近 AI serving 的 workload 和指标打基础