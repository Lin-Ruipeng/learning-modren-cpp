# Class 1 unique_ptr —— 独占所有权与零开销抽象

## 1.cpp

### 1.创建独占指针 make_unique\<T\>()

```cpp
struct ImuData;
std::unique_ptr<ImuData> p1 = std::make_unique<ImuData>();
```

### 2.重置 .reset(新对象)
```cpp
// [重置] 释放当前内存，并接管新的内存（或置空）
p1.reset(new ImuData{1.0, 2.0, 3.0, 2000}); 
```

### 3.释放 .release()
```cpp
// [释放] 放弃所有权，返回裸指针，p1 变为 nullptr
// 注意：此时内存需由返回的指针手动管理，慎用
ImuData* raw_ptr = p1.release(); 
delete raw_ptr; // 演示需要，工程中尽量避免手动 delete
```
### 4.移动语义 std::move(旧独享指针)
```cpp
std::unique_ptr<ImuData> p3 = std::move(p2);
```

## 2.cpp

### 场景背景：
你正在编写一个 IMU 数据采集模块。为了保证数据安全，每个 IMU 数据包在采集生成后，应该由唯一的 unique_ptr 持有。后续处理模块需要“拿走”这个数据进行滤波。

### 任务要求：
请补全以下代码中的 TODO 部分。要求使用 unique_ptr 管理内存，严格体现所有权的转移，并使用 make_unique 创建对象。
