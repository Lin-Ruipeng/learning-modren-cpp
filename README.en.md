# Modern C++ Study

A comprehensive C++ high-performance programming learning project, focusing on smart pointers, move semantics, and multi-threaded concurrency.

## Project Overview

This is a practical course for systematically learning modern C++ (C++11/14/17/20) core features, using IMU (Inertial Measurement Unit) sensor data processing as the business scenario:

- **Smart Pointers**: unique_ptr / shared_ptr / weak_ptr - principles and engineering practice
- **Move Semantics**: rvalue references, move constructors, RVO/NRVO optimization, zero-copy design
- **Type Safety**: optional / tuple / variant - modern C++ safe alternatives
- **Multi-threading**: thread / mutex / condition_variable / atomic / async / thread pool

## Directory Structure

```
Modern_Cpp/
├── Chapter_1_Smart_Pointer/              # Chapter 1: Smart Pointers
│   ├── Class1_1_unique_ptr/       # 1.1 unique_ptr Exclusive Ownership
│   │   ├── 1.cpp             # Basic syntax
│   │   ├── 2.cpp             # IMU scenario practice
│   │   └── note.md            # Learning notes
│   ├── Class1_2_shared_ptr/    # 1.2 shared_ptr Shared Ownership
│   │   ├── 1.cpp
│   │   ├── 2.cpp
│   │   └── note.md
│   ├── Class1_3_weak_ptr/      # 1.3 weak_ptr Weak Reference
│   │   ├── 1.cpp
│   │   ├── 2.cpp
│   │   └── note.md
│   ├── Class1_4_smart_pointers/ # 1.4 Comprehensive Practice
│   │   ├── 1.cpp
│   │   ├── 2.cpp
│   │   └── note.md
│   └── prompt.md                # Chapter prompt
│
├── Chapter_2_Move_and_SafetyType/    # Chapter 2: Move Semantics & Type Safety
│   ├── Class2_1_Rvalue_reference/     # 2.1 Rvalue Reference & Function Overload
│   │   ├── 1.cpp
│   │   ├── 2.cpp
│   │   └── note.md
│   ├── Class2_2_noexcept_and_Mobile_structure/ # 2.2 Move Constructor & noexcept
│   │   ├── 1.cpp
│   │   ├── 2.cpp
│   │   └── note.md
│   ├── Class2_3_modern_rule_of_five/   # 2.3 Modern Zero-cost Abstraction
│   │   ├── 2.cpp
│   │   └── note.md
│   ├── Class2_4_RVO_NRVO/        # 2.4 RVO Return Value Optimization
│   │   ├── 2.cpp
│   │   └── note.md
│   ├── Class2_5_STL_noexpect/     # 2.5 noexcept Lifecycle
│   │   ├── 2.cpp
│   │   └── note.md
│   ├── Class2_6_optional/       # 2.6 std::optional
│   │   ├── 2.cpp
│   │   └── note.md
│   ├── Class2_7_weak_ptr/      # 2.7 shared_ptr Circular Reference
│   │   ├── main.cpp
│   │   └── note.md
│   ├── Class2_8_tuple/         # 2.8 std::tuple
│   │   ├── main.cpp
│   │   └── note.md
│   ├── Class2_9_variant/      # 2.9 std::variant Static Polymorphism
│   │   ├── main.cpp
│   │   └── note.md
│   ├── Class2_10_lambda/     # 2.10 Lambda Expressions
│   │   ├── main.cpp
│   │   └── note.md
│   ├── Class2_11_project/    # 2.11 Comprehensive Project
│   │   └── src/main.cpp
│   └── prompt.md
│
├── Chapter_3_multi_thread/       # Chapter 3: Multi-threading & Concurrency
│   ├── Class3_1_thread/      # 3.1 Thread Basics
│   │   ├── src/main.cpp
│   │   ├── src/example.cpp
│   │   ├── CMakeLists.txt
│   │   └── note.md
│   ├── Class3_2_mutex/       # 3.2 Mutex & Locking
│   │   ├── src/main1.cpp
│   │   ├── src/main2.cpp
│   │   ├── src/example.cpp
│   │   ├── CMakeLists.txt
│   │   └── note.md
│   ├── Class3_3_condition_variable/ # 3.3 Condition Variables
│   │   ├── src/main1.cpp
│   │   ├── src/main2.cpp
│   │   ├── src/example.cpp
│   │   ├── CMakeLists.txt
│   │   └── note.md
│   ├── Class3_4_atomic/     # 3.4 Atomic Operations
│   │   ├── src/main.cpp
│   │   ├── src/example.cpp
│   │   ├── CMakeLists.txt
│   │   └── note.md
│   ├── Class3_5_async/     # 3.5 async & Asynchronous
│   │   ├── src/main1.cpp
│   │   ├── src/main2.cpp
│   │   ├── src/example.cpp
│   │   ├── CMakeLists.txt
│   │   └── note.md
│   ├── Class3_6_share_data/ # 3.6 Cross-thread Data Transfer
│   │   ├── src/main1.cpp
│   │   ├── src/main2.cpp
│   │   ├── src/example.cpp
│   │   ├── CMakeLists.txt
│   │   └── note.md
│   ├── Class3_7_thread_pool/ # 3.7 Thread Pool
│   │   ├── main.cpp
│   │   ├── thread_num_benchmark.md
│   │   ├── benchmark_report.md
│   │   └── note.md
│   ├── Class3_8_project/ # 3.8 Comprehensive Project
│   │   ├── main.cpp
│   │   └── note.md
│   └── prompt.md
│
├── .clang-format                  # Code formatting config
├── .cmake-format.yaml            # CMake formatting config
├── .editorconfig              # Editor config
├── .gitignore
├── LICENSE                   # MIT License
├── README.md                # Chinese README
└── README.en.md            # English README
```

## Learning Path

### Chapter 1: Smart Pointers

| Lesson | Topic | Key Concepts |
|:---:|:---|:---|
| 1.1 | unique_ptr Exclusive Ownership | make_unique, reset, release, std::move |
| 1.2 | shared_ptr Shared Ownership | make_shared, custom deleter, use_count, thread safety |
| 1.3 | weak_ptr Weak Reference | lock, expired, circular reference resolution |
| 1.4 | Comprehensive Practice | enable_shared_from_this, hybrid architecture |

### Chapter 2: Move Semantics & Type Safety

| Lesson | Topic | Key Concepts |
|:---:|:---|:---|
| 2.1 | Rvalue Reference & Function Overload | T& / T&& overload, lvalue/rvalue matching |
| 2.2 | Move Constructor & noexcept | move constructor, noexcept importance |
| 2.3 | Modern Zero-cost Abstraction | std::vector replacing raw pointers, Rule of Five demise |
| 2.4 | RVO/NRVO | Return Value Optimization, implicit move |
| 2.5 | noexcept Lifecycle | STL fallback, container expansion |
| 2.6 | std::optional | Optional value, magic number killer |
| 2.7 | weak_ptr Circular Reference | Breaking cycles, lifecycle management |
| 2.8 | std::tuple | Structured binding, multiple return values |
| 2.9 | std::variant | Static polymorphism, type-safe union |
| 2.10 | Lambda Expressions | Init capture, mutable |
| 2.11 | Comprehensive Project | Comprehensive application |

### Chapter 3: Multi-threading & Concurrency

| Lesson | Topic | Key Concepts |
|:---:|:---|:---|
| 3.1 | Thread Basics | std::thread, join, detach |
| 3.2 | Mutex & Locking | std::mutex, lock_guard, unique_lock, data race |
| 3.3 | Condition Variables | Producer-consumer model, spurious wakeup |
| 3.4 | Atomic Operations | std::atomic, lock-free programming |
| 3.5 | Asynchronous Programming | std::async, promise/future |
| 3.6 | Cross-thread Data Transfer | unique_ptr zero-copy, shared_ptr |
| 3.7 | Thread Pool | Task queue, thread reuse |
| 3.8 | Comprehensive Project | IMU multi-threaded processing framework |

## Build & Run

```bash
# Method 1: Direct compilation
g++ -std=c++17 -std=c++20 Class2_10_lambda/main.cpp -o app

# Method 2: Using CMake (Chapter 3)
cd Chapter_3_multi_thread/Class3_1_thread
mkdir build && cd build
cmake .. && make
./app
```

## Requirements

- GCC/Clang compiler (C++17 or higher)
- CMake 3.10+ (for Chapter 3)

## License

MIT License - see LICENSE file