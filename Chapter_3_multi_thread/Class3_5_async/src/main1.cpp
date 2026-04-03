#include <array>
#include <chrono>
#include <future>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

// 消除魔法数字，提高可读性和可维护性
constexpr std::size_t kFramesSize = 10;
constexpr auto kProcessingTime = std::chrono::milliseconds(50);
constexpr auto kSamplingTime = std::chrono::milliseconds(25);

struct ImuFrame {
  std::array<double, 3> accel{};
  std::array<double, 3> gyro{};
};

std::vector<ImuFrame> filter_imu_async(std::vector<ImuFrame> frames) {
  std::cout << "异步线程开始处理数据!\n"
            << "异步线程接收到的数据大小为: " << frames.size() << "\n";

  std::size_t i = 0;
  for (auto& frame : frames) {
    double accel_mean = (frame.accel[0] + frame.accel[1] + frame.accel[2]) / 3.0;
    std::this_thread::sleep_for(kProcessingTime);  // 模拟处理耗时

    // 注意：实际多线程开发中，最好使用专门的 logger 防止交织
    std::cout << "异步线程处理第 " << ++i << " 帧数据!\n";
    frame.accel.fill(accel_mean);
  }
  std::cout << "异步线程数据处理完毕!\n";

  return frames;  // 触发 NVRO 或 Move，零拷贝返回
}

auto main() -> int {
  // 1. 主线程准备十帧数据
  std::vector<ImuFrame> imu_frames(kFramesSize);
  {
    double i = 0.0;
    for (auto& frame : imu_frames) {
      // 移除多余的 std::move，直接赋值右值
      i += 1.0;
      frame = ImuFrame{.accel{0.0, 0.1 * i, 0.0}, .gyro{1.0, -1.0, 0.0}};
    }
  }

  // 2. 启动异步线程
  std::future<std::vector<ImuFrame>> future_frames = std::async(
      std::launch::async,
      [frames = std::move(imu_frames)]() mutable { return filter_imu_async(std::move(frames)); });

  std::cout << "异步线程已启动(滤波线程), 主线程正在采集下一批 IMU 数据\n";

  // 3. 等待异步线程的时候做点别的事
  // 使用 std::size_t 匹配循环边界，消除比较警告
  for (std::size_t i = 0; i < kFramesSize * 2; ++i) {
    std::this_thread::sleep_for(kSamplingTime);
    std::cout << "又采集到第 " << i + 1 << " 帧原始数据\n";
  }

  // 4. 取出异步线程处理好的数据
  auto filtered_frames = future_frames.get();
  {
    std::size_t i = 0;
    for (const auto& frame : filtered_frames) {
      std::cout << "第 " << ++i << " 帧 滤波结果为 " << frame.accel[0] << "\n";
    }
  }

  return 0;
}