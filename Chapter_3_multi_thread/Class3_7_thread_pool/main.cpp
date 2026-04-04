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

struct ThreadPool {
  // 注意,这里的工作函数类型是void()但是不用担心不能传入参数,labmda表达式捕获!
  using Task = std::function<void()>;
  std::atomic<std::size_t> task_count_{0};  // 完成的任务计数(外部可见)

 private:
  std::vector<std::thread> workers_;  // 工作线程
  std::queue<Task> tasks_;            // 任务函数
  std::mutex mtx_;                    // 多个任务之间的锁
  std::condition_variable cv_;        // 生产者和消费者的通讯工具, 条件变量
  std::atomic<bool> stop_{false};     // 停止标志

  void worker_loop() {  // 单个线程的工作循环
    while (true) {
      Task task;  // 用于取得任务
      {
        // 消费者需要监听通知,所以用unique_lock
        std::unique_lock<std::mutex> lock(mtx_);
        // (记忆技巧: wait等待条件满足)
        // 当stop=ture的时候肯的不能等了,得检查要不要return了!
        // 当任务队列不为空时,也不能等了,得检查要不要干活了!
        cv_.wait(lock, [this]() { return stop_ || !tasks_.empty(); });
        if (stop_ && tasks_.empty()) {
          return;  // 终止运行的条件是: 既有停止位又任务为空
        }
        // 锁内获取共享资源(取一个任务队列的任务)
        task = std::move(tasks_.front());  // move移动最早的任务
        tasks_.pop();  // 队列只能手动弹出, 你都move了, 也必须弹出
      }
      // 锁外执行任务,充分利用多线程
      if (task) {       // 为了安全,检查不为nullptr
        task();         // 执行任务
        ++task_count_;  // 计数, 这是原子变量不用加锁
      }
    }
  }

 public:
  //  线程池构造: 创建n个工作线程,每个线程对应一个任务函数
  ThreadPool(std::size_t thread_num) {
    for (std::size_t i = 0; i < thread_num; ++i) {
      // emplace_back 原地构造, 连move开销都没有
      workers_.emplace_back([this]() { worker_loop(); });
    }
  }

  // 线程池析构: 对应销毁n个工作线程
  ~ThreadPool() {
    {  // 必须在获取互斥锁的保护下修改状态和发出通知：
      std::lock_guard<std::mutex> lock(mtx_);
      stop_ = true;      // 停止标志位
      cv_.notify_all();  // 通知全体
    }
    for (auto& t : workers_) {
      t.join();  // 等待所有线程都结束才行
    }
    // 最后报告一下!
    std::cout << "线程池总计处理任务数: " << task_count_ << "\n";
  }

  // 提交任务函数, 对外暴露的唯一接口
  void enqueue(Task task) {
    {
      std::lock_guard<std::mutex> lock(mtx_);  // 锁内修改共享数据
      tasks_.push(std::move(task));            // 移动任务函数到任务列表内
    }
    cv_.notify_one();  // 提醒一个消费者线程来处理
  }
};

struct ImuFrame {
  std::array<double, 3> accel{};
  std::array<double, 3> gyro{};
};

auto main(int argc, char** argv) -> int {
  const std::size_t FRAME_COUNT = 1000000;
  // 1. 生成需要处理的数据(生产者)
  std::vector<ImuFrame> imu_batch;
  imu_batch.reserve(FRAME_COUNT);
  for (std::size_t i = 0; i < FRAME_COUNT; ++i) {
    imu_batch.emplace_back(ImuFrame{.accel = {1.1 + i, 0, 0}, .gyro = {-1.1 - i, 0, 0}});
  }

  std::vector<ImuFrame> result_frame;
  result_frame.reserve(FRAME_COUNT);
  std::mutex res_mtx;
  // 2. 使用线程池来处理
  std::size_t THREAD_NUM = (argc > 1) ? std::atoi(argv[1]) : 1;
  ThreadPool thread_pool(THREAD_NUM);
  int frames = 0;
  for (auto& frame : imu_batch) {
    // 往池子里塞任务
    thread_pool.enqueue(
        // 移动捕获确保数据所有权完全交付, 数据安全
        [frame = std::move(frame), &result_frame, &res_mtx, &frames]() mutable {
          double mean_accel = (frame.accel[0] + frame.accel[1] + frame.accel[2]) / 3.0;
          frame.accel.fill(mean_accel);
          double mean_gyro = (frame.gyro[0] + frame.gyro[1] + frame.gyro[2]) / 3.0;
          frame.gyro.fill(mean_gyro);
          {  // 把结果move出来,但是顺序可能不对了就
            std::lock_guard res_lock(res_mtx);
            result_frame.push_back(std::move(frame));
          }
          std::this_thread::sleep_for(std::chrono::microseconds(50));  // 模拟处理耗时
          // std::cout << "线程 " << std::this_thread::get_id()
          // << " 处理第 " << ++frames << " 帧\n";
        });
  }

  // 3. 主线程必须等待子线程运行结束!!!
  while (thread_pool.task_count_ != FRAME_COUNT) {
    std::this_thread::yield();  // 让出CPU，不空转浪费(但是这不好,这是一个自旋锁!浪费性能!)
  }

  // 4. 打印相关结果
  std::cout << "总计发布任务数: " << FRAME_COUNT << "\n";
  std::cout << "总计运行线程数: " << THREAD_NUM << "\n";
  std::cout << "总结接收结果数: " << result_frame.size() << "\n";

  // std::cout << "打印滤波结果: \n";
  // for (const auto& frame : result_frame) {
  //   std::cout << frame.accel[0] << " ";
  // }
  // std::cout << "\n滤波结果\n";

  return 0;
}