// 题目 1：基础生产者 - 消费者
#define DATA_FRAME_LENGTH 301
#include <array>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

struct ImuFrame {
  std::array<double, 3> accel{};
  std::array<double, 3> gyro{};
};

void producer(std::queue<ImuFrame> &imu_queue, std::mutex &mtx, std::condition_variable &cv,
              bool &stop_flag) {
  // 生产数据
  for (size_t i = 0; i < DATA_FRAME_LENGTH; ++i) {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));           // 睡50ms
    ImuFrame frame{.accel = {0, 0.1 * i, 0}, .gyro = {1, -0.1 * i, -1}};  // 锁之外准备好数据
    {
      std::lock_guard<std::mutex> lock(mtx);  // 加锁后访问共享资源
      imu_queue.push(std::move(frame));       // 移动,性能更优
    }
    cv.notify_one();                             // 发送信号唤醒消费者处理数据
    std::cout << "\n生产数据, 序号: " << i + 1;  // 锁内打印性能不好
  }
  // 生产结束!需要通过一个变量额外确认!
  {
    std::lock_guard<std::mutex> lock(mtx);  // 先上锁
    stop_flag = true;                       // 访问共享资源, 再离开作用域自动解锁
  }
  cv.notify_all();  // 通知所有线程这个flag变了
}

// 模拟简单滤波函数
void simpleFilter(ImuFrame &imu_frame) {
  auto [ax, ay, az] = imu_frame.accel;
  auto mean_xyz = (ax + ay + az) / 3.0;  // 不要/3,可能会隐式截断!
  imu_frame.accel = {mean_xyz, mean_xyz, mean_xyz};
}

void consumer(std::queue<ImuFrame> &imu_queue, std::mutex &mtx, std::condition_variable &cv,
              bool &stop_flag) {
  // 无限循环消费数据
  size_t count = 0;  // 记录处理次数
  while (true) {
    // 消费者需要调用wait,所以必须使用多一个bit的unique_lock
    std::unique_lock<std::mutex> lock(mtx);
    // wait检查要不要等待, 条件为真时立马返回执行(防止虚假唤醒 + 阻塞等待)
    cv.wait(lock, [&] { return !imu_queue.empty() || stop_flag; });
    if (stop_flag && imu_queue.empty()) {
      break;  // 没数据又确定停止了就退出了
    }
    auto frame = std::move(imu_queue.front());  // 取出最头部的数据(move性能更优)
    imu_queue.pop();                            // 移除数据
    lock.unlock();                              // 访问结束,解锁
    simpleFilter(frame);                        // 锁外进行数据处理!
    std::cout << "\n消费数据,滤波结果: " << frame.accel[0];
    ++count;  // 记录消费次数
  }
  // 消费结束
  std::cout << "\n消费数据总量: " << count << '\n';
}

auto main() -> int {
  std::queue<ImuFrame> imu_queue;  // IMU 数据队列
  std::mutex mtx;                  // 互斥锁保护队列
  std::condition_variable cv;      // 条件变量用于通知消费者
  bool stop_flag = false;          // 最终停止变量

  std::thread t_prod(producer, std::ref(imu_queue), std::ref(mtx), std::ref(cv),
                     std::ref(stop_flag));
  std::thread t_cons(consumer, std::ref(imu_queue), std::ref(mtx), std::ref(cv),
                     std::ref(stop_flag));
  t_prod.join();
  t_cons.join();
  return 0;
}
