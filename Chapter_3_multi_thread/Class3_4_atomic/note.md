# 第4课：std::atomic 原子操作与无锁并发（IMU状态/计数高性能优化）
## 一、核心概念点拨
1. **std::atomic 原子类型**
C++11提供的**无锁线程安全**类型，通过CPU原子指令保证**读写/加减操作不可分割**，完全避免数据竞争，**不需要mutex**。
- 原子操作：指令级原子性，操作系统/CPU保证不会被线程切换打断
- 无锁：无锁竞争、无死锁风险、开销远小于互斥锁
- 适用场景：**简单共享变量**（IMU帧计数、采集启停标志、丢包统计）

2. **核心边界（必记）**
❌ 绝对不能用`std::atomic`保护队列、vector、复杂数据结构
✅ 只适合：单个数值、单个标志位（int/bool/指针）

3. **默认内存序**
`std::atomic`默认使用`memory_order_seq_cst`（全局顺序一致），工程开发直接用即可，无需手动修改。

4. **IMU场景价值**
替代mutex实现**帧计数、启停控制、实时状态标记**，大幅降低高频IMU数据下的锁开销，端侧性能提升明显。

---

## 二、极简API示例（IMU无锁计数+启停）
```cpp
#include <iostream>
#include <thread>
#include <atomic>

// 原子计数：无锁线程安全
std::atomic<int> g_imu_processed{0};
// 原子启停标志：无锁控制线程
std::atomic<bool> g_stop{false};

void imu_worker(int id) {
    // 原子读：判断是否停止
    while (!g_stop) {
        // 原子自增：无锁安全
        g_imu_processed.fetch_add(1, std::memory_order_relaxed);
        printf("线程%d处理1帧IMU，总帧数：%d\n", id, g_imu_processed.load());
    }
}

int main() {
    std::thread t1(imu_worker, 1);
    std::thread t2(imu_worker, 2);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // 原子写：停止线程
    g_stop = true;

    t1.join();
    t2.join();
    std::cout << "最终处理总帧数：" << g_imu_processed << std::endl;
    return 0;
}
```

---

## 三、IMU场景实操作业（无锁改造实战）
基于你**第3课的生产者-消费者零拷贝代码**，完成**两道必做题**，实现**无锁线程安全**：

### 题目1：无锁IMU帧计数
1. 移除所有用于保护`imu_frame_count`的mutex/lock
2. 使用`std::atomic<size_t>`替代普通计数变量
3. 生产者/消费者**无锁**安全读写计数，最终统计结果必须准确
4. 保留原有队列、条件变量逻辑（复杂结构仍需锁，仅计数无锁）

### 题目2：无锁IMU采集启停控制
1. 将`stop_flag`从普通bool改为`std::atomic<bool>`
2. 实现**无锁**线程安全的启停控制，生产者、消费者直接原子读写标志
3. 保证线程正常退出，无死循环、无泄漏
4. 禁止对启停标志加任何锁

### 强制要求
1. 完整可编译代码，沿用`unique_ptr<ImuFrame>`零拷贝队列
2. 仅对**计数、启停标志**使用`std::atomic`无锁优化
3. 队列操作仍保留mutex+condition_variable（复杂结构必须锁）
4. 运行结果：计数准确、线程安全退出、无数据竞争

请提交你的代码解答，我将重点审查：
- `std::atomic`使用是否规范，有无误用
- 无锁变量是否彻底脱离mutex保护
- 原子读写/自增操作是否正确
- 整体线程安全、无死锁、无空轮询