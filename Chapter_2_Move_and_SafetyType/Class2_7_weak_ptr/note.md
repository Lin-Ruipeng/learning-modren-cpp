# 第八课：共享所有权与幽灵引用 —— `shared_ptr` 与 `weak_ptr` 的生死较量
在前面七节课里，我们都是 `unique_ptr` 的狂热信徒：“谁创建，谁销毁，绝对不共享”。
但在真实的工程中，有时候你就是**无法确定对象该在什么时候死**。
比如自动驾驶架构：一个“传感器数据中枢”和一个“视觉处理节点”。中枢需要向节点推送数据（中枢持有节点的指针），节点处理完异常时需要往中枢写日志（节点持有中枢的指针）。生命周期互相纠缠，谁也不能独占对方。
这时候 `unique_ptr` 就失效了，我们需要 `std::shared_ptr`（共享所有权的智能指针）。它通过内部的**引用计数**来管理内存：有多少个 `shared_ptr` 指向这块内存，计数就是几。计数降为 0，内存才释放。
## 1. 致命陷阱：循环引用
`shared_ptr` 有一个极其恐怖的副作用：**循环引用导致的内存泄漏**。
如果 A 持有 B 的 `shared_ptr`，B 又持有 A 的 `shared_ptr`。A 的计数因为有 B 指着，永远不为 0；B 的计数因为有 A 指着，也永远不为 0。两者死锁，内存永远泄漏，析构函数永远不会执行。
为了打破这个死局，C++ 引入了 `std::weak_ptr`（弱指针）。它像一个旁观者，**指向对象，但绝对不增加引用计数**。需要用的时候，通过 `.lock()` 方法临时“提升”为 `shared_ptr` 用一下，用完马上释放。
---
## 课后实操题
请创建一个全新的代码文件，**只贴代码，不要废话**。
**任务要求：**
1. 包含 `<iostream>` 和 `<memory>`。
2. 定义两个类：`SensorHub` 和 `VisionNode`。
3. **在 `VisionNode` 的 `private` 中**：声明一个 `std::weak_ptr<SensorHub> hub_;`（注意，这里直接用 weak_ptr）。
4. **在 `SensorHub` 的 `private` 中**：声明一个 `std::shared_ptr<VisionNode> node_;`。
5. 给两个类都写上公开的析构函数 `~SensorHub()` 和 `~VisionNode()`，函数体内只打印一句 `[Class Name] 被析构!`。
6. 给 `SensorHub` 写一个公开方法 `void bindNode(std::shared_ptr<VisionNode> node)`，把传进来的 `node` 赋值给 `node_`。
7. 给 `VisionNode` 写一个公开方法 `void bindHub(std::shared_ptr<SensorHub> hub)`，把传进来的 `hub` 赋值给 `hub_`。
8. 给 `VisionNode` 写一个方法 `void reportStatus()`，内部写：`if (auto h = hub_.lock()) { std::cout << "[Node] Hub 连接正常, 准备上报..." << std::endl; } else { std::cout << "[Node] Hub 已经挂了!" << std::endl; }`。
9. 在 `main` 函数中：
   * 用 `std::make_shared` 分别创建 `hub` 和 `node`。
   * 调用 `hub->bindNode(node);`
   * 调用 `node->bindHub(hub);`
   * 调用 `node->reportStatus();` 验证 `weak_ptr` 的 `.lock()` 能正常工作。
   * 打印 `"[main] 准备离开作用域..."`。
   * 在 `main` 的最后加一个大括号 `{}` 形成局部作用域，把上述所有代码（除了打印语句）扔进这个大括号里。
**思考（不用回答）**：观察终端输出，两个对象被正常析构了吗？如果把 `VisionNode` 里的 `weak_ptr` 换回 `shared_ptr`，还会析构吗？
请提供你的代码及终端输出。

# AI点评
满分。而且你做了一件极其聪明的事：你把测试代码写了两遍，外面一遍，里面套了一个 `{}` 局部作用域一遍。
你的终端输出里，藏着现代 C++ 内存销毁顺序的终极天机，看这里：
```text
离开局部作用域...
SensorHub 被析构!  <-- 为什么是 Hub 先死？
VisionNode 被析构! <-- 然后才是 Node 死？
```
**为什么是 Hub 先析构？**
在局部作用域结束时，栈上的变量按照**声明的逆序**销毁：先销毁 `node`，再销毁 `hub`。
1. `node` 离开作用域，Node 的强引用计数从 2 降到 1（因为 Hub 还在牵着它）。Node 没死。
2. `hub` 离开作用域，Hub 的强引用计数从 1 降到 0。**触发 Hub 析构！**
3. 在 Hub 的析构函数里，它的成员 `node_`（shared_ptr）被销毁，这导致 Node 的引用计数从 1 降到 0。
4. **触发 Node 析构！**
这就是 `weak_ptr` 的魅力。如果你把 `VisionNode` 里的 `weak_ptr` 改成 `shared_ptr`，这四行析构日志你一行都看不到，程序会安静地泄漏。
你亲手扼杀了一次极其凶险的内存泄漏。从今天起，看到“双向关联/观察者模式”，你的条件反射必须是 `weak_ptr`。
**本课判定：完美通过。**
请发送“**继续下一课**”。
