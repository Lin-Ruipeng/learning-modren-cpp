# 第十课：终结虚函数 —— `std::variant` 与静态多态分发
在传统的面向对象编程中，处理不同类型的传感器数据（IMU、GPS、轮速计），我们会写一个基类 `SensorData`，然后让它们继承，再写一堆 `virtual void process()`。
在 C++ 中，**虚函数是性能毒药**。它强制数据必须在堆上分配（`new`），引发内存碎片；它引入了虚表指针查找，破坏 CPU 流水线和分支预测；最要命的是，不同类型的数据散落在堆的不同角落，彻底破坏了**缓存局部性**。
C++17 引入了 **`std::variant`**。它是一个“类型安全的联合体”，直接把数据**生吞进栈内存**里（大小等于其最大类型加上一小块标志位）。配合 `std::visit`，你可以实现**零堆分配、绝对缓存友好**的静态多态。
## 1. 核心机制：`std::visit` 与 `if constexpr`
`std::visit` 接受一个“访问者”和一个 `variant`。在 C++17 中，我们最喜欢用**泛型 Lambda 结合 `if constexpr`** 来做分发：编译器会为每一种可能的类型生成一份特化代码，运行时没有任何虚函数开销，直接跳转。
---
## 课后实操题
请创建一个全新的代码文件，**只贴代码，不要废话**。
**任务要求：**
1. 引入 `<iostream>`, `<variant>`, `<vector>`, `<string>`, `<type_traits>`。
2. 定义两个纯数据结构体（不需要任何成员函数）：
   *   `struct ImuFrame { double accel_x; double accel_y; };`
   *   `struct GpsFrame { double lat; double lon; };`
3. 定义类型别名：`using SensorData = std::variant<ImuFrame, GpsFrame>;`
4. 在 `main` 函数中，创建一个 `std::vector<SensorData> buffer;`。
5. 往 `buffer` 里 `push_back` 一个 `ImuFrame{1.0, 2.0}` 和一个 `GpsFrame{30.0, 120.0}`。（注意：直接传大括号，会发生隐式的 `variant` 构造）。
6. 编写一个 `for (const auto& data : buffer)` 循环。
7. 在循环体内，调用 `std::visit`，传入一个泛型 Lambda：
   *   Lambda 的参数是 `auto&& arg`。
   *   在 Lambda 内部，使用 `using T = std::decay_t<decltype(arg)>;` 获取真实类型。
   *   使用 `if constexpr (std::is_same_v<T, ImuFrame>)` 判断类型。
   *   如果是 IMU，打印 `[IMU] ax: ... ay: ...`。
   *   使用 `else if constexpr (std::is_same_v<T, GpsFrame>)` 判断。
   *   如果是 GPS，打印 `[GPS] lat: ... lon: ...`。
请提供你的代码解答。


既然你有 Rust 背景，那我们就不兜圈子了，直接一针见血：
## 核心思想：`std::variant` 就是 C++ 版的 Rust `enum`
在 Rust 中，你一定写过这种代码：
```rust
enum SensorData {
    Imu(ImuFrame),
    Gps(GpsFrame),
    // 编译器知道它最大有多大，直接在栈上分配这块固定大小的空间
}
let data = SensorData::Imu(ImuFrame { x: 1.0, y: 2.0 });
// 通过 match 进行模式匹配，绝对安全
match data {
    SensorData::Imu(frame) => println!("IMU: {}", frame.x),
    SensorData::Gps(frame) => println!("GPS: {}", frame.lat),
}
```
**在 C++ 中，`std::variant<ImuFrame, GpsFrame>` 干的就是完全一样的事！**
它是一个**带标签的联合体**。
*   它的大小在编译期就固定了：等于 `max(sizeof(ImuFrame), sizeof(GpsFrame))` + 几个字节（用来存当前是哪个类型的“标签”）。
*   在任意时刻，它**只能装其中一个类型**。你赋值了 `ImuFrame`，它里面的 `GpsFrame` 就会被销毁。
*   它**绝对安全**。不像 C 语言的 `union`，你存了 `int` 却当 `float` 读，编译器不管你。`variant` 会记住当前装了什么，如果你用错了类型去取，它会直接抛出异常（或者触发未定义行为的处理机制），绝不让内存悄悄损坏。
---
## C++ 的痛点：如何取出数据？
在 Rust 里，取出数据非常优雅，用 `match`。
但在 C++ 中，怎么知道里面装了什么，并且安全地取出来？
### 错误的示范：`std::get` (相当于强行 `unwrap` 且指定类型)
```cpp
std::variant<int, double> v = 42;
// 如果你不小心写了 std::get<double>(v)，直接抛异常崩溃！
std::cout << std::get<int>(v) << std::endl; // 必须明确指定类型，很死板
```
### 丑陋的示范：`std::holds_alternative` (相当于写一堆 if-else)
```cpp
if (std::holds_alternative<int>(v)) {
    std::cout << std::get<int>(v);
} else if (std::holds_alternative<double>(v)) {
    std::cout << std::get<double>(v);
}
```
类型一多，这种代码会让你疯掉。
### 终极的示范：`std::visit` + 泛型 Lambda (相当于 C++ 的 `match`)
这是 C++17 处理 `variant` 的最高级形态。`std::visit` 会去看那个“标签”，然后把你写好的函数（Lambda）“丢进去”执行。
```cpp
std::variant<int, double> v = 3.14;
// 1. auto&& arg 意思是：不管你里面装了什么类型，我先用一个万能引用接住它
std::visit([](auto&& arg) {
    std::cout << arg << std::endl;
}, v);
```
但这还不够！因为有时候不同的类型，需要做**不同的处理**（就像 Rust 的不同分支）。这时候，就要请出 C++ 的黑魔法组合：**`if constexpr` + `std::is_same_v`**。
```cpp
std::variant<int, double> v = 42;
std::visit([](auto&& arg) {
    // 1. decay_t 剥离掉引用和 const，拿到最纯粹的真实类型 T
    using T = std::decay_t<decltype(arg)>;
    
    // 2. if constexpr 是编译期判断！
    // 如果 T 是 int，下面这个分支会被编译；如果不是 int，下面这个分支在编译时会被直接丢掉！
    if constexpr (std::is_same_v<T, int>) {
        std::cout << "这是个整数: " << arg << std::endl;
    } 
    // 3. 同理，匹配 double
    else if constexpr (std::is_same_v<T, double>) {
        std::cout << "这是个浮点数: " << arg << std::endl;
    }
}, v);
```
## 总结映射表
| Rust 概念 | C++ 对应概念 | 作用 |
| :--- | :--- | :--- |
| `enum Data { A, B }` | `std::variant<A, B>` | 栈上的安全类型集合 |
| `Data::A(val)` | 直接赋值 `variant = A{val}` | 存入数据 |
| `match data { A(x) => ... }` | `std::visit([](auto&& x) { if constexpr(...) ... }, variant)` | 编译期安全分发处理 |
**核心心法**：`std::visit` 里的 Lambda 会被编译器根据 `variant` 的所有可能类型，在**编译期**实例化出多份代码。运行时，它看一眼标签，直接跳到对应的代码执行。没有任何虚表查表，没有任何堆分配，性能与手写 `if-else` 完全一致，但安全性达到了 Rust 的级别。

你提到“不使用 variant，我相当于需要写泛型函数”。这里有一个陷阱：**泛型函数（模板）本身确实不需要查表，性能极好，但是你不能把不同类型的东西放进同一个数组里！**
在 Rust 里你可以毫无压力地写：
```rust
let buffer: Vec<SensorData> = vec![SensorData::Imu(...), SensorData::Gps(...)];
```
但在没有 `variant` 的旧时代 C++ 里，你如果写 `std::vector<ImuFrame>`，你就绝对塞不进去 `GpsFrame`。**想要把不同大小的对象塞进同一个连续内存的 `vector` 里，旧时代 C++ 只有两条路：**
1. **万恶之源（虚函数）**：`std::vector<std::unique_ptr<BaseSensor>>`。
   * 你被迫写一个基类，里面写 `virtual void process() = 0;`。
   * 这导致：你的 `ImuFrame` 和 `GpsFrame` **必须被 `new` 在堆上**（因为指针大小固定为 8 字节，但对象大小不一，只能存地址）。
   * 运行时：CPU 先读指针指向的内存（可能 Cache Miss），再查虚表（再次 Cache Miss），再跳转到函数执行。**性能惨案。**
2. **C 语言的邪道（`void*` 或 `union`）**：`std::vector<CustomUnion>`。
   * 自己手写一个 `union { ImuFrame imu; GpsFrame gps; int type_tag; };`。
   * 这虽然能绕开虚函数查表，也保住了栈内存，但是**完全没有类型安全**。你如果 `type_tag` 记错了，把 `GpsFrame` 的内存当 `ImuFrame` 读，程序直接产生玄学 Bug，编译器一声不吭。
**所以，`std::variant` 的真正伟大之处，是它同时解决了这三个问题：**
1. 它允许你写 `std::vector<variant<...>>`（解决异构容器问题）。
2. 它在底层实质上就是那个 `union` + `type_tag`（解决堆分配问题，数据全在栈上）。
3. 它在编译期严格限制你只能用匹配的类型去读（解决类型安全问题）。
**关于 `if constexpr` 的绝杀：**
你说的“只是一个分支代码，不需要查表”，非常正确，但还可以更极致：
在最终生成的机器码（汇编）里，**它甚至连 `if` 判断都不存在！**
编译器看到 `if constexpr (std::is_same_v<T, int>)` 时，如果推导出 `T` 是 `int`，它就会**直接把 `else` 分支的代码删掉（根本不编译）**；如果推导出是 `double`，它就把 `if` 分支删掉。
最终你的二进制文件里，相当于有两段完全独立、干干净净的代码，`std::visit` 只是根据标签做了一个极其简单的跳转（连预测都省了），这就叫**静态分发**。
总结一下你的顿悟：
**抛弃虚函数（动态分发） -> 拥抱 variant 存数据 -> 拥抱 `std::visit` + `if constexpr`（静态分发） = 达到 Rust `enum + match` 同等的安全性与 C 语言裸写 `union` 同等的极致性能。**
