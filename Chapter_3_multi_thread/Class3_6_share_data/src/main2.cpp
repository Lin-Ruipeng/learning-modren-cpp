// 题目 2：shared_ptr 多消费者共享(因为有锁,其实不需要使用智能指针)
// 准确的说, 指针保证的是数据的生命周期安全, 但是锁是解决多线程数据竞争
#include <array>
#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

struct ImuFrame {
  std::array<double, 3> accel{};
  std::array<double, 3> gyro{};
};

using ImuBatch = std::vector<ImuFrame>;

// 共享读取数据, 这里指针用引用捕获, 提高性能, labmda捕获时则用值捕获保证安全
void print_imu_batch(const std::shared_ptr<ImuBatch> &p_imu_batch, std::mutex &mtx, int job_i) {
  std::size_t count = 0;
  for (const auto &imu_frame : *p_imu_batch) {
    std::cout << "线程: " << job_i << " 读取到第 " << ++count << " 帧数据, 内容为";
    // 访问数据需要锁, 尽可能缩小锁的范围
    {
      std::lock_guard<std::mutex> lock(mtx);
      std::cout << imu_frame.accel[0];
    }
    std::cout << "\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(50));  // 模拟处理耗时
  }
  std::cout << "报告: 线程 " << job_i << " 总计读取数据: " << count << "帧\n";
}

auto main() -> int {
  const size_t FRAMES_SIZE = 100;
  // 1. 生成数据
  auto p_imu_batch = std::make_shared<ImuBatch>();
  p_imu_batch->reserve(FRAMES_SIZE);
  for (size_t i = 0; i < FRAMES_SIZE; ++i) {
    p_imu_batch->push_back(ImuFrame{.accel = {i + 1.0, 0, 0}, .gyro = {-(1.0 + i), 0, 0}});
  }

  // 2. 需要一个锁
  std::mutex mtx;

  // 3. 多线程共享数据(数据用拷贝方式捕获,可以增加引用计数保证生命周期安全,锁必须使用引用捕获!)
  std::thread t_print_1([p_imu_batch, &mtx]() { print_imu_batch(p_imu_batch, mtx, 1); });
  std::thread t_print_2([p_imu_batch, &mtx]() { print_imu_batch(p_imu_batch, mtx, 2); });
  std::thread t_print_3([p_imu_batch, &mtx]() { print_imu_batch(p_imu_batch, mtx, 3); });

  // 4. 不能忘了等待完成
  t_print_1.join();
  t_print_2.join();
  t_print_3.join();

  return 0;
}