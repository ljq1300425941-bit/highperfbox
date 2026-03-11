

# Highperfbox



## 一分钟项目介绍稿

​	HighPerfBox 是一个面向 C++ 性能工程训练的实验型项目，当前聚焦缓存策略、访问负载建模与 benchmark 评测，用来研究不同 workload 和容量配置下的缓存行为。
​	 我目前已经完成了通用缓存接口 ICache、基于 `unordered_map + list` 的单线程 O(1) LRU Cache、Workload Generator，以及基础正确性测试，并接入 Google Benchmark 做性能评测。
 	在实验设计上，我构建了 Uniform 和 Hotspot-like 两类访问负载，并在不同缓存容量下观察命中率变化。实验结果表明，随着容量增大，LRU 命中率持续提升，而且在热点访问模式下明显优于均匀访问，例如容量为 500 时，Hotspot-like 和 Uniform 的命中率分别达到 86.93% 和 48.27%。
 	下一步我会继续补充 throughput 和总耗时统计，把这个项目进一步打磨成一个可复现、可量化、可解释的性能实验系统。

## 目录树结构解析

### highperfbox/

这是“仓库根目录”。

规则：

- 一个 Git 仓库 = 一个完整项目
- 所有构建、依赖、说明，都从这里开始
- 任何人 clone 下来，应该一眼看懂结构

它是工程的“地表”。

#### CMakeLists.txt

这是构建系统入口。

作用：

- 告诉编译器怎么编译
- 设置 C++ 版本
- 管理依赖
- 生成 Makefile 或 Ninja

规则：

- 顶层必须有一个
- 子目录可以有自己的 CMakeLists.txt
- 所有编译逻辑都在这里声明

它是“施工图纸”。



#### README.md

作用：

- 项目介绍
- 技术栈
- 如何编译
- 运行方法
- 设计思路

规则：

- 必须写
- 永远保持更新
- 面试官会先看这个

它是“项目的封面”。

####  include/

头文件目录 与src实现分离

这是 C++ 工程规范。

##### include/hp/

这是命名空间映射。

规则：

目录名 = 命名空间

例如：

```
#include <hp/logger.hpp>

namespace hp {
    class Logger {};
}
```

这叫做：

“目录与命名空间对齐原则”

优点：

- 不冲突
- 清晰
- 专业


###### core

###### log

#### src/

  规则：

  - 所有 .cpp 在这里
  - 不暴露给外部
  - 只编译，不被直接 include

  它是“发动机内部结构”。

##### src/core/

  分模块管理。

  以后你可能会有：



```
src/core/cache/
src/core/net/
src/core/memory/
```

规则：

- 一个子目录 = 一个逻辑模块
- 模块内部高内聚
- 模块之间低耦合

这就是软件工程的“物理边界”。

##### src/log

####  benchmark/

性能测试代码。

为什么单独放？

因为：

- benchmark 不属于业务逻辑
- 不应该和 src 混在一起
- 可以单独编译

#### test/

单元测试目录。

放：

- gtest
- mock
- 测试代码

原则：

测试代码 ≠ 生产代码

你未来面试说：

“我写了完整的单元测试覆盖”

工程分量立刻翻倍。

#### third_party/

放第三方依赖源码。

例如：

- google benchmark
- fmt
- spdlog

规则：

- 不手写
- 不修改
- 不直接开发

它是“外部供应商仓库”。

####  scripts/

放脚本。

例如：

- build.sh
- run_bench.sh
- deploy.sh

原则：

自动化优先。

工程师的尊严来自自动化。

#### build/

构建产物目录。

规则：

- 永远不要提交到 Git
- 用 .gitignore 忽略

它只是临时文件。

你删掉也没事。

------



### 为什么include和src必须分开？

#### 编译层面

头文件是“声明”

如果不分开，会出现两个问题：

1. 编译耦合严重
2. 重复包含导致链接错误

分离可以：

- 降低编译时间
- 控制依赖方向
- 避免循环依赖

这是“技术层理由”。

#### 架构层面

头文件是接口边界。

当别人使用你的库，

他不需要知道：

- 你用了 mutex
- 你用了 std::ofstream
- 你内部是否优化

这叫：

**信息隐藏（Information Hiding）**

软件工程的核心原则之一。

#### 工程规模

假设项目 10 万行代码。

如果没有 include/src 分离：

- 所有人可以直接 include 内部实现
- 没有边界
- 改一处牵动全局

分离之后：

- include = 稳定API层
- src = 可随意重构

你可以疯狂优化 src，
 只要接口不变，外部代码无需修改。

这就是“可维护性”。

#### 抽象能力

真正厉害的工程师会思考：

> 哪些是稳定的？
> 哪些是会变化的？

稳定的放接口。
 变化的放实现。

这叫：

“变化隔离原则”。

------



## linkage

### 是什么

符号可见性

1. 每个 `xxx.cpp` 单独编译成 `xxx.o`（目标文件）
2. 目标文件里会有很多“符号”（函数名、全局变量名等）
3. 链接器把所有 `.o` 拼起来，解决“这个名字到底指向哪一份实现”

**linkage** 描述的就是：

> 这个名字（符号）是只在本 `.o` 内部有效，还是能被别的 `.o` 引用？

### Internal linkage

**定义：符号只在“当前翻译单元”（一个 `.cpp` + 它 include 的所有头文件）里可见。**
 别的 `.cpp` 根本看不见它的名字

A) 全局 `static`

B) 全局 `const`（注意是 C++，且在命名空间作用域）

C) 匿名命名空间

**internal 的好处**

- 不会产生跨文件符号冲突（multiple definition）
- 能把实现细节“藏起来”，减少全局命名污染

**⚠️ internal 的代价**

- **每个翻译单元都会生成一份副本**
   如果函数/对象大，会“代码膨胀”或“数据膨胀”

### Exteral linkage

定义：符号对整个程序可见，多个 `.o` 之间可以互相引用。

A) 普通全局变量 / 普通非成员函数

B) `extern` 声明（常用于头文件里声明，在某个 cpp 定义）

 **external 的好处**

- 全项目共享“同一个实体”（同一个变量/函数实现）
- 能做库接口/API

**⚠️ external 的危险点（你最常遇到的坑）**

- 如果你在多个 `.cpp` 都“定义”了同一个 external 符号：
   💥 链接错误：multiple definition

### header注意事项

默认链接为external linkage 的头文件被重复引用会产生冲突 

即multiple definition



#### inline

`inline` 的核心不是“让它变快”（那只是可能性），而是：

> **允许 external linkage 的符号在多个翻译单元重复定义，链接器会合并。**

```
// header
inline int add(int a,int b){ return a+b; }
```

多个 `.cpp` include 没事，因为它是 **“允许重复定义但视为同一个”** 的规则（ODR 相关）。

#### 工程规则

**头文件（include/）里建议放：**

- `class/struct` 声明
- `extern` 声明
- `inline` 小函数 / 模板
- `constexpr/const` 常量（或 C++17 inline 变量）

**源文件（src/）里建议放：**

- external 的“唯一”定义（比如 `int g = 1;`）
- 大函数实现
- 私有实现细节（匿名命名空间）

------



## Git

### 修改后提交

```
git add .
git commit -m "写清楚你做了什么"
git push
```



------

## CMake

### make

make 是一个 **构建工具（build tool）**。

它的作用是：

> 根据一份规则文件（Makefile），决定哪些文件需要重新编译，然后执行对应的命令。

它解决的核心问题是：

- 哪些源文件改了？

- 哪些目标文件需要重新生成？

- 编译顺序是什么？

- 依赖关系是什么？

  ```
  g++ main.cpp cache.cpp util.cpp -o highperfbox
  ```

如果只改了 util.cpp 

还得全量重新编译。

而 make 可以：

✔ 只编译 util.cpp
 ✔ 只重新链接
 ✔ 自动判断依赖

这就是它存在的意义。

### Makefile

```
//makefile

target: dependency
    command
    
highperfbox: main.o cache.o
    g++ main.o cache.o -o highperfbox
```

### 是什么

手写 Makefile 很痛苦。

当项目变大：

- 有很多目录
- 有静态库
- 有动态库
- 有第三方依赖
- 有不同平台（Linux / Windows / Mac）

手写 Makefile 会变成地狱。

于是，CMake 出现了。



CMake 是一个：

> 构建系统生成器（Build System Generator）

它不负责编译。

它的工作是：

👉 生成 Makefile
 👉 或者生成 Ninja 文件
 👉 或者生成 Visual Studio 工程

然后再交给 make 或 ninja 去执行。



### 怎么用

```
cmake ..
```

CMake 做了：

1. 读取 CMakeLists.txt
2. 分析你的工程结构
3. 生成 Makefile
4. 写入 build 目录

```
make
```

make 才真正开始编译。

### 优势

 1️⃣ 跨平台
 2️⃣ 支持大工程
 3️⃣ 支持第三方库管理
 4️⃣ 支持多构建系统（Make / Ninja / MSVC）
 5️⃣ 生态成熟

像：

- Linux 内核不用 CMake（历史原因）
- 但几乎所有现代 C++ 项目都用 CMake

CMake 把工程抽象成：

- target
- library
- executable
- dependency
- include path
- compile options

这是工程化的核心。

### 流程图

```
CMakeLists.txt
        ↓
cmake ..
        ↓
生成 Makefile
        ↓
make
        ↓
生成 highperfbox 可执行文件
```

CMake 不是编译器。

g++ 才是。

make 也不是编译器。

它只是执行命令的调度器。

整个链条是：

```
CMake → Make → g++ → binary
```



------

## Logger

### atomic

#### 应对问题

对于下述情况的优化

```
std::lock_guard<std::mutex> lk(mu_);
    if (static_cast<int>(lv) < static_cast<int>(level_)) return;
```

如果lv真的小于level_ 虽然会return 但是还是有加锁和解锁的过程 

这在高并发时候产生的损耗是不能被忽略的 所以使用原子操作

#### 为什么用

我一开始有个疑问

直接不加锁不可以吗？

那就产生问题了

线程 A：

```
logger.set_level(Level::Warn);
```

线程 B：

```
logger.log(Level::Debug, ...);
```

一个在写 `level_`
 一个在读 `level_`

如果没有锁，也没有 atomic，会发生什么？

 这是**数据竞争（data race）**

在 C++ 内存模型里：

> 数据竞争 = 未定义行为

未定义行为不是“可能错”。

是：

- 可能读到旧值
- 可能读到撕裂值
- 可能崩溃
- 可能优化器重排导致诡异问题

而且即使不是一读一写

同时去读也会发生数据竞争 导致未定义问题

#### 作用

std::atomic<Level> level_;

atomic 保证：

1️⃣ 读写是原子的（不会撕裂）
2️⃣ 具有内存可见性保证
3️⃣ 不需要 mutex

它解决的是：

> “并发可见性问题”

不是“性能问题”。

#### 适用性

只适合单变量读写 相对于mutex轻量很多

| 情况         | 用 atomic | 用 mutex |
| ------------ | --------- | -------- |
| 单变量读写   | ✅         | 可以但重 |
| 多变量一致性 | ❌         | ✅        |
| 函数执行区间 | ❌         | ✅        |
| IO 操作      | ❌         | ✅        |

### 时间拼写操作

### 踩坑

我开始认为如果锁的粒度足够小，那么加锁不太会影响性能

但是经过测试后，加锁解锁本身就会产生开销

而且实际上会比核心代码开销更大，所以才使用原子操作，提升性能



------

## Config

### 是什么

> 让“程序行为”从“代码里”移到“外部文件”。

这叫：

👉 行为外部化
👉 运行时控制
👉 策略解耦



假设你要测试缓存系统。

你有几个可调参数：

- cache_size
- replacement_policy（LRU / FIFO）
- thread_count
- 是否开启日志

------

没有 Config 的世界

你的代码里写：

```
int cache_size = 1024;
int thread_count = 4;
bool enable_log = true;
```

然后你想测试：

- cache_size=1024
- cache_size=4096
- cache_size=16384

你怎么办？

👉 改代码
👉 重新编译
👉 再运行

改三次，编译三次。

这还只是一个参数。

------

🔥 当参数多起来时

假设你要测试：

- 3 种 cache_size
- 3 种策略
- 3 种线程数

组合数是：

3 × 3 × 3 = 27 次实验

你每次改代码？

这不现实。

------

✅ 有 Config 的世界

你写：

```
cache_size=1024
policy=LRU
threads=4
enable_log=false
```

然后你运行：

```
./highperfbox
```

想换参数？

改 config.ini。

不需要编译。



------

## benchmark

### 是什么

Benchmark 是一种**可重复、可量化、可对比**的性能测试方法。

它回答的问题是：

- A 和 B 谁更快？
- 数据规模变大时性能如何变化？
- 优化是否真的有效？
- 时间复杂度是否符合预期？

它不是“跑一下看看感觉”。

它是工程级测速。

### 运行解析

![image-20260302221517940](C:\Users\13004\AppData\Roaming\Typora\typora-user-images\image-20260302221517940.png)

CPU Caches 代表每层cpu缓存容量

重点解析表格

1000 10000 100000 代表测试使用的数据量

- **Time**：每次基准循环的“墙钟时间”平均值（ns/op）
- **CPU**：每次基准循环的 CPU 时间平均值（ns/op）
- **Iterations**：Google Benchmark 为了稳定测量自动跑了多少次

### 具体测试

#### 1.map VS unordered_map

![](C:\Users\13004\Pictures\Screenshots\屏幕截图 2026-03-02 222629.png)

map 底层红黑树

平均O（NlogN）

unordered_map 底层哈希表

平均O（N）

结论：

我用 Google Benchmark 对比了 map 与 unordered_map 的插入性能
在 100k 规模下，unordered_map 约为 map 的 1/3 时间
增长趋势符合理论复杂度

#### 2.find对比

![image-20260302231110534](C:\Users\13004\AppData\Roaming\Typora\typora-user-images\image-20260302231110534.png)

find对比性能差距更明显了

插入时差距只有 ~3倍。

查找时差距 ~22倍。

🔹 1️⃣ 红黑树 = 指针跳跃

map 查找时：

- 每层都是一次指针访问
- 指针可能分散在内存各处
- CPU cache 命中率低

这是“缓存不友好结构”。

------

🔹 2️⃣ 哈希表 = 局部连续访问

unordered_map：

- 桶数组是连续内存
- 访问更接近数组
- cache 命中率更高

结论： 

 在 100k 规模下，map 查找约为 unordered_map 的 22 倍时间
 查找阶段差距远大于插入阶段
 主要原因是树结构的 cache locality 差

#### 3.是否预留空间

![image-20260302230337592](C:\Users\13004\AppData\Roaming\Typora\typora-user-images\image-20260302230337592.png)

可见 还是有差距的

要是不reserve空间的话 会发生rehash产生开销

具体发生：

 1️⃣ 插入
 2️⃣ load_factor 超过阈值
 3️⃣ 触发 rehash
 4️⃣ 重新分配更大的 bucket 数组
 5️⃣ 所有元素重新计算 hash
 6️⃣ 所有节点重新挂载

什么时候用reserve

✅ 已知大概数据规模
✅ 批量插入
✅ 性能敏感路径
✅ 服务端核心逻辑

## cache



### LRU

#### LRU 在什么情况下表现很差

顺序扫描

#### 为什么真实系统很少用纯 LRU

1.维护成本高

2.scan pollution 即上文所说

3.并发难



## assert

### 是什么

先包含头文件：

```
#include <cassert>
```

然后写：

```
assert(x == 10);
```

意思是：

- 如果 `x == 10` 为真，程序继续跑
- 如果为假，程序直接终止，并提示哪一行断言失败

### 和if的区别

比如你也可以写：

```
if (v != 10) {
    std::cout << "error\n";
}
```

但 `assert` 更适合“这里必须成立”的场景，因为它更像一把闸刀：

- `if` 更偏业务逻辑判断
- `assert` 更偏“程序员自检”

你现在用它，不是在写正式测试框架，而是在给 LRU 做一次快速体检。

## workload

主要就是读懂代码

#### `class WorkloadType`

​	这是在定义“工作负载类型”。

你可以理解成一个开关：

- `Uniform` = 均匀随机访问
- `Hotspot` = 热点访问

为什么不用普通 enum？

因为 `enum class` 更安全，不容易和别的名字混在一起。

##### uniform模式

意思很简单：

- 用随机引擎 `rng_`
- 按均匀分布 `dist_`
- 生成一个 key

所以所有 key 被访问的概率差不多一样。

这适合做基线测试。

##### hotspot模式

这段是今天最值得吃透的部分。

它模拟的是：

> 80% 的访问集中在前 20% 的 key 上

这就是“热点”。

#### key_space

限定key的范围 很重要

后面缓存容量、命中率、热点分布都和它有关。

#### seed

随机数种子 确保重复试验后数据可以复现

#### dist_(0, key_space - 1)

这是一个均匀整数分布对象，表示：

> 从 `[0, key_space - 1]` 里等概率取值

比如 `key_space = 100`，那就是从 0 到 99 里均匀随机取一个数。

注意这里不是“立刻生成随机数”，而是**先创建分布规则**。

#### next统一接口

涉及一种设计思想

它的意思是：

> 对外暴露统一接口 `next()`
>  但内部根据 workload 类型，走不同生成逻辑

```
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
```

#### generate(size_t n)

相当于多次调用next

next是单发

generate是连发

#### 实际测试

![image-20260308001004884](C:\Users\13004\AppData\Roaming\Typora\typora-user-images\image-20260308001004884.png)

观察数据能看出下面几点

1.capacity增加 hit rate增加

这个不用解释很简单

2.hotspot一直比uniform多 

这个跟lru特性有关 直接看lru就行

3.hotspot比uniform增长的快很多

直觉上讲hotspot是20%数据被80%访问 那么20%就是热点数据

key_space给的是1000 即热点数据量为200

也就是说capacity达到200左右就能够大体装下所有热点数据了

**Uniform 下：**

容量变大确实有帮助，但因为访问分散，所以帮助是“有限但稳定”的。

**Hotspot 下：**

容量一旦足够把主要热点区装进去，命中率会迅速抬升。

你这组数据就很能说明这一点：

- 50   → 100：17.69% → 34.54%
- 100 → 200：34.54% → 62.76%
- 200 → 500：62.76% → 86.93%

4.capacity无限提升 hit rate不会无限增加

看数据也知道 会有一个提升峰值 之后增速就会变缓

因为缓存收益会逐渐趋于饱和。
当主要热点已经被装进缓存后，再继续扩大容量，新增空间带来的边际收益会变小。
