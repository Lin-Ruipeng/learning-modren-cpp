

# Modern C++ Study

Modern C++ 学习项目，专注于 C++11/14/17 中的智能指针核心特性。

## 项目简介

本项目通过实际案例演示现代 C++ 智能指针的使用方法，帮助理解内存管理机制。

## 目录结构

```
├── Class01_unique_ptr/          # 独占所有权智能指针
│   ├── 1.cpp                    # unique_ptr 基础用法
│   ├── 2.cpp                    # unique_ptr 实际场景应用
│   └── note.md                  # 学习笔记
│
├── Class02_shared_ptr/          # 共享所有权智能指针
│   ├── 1.cpp                    # shared_ptr 基础用法
│   ├── 2.cpp                    # shared_ptr 高级特性
│   └── note.md                  # 学习笔记
│
└── prompt.md                    # 教学框架
```

## 学习内容

### Class01: unique_ptr 独占所有权

- `make_unique<T>()` 创建独占指针
- `.reset()` 重置指针
- `.release()` 释放所有权
- `std::move()` 移动语义

### Class02: shared_ptr 共享所有权

- `make_shared<T>()` 创建共享指针
- 拷贝构造与赋值
- 自定义删除器
- `.use_count()` 查看引用计数

## 编译运行

```bash
# 编译 unique_ptr 示例
g++ -std=c++14 Class01_unique_ptr/1.cpp -o unique_ptr_demo1
g++ -std=c++14 Class01_unique_ptr/2.cpp -o unique_ptr_demo2

# 编译 shared_ptr 示例
g++ -std=c++14 Class02_shared_ptr/1.cpp -o shared_ptr_demo1
g++ -std=c++14 Class02_shared_ptr/2.cpp -o shared_ptr_demo2
```

## 依赖

- C++14 及以上标准
- 支持 C++11 智能指针的编译器

## 许可证

MIT License