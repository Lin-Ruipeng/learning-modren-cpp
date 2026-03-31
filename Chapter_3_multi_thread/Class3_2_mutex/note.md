# 第2课：数据竞争与互斥锁（std::mutex / lock_guard / unique_lock）
## 一、核心概念点拨
1. **数据竞争（竞态条件）**
多个线程**同时读写同一块共享内存**，且无任何同步措施，最终结果未定义、完全随机。
在IMU场景中：多线程同时写入IMU数据队列、同时累加处理帧数，必然出现数据错乱、丢帧、数值异常。

2. **std::mutex**
互斥锁，用于保护**临界区**（共享资源访问代码段），保证同一时刻只有一个线程进入临界区。

3. **std::lock_guard**
- RAII 自动锁，**构造加锁、析构解锁**
- 无手动 lock/unlock，**简单、安全、无灵活性**
- 适合：临界区代码短、无需中途解锁的场景

4. **std::unique_lock**
- 同样 RAII，但**更灵活**：支持手动 lock/unlock、延迟加锁、锁所有权转移
- 是后续条件变量的**强制搭档**
- 适合：复杂同步、需要临时解锁、条件等待的IMU数据流场景

5. **工程铁律**
IMU 共享数据（队列、计数、状态位）必须加锁保护，**严禁裸奔读写**。

---

## 二、极简API示例（IMU线程安全计数）
```cpp
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

std::mutex g_imu_mutex;
int g_imu_processed = 0;  // 共享：已处理IMU帧数

void add_processed_count() {
    // 自动加锁/解锁，保护临界区
    std::lock_guard<std::mutex> lock(g_imu_mutex);
    ++g_imu_processed;
}

void worker(int id) {
    for (int i = 0; i < 10; ++i) {
        add_processed_count();
        std::cout << "线程" << id << "处理1帧IMU数据\n";
    }
}

int main() {
    std::thread t1(worker, 1);
    std::thread t2(worker, 2);

    t1.join();
    t2.join();

    std::cout << "总处理帧数: " << g_imu_processed << std::endl;
    return 0;
}
```

---

## 三、IMU场景实操作业（手写代码）
基于你上一课的 `ImuFrame` 结构体，完成**两道必做题**，必须出现**共享资源竞争 + 锁修复**：

### 题目1：制造并修复数据竞争
1. 定义**共享全局变量**：
   - `std::vector<ImuFrame> g_imu_shared_queue;`（共享IMU数据队列）
   - `int g_imu_frame_count = 0;`（已入队帧数）
2. 创建**2个子线程**，同时向该共享队列插入 ImuFrame 数据，**先不加锁**，运行观察乱序/崩溃/数据丢失。
3. 用 `std::mutex + std::lock_guard` 修复线程安全，保证队列插入、计数累加完全正确。

### 题目2：对比 lock_guard / unique_lock
1. 在上题基础上，将其中一个线程的锁改为 `std::unique_lock`，实现：
   - 先**延迟加锁**（构造时不加锁，手动lock）
   - 访问完共享队列后**手动解锁**，再执行打印操作（减少锁持有时间，提升IMU实时性）
2. 依然保证线程安全，不许出现数据竞争。

### 强制要求
1. 沿用你的 `ImuFrame` 结构体，保持工程风格一致
2. 提供完整可编译代码 + CMakeLists.txt
3. 必须体现：**无锁错乱 → 加锁修复** 的对比逻辑
4. 主线程等待所有子线程结束后，打印最终队列大小与计数，验证正确性

请提交你的代码解答，我将重点审查：
- 是否彻底消除数据竞争
- 锁粒度是否合理（是否出现无脑大锁）
- unique_lock 使用是否规范
- 有无死锁风险