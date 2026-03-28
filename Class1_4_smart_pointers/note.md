# Class 4 综合实战 —— 混合架构与工程陷阱

## 知识点

核心知识点：enable_shared_from_this
这是一个容易被误用的神器。当一个对象已经被 shared_ptr 管理，而你需要在成员函数内获取指向自己的 shared_ptr 时，不能直接 shared_ptr(this)（会导致双重释放），必须继承 enable_shared_from_this 并调用 shared_from_this()。

## 1.cpp


## 2.cpp

背景：
本课我们将模拟一个 IMU 数据处理器，它需要：

- 使用 unique_ptr 管理大块内存缓冲区（独占资源）
- 使用 shared_ptr 共享配置参数（多模块共享）
- 使用 enable_shared_from_this 安全地将自己注册给回调系统


场景背景：
你需要实现一个 ImuProcessor 类，它包含：

独占资源：一个固定大小的环形缓冲区，用于存储最近的 IMU 数据。由于数据量大且频繁更新，使用 unique_ptr 管理，避免不必要的引用计数开销。
共享配置：一个配置参数结构，可能被多个模块共享（如日志模块、算法模块）。使用 shared_ptr 管理。
回调注册：处理器需要能将自己注册到外部的事件循环中，使用 enable_shared_from_this。
