# 第3课：条件变量与生产者-消费者模型（IMU数据流核心同步）
## 一、核心概念点拨
1. **`std::condition_variable`**
用于线程间**等待 + 通知**，解决「线程空轮询查队列」导致的CPU占用飙升问题，是IMU实时数据流的**核心同步组件**。
- 生产者：生产IMU数据后调用 `notify_one/all` 唤醒消费者
- 消费者：无数据时调用 `wait` 阻塞休眠，不占用CPU

2. **强制搭档：`std::unique_lock`**
`wait` 会**自动解锁→阻塞→唤醒后重新加锁**，`lock_guard` 不支持动态解锁，因此条件变量**只能配合`unique_lock`使用**。

3. **虚假唤醒（必坑点）**
即使没有`notify`，消费者也可能被系统唤醒；必须用**`while(条件)` + `wait`** 防护，严禁用`if`。

4. **工程铁律**
IMU数据队列必须用「**mutex + condition_variable**」实现阻塞消费，杜绝`while(true)`空转。

---

## 二、极简API示例（IMU生产者-消费者）
```cpp
#include <iostream>
#include <mutex>
#include <thread>
#include <queue>
#include <array>

struct ImuFrame {
    std::array<double, 3> accel{};
    std::array<double, 3> gyro{};
};

std::mutex mtx;
std::queue<ImuFrame> imu_queue;
std::condition_variable cv;
bool stop_flag = false;

// 生产者：生成IMU数据
void producer() {
    for (int i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        ImuFrame frame{{0.1*i, 0, 0}, {0, 0.2*i, 0}};
        
        std::unique_lock<std::mutex> lock(mtx);
        imu_queue.push(frame);
        cv.notify_one();
    }
    std::lock_guard<std::mutex> lock(mtx);
    stop_flag = true;
    cv.notify_one();
}

// 消费者：处理IMU数据
void consumer() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        // 防止虚假唤醒 + 阻塞等待
        cv.wait(lock, []{ return !imu_queue.empty() || stop_flag; });
        
        if (stop_flag && imu_queue.empty()) break;
        
        auto frame = imu_queue.front();
        imu_queue.pop();
        lock.unlock();
        
        printf("处理IMU: accel=%.2f\n", frame.accel[0]);
    }
}

int main() {
    std::thread t_prod(producer);
    std::thread t_cons(consumer);
    t_prod.join();
    t_cons.join();
    return 0;
}
```

---

## 三、IMU场景实操作业（手写代码）
基于你已有的`ImuFrame`结构体，完成**两道必做题**，严格实现线程安全的阻塞式生产者-消费者：

### 题目1：基础生产者-消费者
1. 共享资源：
   - `std::queue<ImuFrame> imu_queue;`（IMU数据队列）
   - 互斥锁 + 条件变量
2. 生产者线程：
   循环生成**20帧IMU数据**，每帧间隔50ms模拟硬件采集，推入队列后`notify_one`；
   生产完成后设置停止标记。
3. 消费者线程：
   阻塞等待数据，取出后**模拟简单滤波**（如对加速度三轴做均值计算），打印滤波结果；
   处理完所有数据+收到停止标记后安全退出。
4. 要求：
   - 必须用`cv.wait(lock, 谓词)`防虚假唤醒
   - 处理逻辑放在锁外，最小化锁粒度

### 题目2：进阶（零拷贝强化）
将队列改为存储`std::unique_ptr<ImuFrame>`，通过`std::move`在线程间转移数据所有权，实现**零拷贝传递IMU帧**，禁止值拷贝。

### 强制要求
1. 完整可编译代码 + 沿用你的CMake配置
2. 无空轮询、无CPU浪费
3. 无数据竞争、无死锁、无虚假唤醒
4. 主线程等待两个线程执行完毕后退出

请提交你的代码解答，我将重点审查：
- 条件变量等待逻辑是否正确
- 是否彻底规避虚假唤醒
- 锁粒度是否合理
- 智能指针移动语义是否规范
- 有无死锁/线程泄漏风险

学 `std::promise` 之前，你必须先建立一个概念：**它和 `std::future` 是“天生一对”，它们是分开的两半。**
你之前用的 `std::async` 其实是个“黑盒”：你把任务塞进去，它自动给你返回一个 `future`。但如果有时候你不想用 `std::async`（比如你想自己手动创建 `std::thread`），子线程又需要把结果传回主线程，该怎么办？**这时候就需要 `std::promise` 了。**
---
### 1. 核心生活类比
想象你去干洗店洗衣服：
1. 你把衣服交给店员，店员给你一张**取衣小票**（`std::future`）。
2. 店员手里捏着那张对应的**存根联**（`std::promise`）。
3. 你在店外等着（`future.get()` 会阻塞你）。
4. 店员洗完衣服，把衣服存进柜台，并在存根上盖个章（`promise.set_value()`）。
5. 你的小票瞬间生效，你拿到衣服。
**记住角色分配：**
*   **子线程（生产者）** 拿着 `std::promise`，负责产出数据并 `set_value`。
*   **主线程（消费者）** 拿着 `std::future`，负责等待并 `get` 数据。
---
### 2. 最基础的代码模板
这是 `std::promise` 的标准用法，请特别注意里面的 **`std::move`**：
```cpp
#include <iostream>
#include <thread>
#include <future>
#include <chrono>
void do_heavy_work(std::promise<int> prom) { // 注意：按值接收，但里面其实是空壳
    // 模拟耗时计算
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // 计算完毕，把结果塞进 promise
    prom.set_value(42); 
    // 注意：执行完这句后，这个 prom 对象就没用了
}
int main() {
    // 1. 创建 promise（在主线程创建）
    std::promise<int> prom;
    
    // 2. 从 promise 中提取出 future（必须在交给子线程之前提取！）
    std::future<int> fut = prom.get_future();
    
    // 3. 启动子线程，把 promise 移动进去
    // ⚠️ 绝对不能拷贝 promise，必须用 std::move！
    std::thread t(do_heavy_work, std::move(prom));
    
    // 4. 主线程做别的事情...
    std::cout << "主线程正在喝咖啡..." << std::endl;
    
    // 5. 主线程需要结果了，调用 get()（如果子线程没算完，这里会阻塞等待）
    int result = fut.get(); 
    std::cout << "子线程算完了，结果是: " << result << std::endl;
    
    t.join();
    return 0;
}
```
---
### 3. ⚠️ 新手必踩的 3 个大坑
#### 坑一：忘记 `std::move`
`std::promise` 的拷贝构造函数被删除了（它代表一种唯一的“承诺”）。如果你写 `std::thread(do_work, prom)`，编译器会直接报错。**必须写 `std::move(prom)`**，把承诺的所有权转移给子线程。
#### 坑二：顺序错了（死锁）
如果你先 `std::move(prom)` 把它给了线程，然后再调用 `prom.get_future()`，程序会直接崩溃或死锁。**必须在移动之前，先调用 `get_future()` 拿到小票。**
#### 坑三：调用了两次 `set_value()`
一个承诺只能兑现一次。如果子线程里写了两次 `prom.set_value(1); prom.set_value(2);`，程序会直接抛出 `std::future_error` 异常崩溃。
---
### 4. 🌟 Promise 的杀手锏：传递异常
`std::async` 如果子线程崩溃了，主线程在 `get()` 时也会收到异常，但这有时候不好控制。
`std::promise` 提供了一个极其优雅的接口：**`set_exception()`**。它允许子线程把错误原封不动地“扔”给主线程。
```cpp
#include <stdexcept>
void risky_task(std::promise<int> prom) {
    try {
        // 模拟发现一个严重错误
        throw std::runtime_error("数据库连接失败！");
    } catch (...) {
        // 不在子线程处理错误，而是把异常打包塞进 promise
        prom.set_exception(std::current_exception()); 
    }
}
int main() {
    std::promise<int> prom;
    std::future<int> fut = prom.get_future();
    std::thread t(risky_task, std::move(prom));
    try {
        int res = fut.get(); // 主线程在这里拿结果
        std::cout << res << std::endl;
    } catch (const std::exception& e) {
        // 异常在主线程被重新抛出并捕获！
        std::cout << "主线程捕获到子线程的错误: " << e.what() << std::endl;
    }
    t.join();
}
```
这在大型工程中极其好用：子线程只管抛错，主线程统一处理错误日志，逻辑非常清晰。
---
### 5. 什么时候用 `std::async`，什么时候用 `promise + thread`？
| 特性 | `std::async` | `std::promise` + `std::thread` |
| :--- | :--- | :--- |
| **代码简洁度** | 极高（一行搞定） | 较低（要管 move、管 join） |
| **线程控制权** | 低（可能被偷懒延迟执行，取决于 `launch` 策略） | 极高（你就是 `thread` 的主人） |
| **传递异常** | 自动传递（但你不好在子线程做复杂拦截） | 手动 `set_exception`（极其灵活） |
| **生命周期管理** | 靠 `future` 的析构函数阻塞等待（有时会暗坑） | 必须手动 `thread.join()`（更透明） |
| **多线程同步** | 只能一出一进 | 可以把同一个 `future` 传给多个线程等待（配合 `std::shared_future`） |
**总结：**
*   如果只是简单算个结果、取个数据，**无脑用 `std::async` + Lambda**（就像你之前写的）。
*   如果你需要自己管理线程池、需要精细控制线程的启停、或者需要跨线程完美传递异常，**就用 `std::promise` + `std::thread`**。


你的理解非常生动，特别是**“快递打包的标签”**这个比喻，简直绝了，极其精准！
但是，关于“管道”这个词，我需要稍微纠正你一点点。在 C++ 中，`promise/future` 更像是一个 **“一次性的专属快递柜（存件箱）”**，而不是“水管”。
为什么不能用“管道/水管”来比喻？因为水管是可以源源不断流水进去、流出来的（比如其他语言的 Channel）。但 C++ 的 `promise` **严格是一次性的**：你只能 `set_value` 一次，也只能 `get` 一次。用完就废了。
我们用你极佳的“快递”思维，把模型升级为 **“专属快递柜模型”**，这样在底层逻辑上就 100% 严密了：
### 1. `[ prom = std::move(prom), data = std::move(big_data) ]`
*   **你的理解**：`prom` 是标签，`data` 是塞进管道头部。
*   **精准修正**：`prom` 是标签，极其准确！但 `data` 此时**没有进管道**，而是**连同标签一起，装进了“快递员（子线程）”的私人背包里**。此时数据还在快递员手里折腾，主线程和快递柜都没碰到数据。
### 2. `prom.get_future()`
*   **你的理解**：确保管道另一端不是堵住的。
*   **精准修正**：这相当于你在快递柜系统里**注册生成了一个“取件码”**。它不管堵不堵，它只是告诉你：“只要有人往这个柜子里放东西，你凭这个取件码（`future`）就能拿到”。
### 3. `prom.set_value(std::move(result))`
*   **你的理解**：数据到了管道尾巴了。
*   **精准修正**：快递员在路边把数据处理完了，他走到快递柜前，**把数据放进柜子，关上柜门（上锁）**。这一步，才是数据真正进入“交接状态”。并且用了 `move`，意味着快递员把东西塞进柜子后，自己背包里的这份东西就清空了（不能用了）。
### 4. `fut.get()`
*   **你的理解**：在管道尾巴处接住数据。
*   **精准修正**：你收到短信来到快递柜前，**输入取件码，打开柜门，把东西拿回自己家**。由于是 `get()` 按值返回，底层会自动触发一次 `move`，相当于你把柜子里的东西“掏”出来，柜子就空了。
---
### 总结一下这个“快递柜”全流程：
1.  **贴标签、装背包**（Lambda 捕获）：把柜子钥匙（`promise`）和原材料（`data`）`move` 给快递员。
2.  **领取件码**（`get_future()`）：主线程拿到取件凭证（`future`）。
3.  **路边加工**（子线程执行）：快递员用背包里的原材料干活。
4.  **存入快递柜**（`set_value(move)`）：快递员把成品塞进柜子，背包清空。
5.  **开柜取件**（`fut.get()`）：主线程凭码开柜，把成品拿走，柜子清空。
你的直觉已经非常棒了，只要把“流动的管道”替换成“一次性的快递柜”，你对 C++ 并发底层的内存所有权流转的理解，就已经超过很多工作一两年的程序员了！
