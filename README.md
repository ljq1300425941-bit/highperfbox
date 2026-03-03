# HighPerfBox

A high-performance cache experimentation platform built with modern C++20.

HighPerfBox focuses on performance measurement, cache design, and benchmark-driven optimization.

## 🚀 Project Vision

This project is designed to:

- Explore cache data structure implementations (LRU, LFU, FIFO, ARC etc.)
- Measure performance using Google Benchmark
- Analyze STL container behavior at scale
- Study concurrency strategies in modern C++

The goal is to build a reproducible, benchmark-oriented performance lab.

---

##  Architecture (Week 1 Completed)

### Core Infrastructure

- Modern CMake-based project structure
- Strict include/src separation
- C++20 enabled
- Debug / Release build modes

### Logger Module

- Thread-safe logging
- Atomic level filtering
- Minimal lock scope design

### Config System

- Runtime-configurable parameters
- Externalized behavior control
- Designed for future experiment scaling

### Benchmark Integration

- Google Benchmark integrated via submodule
- Release-mode performance measurement
- Structured benchmark outputs

## 📊 Benchmark Highlights

### 1️⃣ map vs unordered_map (Insert)

At 100k elements:

| Container | Time (ns) |
|------------|-----------|
| std::map | ~7.8 ms |
| std::unordered_map | ~2.2 ms |

unordered_map shows ~3.5x speedup for insertion.

---

### 2️⃣ map vs unordered_map (Find)

At 100k elements:

| Container | Time (ns) |
|------------|-----------|
| std::map | ~4.2 ms |
| std::unordered_map | ~0.19 ms |

unordered_map shows ~22x speedup for find operations.

This demonstrates the impact of:

- Tree traversal vs hash indexing
- Cache locality differences
- Algorithmic complexity in real-world hardware

### 3️⃣ Reserve Impact on unordered_map

Pre-reserving capacity significantly reduces rehash overhead:

- Avoids bucket reallocation
- Avoids full rehashing
- Stabilizes performance at scale


## 📁 Project Structure
highperfbox/
├── include/ # Public interfaces
├── src/ # Implementation
├── benchmark/ # Performance tests
├── third_party/ # External dependencies
├── docs/ # Learning notes
└── CMakeLists.txt

## 🛠 Build Instructions

mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j