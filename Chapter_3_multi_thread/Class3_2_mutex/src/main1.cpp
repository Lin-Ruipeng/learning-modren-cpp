#include <array>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

struct ImuFrame {
  std::array<double, 3> accel;
  std::array<double, 3> gyro;
};

// 接收普通引用（std::ref）即可，因为主线程的 join() 保证了数据绝对存活
void insertImuFrame(std::vector<ImuFrame>& data_buffer, std::mutex& mtx,
                    int& count, size_t times, int id) {
  for (size_t i = 0; i < times; ++i) {
    {
      std::lock_guard<std::mutex> lock(mtx);
      data_buffer.push_back(
          ImuFrame{{1.0 * id, 0, 1.0 * i}, {0.1 * id, 0, 0.1 * i}});
      ++count;
      // 如果想要整洁的日志，输出必须放在锁的内部！
      std::cout << "线程 [" << id << "] 运行到第 " << i + 1 << " 次\n";
    }
  }
}

auto main() -> int {
  size_t times = 1000;

  // 1. 所有状态都在上层调度器（main）中实例化
  std::vector<ImuFrame> imu_data;
  std::mutex imu_mtx;
  int imu_frame_count = 0;

  imu_data.reserve(2 * times);  // 极其优秀的预分配习惯保留！

  // 2. 将数据、锁、计数器的引用显式传递给线程
  // 注意：std::thread 默认按值拷贝，传递引用必须套一层 std::ref
  std::thread t1(insertImuFrame, std::ref(imu_data), std::ref(imu_mtx),
                 std::ref(imu_frame_count), times, 1);
  std::thread t2(insertImuFrame, std::ref(imu_data), std::ref(imu_mtx),
                 std::ref(imu_frame_count), times, 2);

  // 3. 严格的生命周期屏障
  t1.join();
  t2.join();

  std::cout << "运行结束: \n"
            << "预期的数据长度和计数量: " << 2 * times
            << "\n实际的数据长度: " << imu_data.size()
            << "\n实际的计数量  : " << imu_frame_count << "\n";
  return 0;
}