# 第7课：C++ 多线程工程最佳实践 + 极简IMU专用线程池
## 一、核心概念点拨（端侧/IMU项目必守铁律）
### 1. 死锁彻底规避法则
- 锁的获取顺序**全局统一**，绝不交叉加锁
- 最小化锁粒度，只锁共享资源，绝不锁耗时计算
- 禁止在锁内执行**sleep、IO、复杂计算**
- 优先用 `std::lock_guard`/`std::unique_lock` RAII锁，杜绝手动unlock

### 2. 线程性能黄金准则
- 禁止频繁创建/销毁线程（IMU流式数据场景开销爆炸）
- 线程数 ≈ CPU核心数（端侧开发板通常4/8核）
- 无锁优先于轻锁，轻锁优先于重锁
- 小数据连续内存 > 指针分散内存（你之前的判断完全正确）

### 3. 线程安全边界
- 锁保护**数据**，不是指针/智能指针
- 共享状态优先用 `std::atomic`，杜绝锁滥用
- 线程间传递数据**优先移动所有权**，少用共享

### 4. 线程池核心价值
IMU是**流式高频数据**，频繁创建`std::thread`会造成大量线程切换开销，**线程池复用线程**是端侧工程标配。
- 提前创建固定线程
- 任务入队，线程自动抢占执行
- 无线程创建/销毁开销，极致适配IMU数据处理

---

## 二、极简IMU线程池实现（C++11，无依赖，可直接上工程）
基于你已掌握的**mutex + condition_variable + 任务队列**实现，完全贴合你的知识体系。

---

## 三、IMU场景实操作业（线程池实战）
1. 实现一个**固定4线程**的极简线程池
2. 主线程生成**100帧IMU数据**
3. 向线程池提交**100个滤波任务**（单帧处理）
4. 线程池自动调度执行，主线程等待所有任务完成
5. 全程零拷贝、锁安全、无死锁、无资源泄漏

### 强制要求
- 线程池使用RAII锁+条件变量
- 任务用lambda封装，移动捕获IMU帧
- 线程数固定，复用执行
- 原子计数统计完成帧数，无锁安全

---

## 线程池基础骨架（给你参考，不替写核心逻辑）
```cpp
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

struct ThreadPool {
    using Task = std::function<void()>;

    ThreadPool(size_t thread_num) {
        for (size_t i=0; i<thread_num; ++i) {
            workers_.emplace_back([this] { worker_loop(); });
        }
    }

    ~ThreadPool() {
        stop_ = true;
        cv_.notify_all();
        for (auto& t : workers_) t.join();
    }

    // 提交任务
    void enqueue(Task task) {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            tasks_.push(std::move(task));
        }
        cv_.notify_one();
    }

private:
    void worker_loop() {
        while (true) {
            Task task;
            {
                std::unique_lock<std::mutex> lock(mtx_);
                cv_.wait(lock, [this] { return stop_ || !tasks_.empty(); });
                if (stop_ && tasks_.empty()) return;
                task = std::move(tasks_.front());
                tasks_.pop();
            }
            // 执行任务（锁外，不阻塞）
            if (task) task();
        }
    }

    std::vector<std::thread> workers_;
    std::queue<Task> tasks_;
    std::mutex mtx_;
    std::condition_variable cv_;
    std::atomic<bool> stop_{false};
};
```

---

