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