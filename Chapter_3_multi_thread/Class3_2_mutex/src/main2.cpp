// #define  TEST_NO_MUTEX // 仅在测试不加锁时取消注释!
// 本程序并非最佳实践! 最佳实践仍然是main1.cpp
// 只是为了学习 unique_lock + defer_lock
#include <array>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

struct ImuFrame {
  std::array<double, 3> accel;
  std::array<double, 3> gyro;
};

std::mutex mtx;  // 互斥锁

auto p_imu_data =
    std::make_shared<std::vector<ImuFrame>>();  // （共享 IMU 数据队列）

void insertImuFrameUnique(int& count, size_t times, int id) {
  for (size_t i = 0; i < times; ++i) {
    {
#ifndef TEST_NO_MUTEX
      // 临时构造不会加锁,手动加锁后访问
      std::unique_lock<std::mutex> lock(mtx, std::defer_lock);
      lock.lock();
#endif
      p_imu_data->push_back(
          ImuFrame{{1.0 * id, 0, 1.0 * i}, {0.1 * id, 0, 0.1 * i}});
      ++count;

#ifndef TEST_NO_MUTEX
      lock.unlock();
#endif
      std::cout << "线程 [" << id << "] 运行到第 " << i + 1 << " 次\n";
    }
  }
}

void insertImuFrameGuard(int& count, size_t times, int id) {
  for (size_t i = 0; i < times; ++i) {
    {
#ifndef TEST_NO_MUTEX
      // 临时构造加锁,出作用域自动解锁
      std::lock_guard<std::mutex> lock(mtx);
#endif
      p_imu_data->push_back(
          ImuFrame{{1.0 * id, 0, 1.0 * i}, {0.1 * id, 0, 0.1 * i}});
      ++count;
    }
    std::cout << "线程 [" << id << "] 运行到第 " << i + 1 << " 次\n";
  }
}

auto main() -> int {
  int imu_frame_count = 0;  // （已入队帧数）

  size_t times = 1000;
  p_imu_data->reserve(2 * times);

  std::thread t1(insertImuFrameUnique, std::ref(imu_frame_count), times, 1);
  std::thread t2(insertImuFrameGuard, std::ref(imu_frame_count), times, 2);

  t1.join();
  t2.join();

  std::cout << "运行结束: \n"
            << "预期的数据长度和计数量: " << 2 * times
            << "\n实际的数据长度: " << p_imu_data->size()
            << "\n实际的计数量  : " << imu_frame_count << "\n";
}