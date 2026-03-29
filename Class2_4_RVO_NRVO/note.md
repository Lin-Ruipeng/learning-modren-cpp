# 第五课：隐式移动与返回值优化（RVO/NRVO 的底层真相）
在前四课中，我们一直在用 `std::move()` 来“显式”地触发移动。但移动语义最强大的地方，在于它的**隐式触发**。
在 C++98 时代，有一个著名的毒瘤设计：**坚决不要返回局部对象**。老一代 C++ 程序员会被教导写成这样：
```cpp
// C++98 的恐惧
void generateImuData(ImuDataFrame& out_frame) { 
    // 填充 out_frame
}
```
因为他们害怕 `return` 局部对象会触发一次昂贵的深拷贝。
到了现代 C++，这个恐惧被彻底终结。编译器会像幽灵一样，在背后帮你做极致的性能优化。
## 1. 编译器的两道免死金牌
当你在函数里写 `return local_obj;` 时，编译器会按顺序尝试两招：
*   **第一招：RVO / NRVO (返回值优化)**。
    编译器直接把局部变量 `local_obj` 的内存地址，安排在调用者预留好的内存上。**连移动构造函数都不调用！直接 zero cost（零成本）！**
*   **第二招：隐式移动**。
    如果你的代码逻辑太复杂（比如多个分支返回不同的局部变量），导致编译器无法执行 RVO。C++11 标准规定：**编译器必须自动把 `return local_obj;` 当作 `return std::move(local_obj);` 来处理。** 它会自动调用移动构造，绝不可能退化成拷贝构造。
## 2. 核心痛点：如何观察“隐式移动”？
在第四课中，你把移动构造写成了 `= default`。这导致一个结果：当编译器执行移动构造时，**你完全看不到任何日志**，你会怀疑它到底有没有移动。
为了验证“隐式移动”，我们必须把移动构造的手写实现拿回来，**仅仅是为了打印一行日志**。
---
## 课后实操题
请基于第四课的代码进行修改，**只贴代码，不要废话**：
**任务要求：**
1. **恢复手写移动构造**：将类内的 `ImuDataFrame(ImuDataFrame &&) = default;` 改为在 `private` 下面声明，并在类外实现。实现内容非常简单：在初始化列表中调用各个成员的移动构造（`accel_(std::move(other.accel_))`），在函数体内打印一行 `[Class info] 移动构造被调用!`。（移动赋值保持 `= default` 即可，不用管它）。
2. **编写工厂函数**：在类外写一个全局函数：
   ```cpp
   ImuDataFrame createImuFrame(bool use_high_precision)
   ```
   *   如果 `use_high_precision` 为 `true`，在函数内构造一个局部变量 `high_res_frame(...)`，并 `return high_res_frame;`。
   *   如果为 `false`，构造另一个局部变量 `low_res_frame(...)`，并 `return low_res_frame;`。
3. **在 `main` 函数中**：
   *   打印一句 `=== 测试 RVO/隐式移动 ===`。
   *   调用 `auto frame = createImuFrame(true);`。
   *   调用 `frame.display();` 验证数据正确。
**思考题（不用回答，看输出结果去体会）**：
观察终端输出，在 `createImuFrame` 返回时，你看到了几次 `[Class info] 对象构造!`？你看到了几次 `[Class info] 移动构造被调用!`？思考一下为什么是这个结果。
请提供你的代码解答。
