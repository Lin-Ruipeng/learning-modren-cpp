# Modern C++ Study

现代 C++ 高性能编程学习项目，专注智能指针、移动语义与多线程并发。

## 项目简介

本项目是系统学习现代 C++ (C++11/14/17/20) 核心特性的实践课程，以 IMU（惯性测量单元）传感器数据处理为业务场景，涵盖：

- **智能指针**：unique_ptr / shared_ptr / weak_ptr 的原理与工程实践
- **移动语义**：右值引用、移动构造、RVO/NRVO 优化、零拷贝设计
- **类型安全**：optional / tuple / variant 的现代 C++ 替代方案
- **多线程并发**：thread / mutex / condition_variable / atomic / async / thread pool

## 文件结构

```
Modern_Cpp/
├── Chapter_1_Smart_Pointer/              # 第1章：智能指针
│   ├── Class1_1_unique_ptr/       # 1.1 unique_ptr 独占所有权
│   │   ├── 1.cpp             # 基础语法
│   │   ├── 2.cpp             # IMU场景实战
│   │   └── note.md            # 学习笔记
│   ├── Class1_2_shared_ptr/    # 1.2 shared_ptr 共享所有权
│   │   ├── 1.cpp
│   │   ├── 2.cpp
│   │   └── note.md
│   ├── Class1_3_weak_ptr/      # 1.3 weak_ptr 弱引用
│   │   ├── 1.cpp
│   │   ├── 2.cpp
│   │   └── note.md
│   ├── Class1_4_smart_pointers/ # 1.4 综合实战
│   │   ├── 1.cpp
│   │   ├── 2.cpp
│   │   └── note.md
│   └── prompt.md                # 章节 prompt
│
├── Chapter_2_Move_and_SafetyType/    # 第2章：移动语义与安全类型
│   ├── Class2_1_Rvalue_reference/     # 2.1 右值引用与函数重载
│   │   ├── 1.cpp
│   │   ├── 2.cpp
│   │   └── note.md
│   ├── Class2_2_noexcept_and_Mobile_structure/ # 2.2 移动构造与noexcept
│   │   ├── 1.cpp
│   │   ├── 2.cpp
│   │   └── note.md
│   ├── Class2_3_modern_rule_of_five/   # 2.3 现代零成本抽象
│   │   ├── 2.cpp
│   │   └── note.md
│   ├── Class2_4_RVO_NRVO/        # 2.4 RVO返回值优化
│   │   ├── 2.cpp
│   │   └── note.md
│   ├── Class2_5_STL_noexpect/     # 2.5 noexcept的生杀局
│   │   ├── 2.cpp
│   │   └── note.md
│   ├── Class2_6_optional/       # 2.6 std::optional
│   │   ├── 2.cpp
│   │   └── note.md
│   ├── Class2_7_weak_ptr/      # 2.7 shared_ptr循环引用
│   │   ├── main.cpp
│   │   └── note.md
│   ├── Class2_8_tuple/         # 2.8 std::tuple
│   │   ├── main.cpp
│   │   └── note.md
│   ├── Class2_9_variant/      # 2.9 std::variant静态多态
│   │   ├── main.cpp
│   │   └── note.md
│   ├── Class2_10_lambda/     # 2.10 Lambda表达式
│   │   ├── main.cpp
│   │   └── note.md
│   ├── Class2_11_project/    # 2.11 综合项目
│   │   └── src/main.cpp
│   └── prompt.md
│
├── Chapter_3_multi_thread/       # 第3章：多线程与并发
│   ├── Class3_1_thread/      # 3.1 线程基础
│   │   ├── src/main.cpp
│   │   ├── src/example.cpp
│   │   ├── CMakeLists.txt
│   │   └── note.md
│   ├── Class3_2_mutex/       # 3.2 互斥锁
│   │   ├── src/main1.cpp
│   │   ├── src/main2.cpp
│   │   ├── src/example.cpp
│   │   ├── CMakeLists.txt
│   │   └── note.md
│   ├── Class3_3_condition_variable/ # 3.3 条件变量
│   │   ├── src/main1.cpp
│   │   ├── src/main2.cpp
│   │   ├── src/example.cpp
│   │   ├── CMakeLists.txt
│   │   └── note.md
│   ├── Class3_4_atomic/     # 3.4 原子操作
│   │   ├── src/main.cpp
│   │   ├── src/example.cpp
│   │   ├── CMakeLists.txt
│   │   └── note.md
│   ├── Class3_5_async/     # 3.5 async与异步
│   │   ├── src/main1.cpp
│   │   ├── src/main2.cpp
│   │   ├── src/example.cpp
│   │   ├── CMakeLists.txt
│   │   └── note.md
│   ├── Class3_6_share_data/ # 3.6 跨线程数据传递
│   │   ├── src/main1.cpp
│   │   ├── src/main2.cpp
│   │   ├── src/example.cpp
│   │   ├── CMakeLists.txt
│   │   └── note.md
│   ├── Class3_7_thread_pool/ # 3.7 线程池
│   │   ├── main.cpp
│   │   ├── thread_num_benchmark.md
│   │   ├── benchmark_report.md
│   │   └── note.md
│   ├── Class3_8_project/ # 3.8 综合项目
│   │   ├── main.cpp
│   │   └── note.md
│   └── prompt.md
│
├── .clang-format                  # 代码格式化配置
├── .cmake-format.yaml            # CMake格式化配置
├── .editorconfig              # 编辑器配置
├── .gitignore
├── LICENSE                   # MIT许可证
├── README.md                # 中文README
└── README.en.md            # English README
```

## 学习路线

### 第1章：智能指针 (Smart Pointers)

| 课程 | 主题 | 核心知识点 |
|:---:|:---|:---|
| 1.1 | unique_ptr 独占所有权 | make_unique, reset, release, std::move |
| 1.2 | shared_ptr 共享所有权 | make_shared, custom deleter, use_count, 线程安全 |
| 1.3 | weak_ptr 弱引用 | lock, expired, 循环引用解决 |
| 1.4 | 综合实战 | enable_shared_from_this, 混合架构 |

### 第2章：移动语义与安全类型 (Move Semantics & Type Safety)

| 课程 | 主题 | 核心知识点 |
|:---:|:---|:---|
| 2.1 | 右值引用与函数重载 | T& / T&& 重载规则，左值右值匹配 |
| 2.2 | 移动构造与noexcept | 移动构造函数，noexcept重要性 |
| 2.3 | 现代零成本抽象 | std::vector 替代裸指针，Rule of Five 消亡 |
| 2.4 | RVO/NRVO | 返回值优化，隐式移动 |
| 2.5 | noexcept生杀局 | STL回退机制，容器扩容 |
| 2.6 | std::optional | 可选值，魔法数字终结者 |
| 2.7 | weak_ptr循环引用 | 打破循环，生命周期管理 |
| 2.8 | std::tuple | 结构化绑定，多返回值 |
| 2.9 | std::variant | 静态多态，类型安全联合体 |
| 2.10 | Lambda表达式 | 初始化捕获，mutable |
| 2.11 | 综合项目 | 综合运用 |

### 第3章：多线程与并发 (Multi-threading & Concurrency)

| 课程 | 主题 | 核心知识点 |
|:---:|:---|:---|
| 3.1 | 线程基础 | std::thread, join, detach |
| 3.2 | 互斥锁 | std::mutex, lock_guard, unique_lock, 数据竞争 |
| 3.3 | 条件变量 | 生产者-消费者模型，虚假唤醒 |
| 3.4 | 原子操作 | std::atomic, 无锁编程 |
| 3.5 | 异步编程 | std::async, promise/future |
| 3.6 | 跨线程数据传递 | unique_ptr零拷贝, shared_ptr |
| 3.7 | 线程池 | 任务队列，复用线程 |
| 3.8 | 综合项目 | IMU多线程处理框架 |

## 编译运行

```bash
# 方式一：直接编译
g++ -std=c++17 -std=c++20 Class2_10_lambda/main.cpp -o app

# 方式二：使用 CMake（Chapter 3）
cd Chapter_3_multi_thread/Class3_1_thread
mkdir build && cd build
cmake .. && make
./app
```

## 依赖要求

- GCC/Clang 编译器 (C++17 及以上)
- CMake 3.10+ (Chapter 3 部分)

## 许可证

MIT License - 见 LICENSE 文件