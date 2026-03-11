# 1.实验目的

本实验旨在观察 LRU 在不同访问模式和不同缓存容量下的命中率变化，验证 workload 特征对缓存行为的影响，并为后续 throughput/总耗时分析提供基础。

# 2.实验配置

缓存策略：LRU

workload：Uniform / Hotspot-like

capacity：50 / 100 / 200 / 500

指标：命中率

可复现性：固定 seed，固定 key_space



# 3.workload说明

Uniform：均匀随机访问，局部性弱

Hotspot-like：热点访问，约 80% 请求落在前 20% key 空间，局部性更强

选择这两类 workload，是为了对比“弱局部性”和“强局部性”场景下 LRU 的表现



## 4.命中率结果

![image-20260308001004884](C:\Users\13004\AppData\Roaming\Typora\typora-user-images\image-20260308001004884.png)

容量从 50 到 500 时，Uniform 从 4.83% 升到 48.27%，Hotspot 从 17.69% 升到 86.93%。

# 5.初步结论

容量增大时，LRU缓存命中率持续提升

Hotspot-like下命中率显著高于Uniform

LRU对时间局部性访问更强的访问模式更敏感

当容量逐渐接近热点集合大小时，命中率显著提升

