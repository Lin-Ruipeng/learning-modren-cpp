#include <array>
#include <future>
#include <iostream>
#include <memory>

struct ImuFrame {
  std::array<double, 3> accel{};
  std::array<double, 3> gyro{};
};

// 异步滤波函数
std::unique_ptr<ImuFrame> filter_imu_async(std::unique_ptr<ImuFrame> frame) {
  // 模拟滤波计算
  double mean = (frame->accel[0] + frame->accel[1] + frame->accel[2]) / 3.0;
  frame->accel.fill(mean);
  return frame;
}

int main() {
  auto frame = std::make_unique<ImuFrame>(ImuFrame{{1.0, 2.0, 3.0}, {}});

  // 异步执行滤波
  std::future<std::unique_ptr<ImuFrame>> future =
      std::async(std::launch::async, filter_imu_async, std::move(frame));

  // 主线程可继续做其他事（如采集下一帧IMU）
  std::cout << "主线程继续采集..." << std::endl;

  // 阻塞获取结果
  auto filtered = future.get();
  std::cout << "滤波后 acc[0] = " << filtered->accel[0] << std::endl;

  return 0;
}