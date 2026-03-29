# 第四课：现代 C++ 的零成本抽象——消灭手写的 Rule of Five
前三课，我逼着你用原生指针（`double*`）手写了析构、移动构造、移动赋值。为什么？**因为不懂底层的苦，你就不知道现代 C++ 的甜。**
在真实的自动驾驶或机器人 IMU 算法工程中，如果我看到谁还在类里写 `new`、`delete[]` 和手动的 `noexcept` 移动构造，我会直接打回。这叫“手动管理内存”，是 C++98 时代的古老做派。
C++11 引入了**移动语义**，配合标准库容器，诞生了一个终极法则：**零成本抽象**。
## 1. 核心原理：编译器的“全自动挡”
只要你把类的成员从原生指针换成 `std::vector` 或 `std::unique_ptr`，你可以**把之前写的所有 Rule of Five 代码全部删掉**！
为什么？因为：
*   `std::vector` 和 `std::unique_ptr` 在标准库内部已经完美实现了移动构造和移动赋值（包括加了 `noexcept`）。
*   当你不在类里写这些函数时，C++ 编译器会**自动生成**默认的构造/析构/赋值/移动函数。
*   编译器自动生成的移动函数，会**自动去调用**每个成员变量（如 `vector`）自己的移动函数。
*   结果：你的类自动获得了最高效的指针窃取能力，代码量是 0，性能与手写裸指针一模一样。
## 2. 代码对比
**以前（苦逼的手动挡）：**
```cpp
double* accel_;
~ImuDataFrame() { delete[] accel_; } // 忘了就泄漏
ImuDataFrame(ImuDataFrame&& o) noexcept : accel_(o.accel_) { o.accel_ = nullptr; } // 繁琐
```
**现在（现代 C++ 的自动挡）：**
```cpp
std::vector<double> accel_;
// 不需要写析构！不需要写移动！编译器全自动搞定！
```
---
## 课后实操题
请基于你第二课/第三课的 `ImuDataFrame` 类进行彻底的**现代化重构**，**只贴代码，不要废话**：
**任务要求：**
1. **修改成员变量**：将 `double* accel_` 和 `double* gyro_` 替换为 `std::vector<double> accel_` 和 `std::vector<double> gyro_`。将 `int64_t* timestamp_` 替换为 `std::unique_ptr<int64_t> timestamp_`。（需要包含 `<memory>` 和 `<vector>`）
2. **大扫除**：**彻底删除**类内声明和类外实现中的：析构函数、拷贝构造、拷贝赋值、移动构造、移动赋值。一个字都不要留！
3. **修改普通构造函数**：直接用初始化列表给 `accel_` 和 `gyro_` 赋初值（如 `{ax, ay, az}`），用 `std::make_unique<int64_t>(timestamp)` 给 `timestamp_` 赋值。
4. **修改 `display` 函数**：因为现在是 `vector`，不需要手动判断指针是否为空，直接用 `accel_.empty()` 判断，用 `accel_[i]` 访问（如果是空 vector 你用 `[i]` 会越界，注意逻辑安全）。`timestamp_` 用 `if(timestamp_)` 判断即可。
5. **在 `main` 函数中**：
   * 执行与上一课完全相同的测试逻辑：构造 `frame1`，移动构造 `frame2`，用临时右值移动赋值给 `frame1`，自赋值 `frame3`。
   * 验证结果与上一课完全一致，且没有内存泄漏。
请提供你的代码解答。
