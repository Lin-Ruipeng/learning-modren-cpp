// 作业1 多线程同时处理数据(各线程独占数据)
#include <array>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>
struct ImuFrame {
  std::array<double, 3> accel{};
  std::array<double, 3> gyro{};
};

using ImuBatch = std::vector<ImuFrame>;

// 模拟均值滤波处理, 这里用&&强制交出所有权,保证了线程安全,代价是函数不够通用了
auto process_imu_batch(ImuBatch&& imu_batch, int job_i) -> ImuBatch {
  size_t count = 0;
  for (auto& imu_frame : imu_batch) {  // 别忘了引用&
    auto mean_accel = (imu_frame.accel[0] + imu_frame.accel[1] + imu_frame.accel[2]) / 3.0;
    imu_frame.accel.fill(mean_accel);
    auto mean_gyro = (imu_frame.gyro[0] + imu_frame.gyro[1] + imu_frame.gyro[2]) / 3.0;
    imu_frame.gyro.fill(mean_gyro);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));  // 模拟处理耗时
    std::cout << "线程 " << job_i << " 处理完成了第 " << ++count << " 帧数据\n";
  }
  std::cout << "报告: 线程 " << job_i << " , 总计处理了 " << count << " 帧数据\n";
  return imu_batch;  // 返回值会被move优化
}

auto main() -> int {
  const size_t FRAMES_SIZE = 100;
  // 1. 生成数据
  ImuBatch imu_batch;
  imu_batch.reserve(FRAMES_SIZE);
  for (size_t i = 0; i < FRAMES_SIZE; ++i) {
    imu_batch.push_back(ImuFrame{.accel = {i + 1.0, 0, 0}, .gyro = {-(1.0 + i), 0, 0}});
  }

  // 2. 拆分数据(make_move_iterator截取出一部分,但是要做好指针的管理)
  size_t mid = imu_batch.size() / 2;
  ImuBatch imu_batch_part1 = std::move(imu_batch);
  ImuBatch imu_batch_part2(std::make_move_iterator(imu_batch_part1.begin() + mid),
                           std::make_move_iterator(imu_batch_part1.end()));
  imu_batch_part1.resize(mid);
  // imu_batch: vector容器被move后会自动置空

  // 3. 启动多线程处理
  std::thread t_process_1([imu_batch = std::move(imu_batch_part1)]() mutable {
    process_imu_batch(std::move(imu_batch), 1);
  });
  std::thread t_process_2([imu_batch = std::move(imu_batch_part2)]() mutable {
    process_imu_batch(std::move(imu_batch), 2);
  });

  // 4. 等待阻塞!不能忘(除非用C++20的jthread)
  t_process_1.join();
  t_process_2.join();

  return 0;
}
