#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

// part 1 线程池
struct ThreadPool {
  using Task = std::function<void()>;
  std::atomic<std::size_t> task_count_{0};

 private:
  std::vector<std::thread> workers_;
  std::queue<Task> tasks_;
  std::mutex mtx_;
  std::condition_variable cv_;
  std::atomic<bool> stop_{false};

  void worker_loop() {
    while (true) {
      Task task;
      {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this]() { return stop_ || !tasks_.empty(); });
        if (stop_ && tasks_.empty()) {
          return;
        }

        task = std::move(tasks_.front());
        tasks_.pop();
      }

      if (task) {
        // std::cout << "正在处理一帧数据\n";
        task();
        ++task_count_;
      }
    }
  }

 public:
  explicit ThreadPool(std::size_t thread_num) {
    for (std::size_t i = 0; i < thread_num; ++i) {
      workers_.emplace_back([this]() { worker_loop(); });
    }
  }

  ~ThreadPool() {
    {
      std::lock_guard<std::mutex> lock(mtx_);
      stop_ = true;
      cv_.notify_all();
    }
    for (auto& t : workers_) {
      t.join();
    }
    // std::cout << "\n[Thread Pool] 总计处理任务: " << task_count_ << "\n";
  }

  void enqueue(Task task) {
    {
      std::lock_guard<std::mutex> lock(mtx_);
      tasks_.push(std::move(task));
    }
    cv_.notify_one();
  }
};

// part 2 IMU 数据结构
struct ImuFrame {
  std::array<double, 3> accel{};  // 加速度
  std::array<double, 3> gyro{};   // 陀螺仪
  std::size_t index{};            // 帧序号

  // 滤波函数, 原地计算 0拷贝
  void filtering() {
    double accel_mean = (accel[0] + accel[1] + accel[2]) / 3.0;
    double gyro_mean = (gyro[0] + gyro[1] + gyro[2]) / 3.0;
    accel.fill(accel_mean);
    gyro.fill(gyro_mean);
  }
};

// part 3 线程安全结果管理
struct ResultManager {
  std::vector<ImuFrame> results;
  std::mutex mtx;

  // 线程安全写入 移动插入 0拷贝
  void add_result(ImuFrame&& frame) {
    std::lock_guard<std::mutex> lock(mtx);
    results.push_back(std::move(frame));
  }

  std::size_t get_result_count() {
    std::lock_guard<std::mutex> lock(mtx);
    return results.size();
  }
};

// part 4 主函数
auto main(int argc, char** argv) -> int {
  // 配置参数
  const std::size_t TOTAL_FRAMES = 10000000;                         // 总采集帧数
  const std::size_t THREAD_NUM = argc > 1 ? std::atoi(argv[1]) : 4;  // 线程数

  // 1. 初始化
  ThreadPool pool(THREAD_NUM);
  ResultManager result_mgr;
  auto start_time = std::chrono::steady_clock::now();

  // 2. 模拟IMU数据采集 + 提交线程池处理
  std::cout << "[main thread] 开始采集IMU数据, 线程数: " << THREAD_NUM << "\n";
  for (std::size_t i = 0; i < TOTAL_FRAMES; ++i) {
    // 构造数据帧, 模拟数据采集
    ImuFrame frame{
        .accel = {1.0 + i, 2.0 + i, 3.0 + i}, .gyro = {-1.0 - i, -2.0 - i, -3.0 - i}, .index = i};

    // 采集到数据就提交任务
    pool.enqueue([frame = std::move(frame), &result_mgr]() mutable {
      // 数据处理: 滤波
      frame.filtering();
      // 线程安全写入结果
      result_mgr.add_result(std::move(frame));
    });
  }

  // 3. 主线程等待所有任务完成
  std::cout << "[main thread] 等待所有IMU数据处理完成...\n";
  while (pool.task_count_ != TOTAL_FRAMES) {
    // std::cout << "已经处理数据帧数: " << pool.task_count_ << "\n";
    std::this_thread::yield();
  }

  // 4. 性能统计与输出
  auto end_time = std::chrono::steady_clock::now();
  auto cost = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

  std::cout << "=========================================\n";
  std::cout << "[综合项目] 运行完成！\n";
  std::cout << "总处理帧数：" << TOTAL_FRAMES << "\n";
  std::cout << "有效结果数：" << result_mgr.get_result_count() << "\n";
  std::cout << "工作线程数：" << THREAD_NUM << "\n";
  std::cout << "总耗时：" << cost << " ms\n";
  std::cout << "帧率：" << (TOTAL_FRAMES * 1000.0 / cost) << " 帧/秒\n";
  std::cout << "=========================================\n";

  return 0;
}
