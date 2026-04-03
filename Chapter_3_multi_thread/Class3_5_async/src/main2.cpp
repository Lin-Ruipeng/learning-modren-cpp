#include <array>
#include <chrono>
#include <future>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

constexpr std::size_t kFramesSize = 10;
constexpr auto kProcessingTime = std::chrono::milliseconds(50);
constexpr auto kSamplingTime = std::chrono::milliseconds(25);

struct ImuFrame {
  std::array<double, 3> accel{};
  std::array<double, 3> gyro{};
};

// 修正 1：恢复按值传递，利用 C++ 编译器的完美移动语义和 NRVO
std::vector<ImuFrame> filter_imu_async(std::vector<ImuFrame> frames) {
  std::cout << "子线程开始处理数据!\n"
            << "子线程接收到的数据大小为: " << frames.size() << "\n";

  std::size_t i = 0;
  for (auto& frame : frames) {
    double accel_mean = (frame.accel[0] + frame.accel[1] + frame.accel[2]) / 3.0;
    std::this_thread::sleep_for(kProcessingTime);
    std::cout << "子线程处理第 " << ++i << " 帧数据!\n";
    frame.accel.fill(accel_mean);
  }
  std::cout << "子线程数据处理完毕!\n";

  return frames;
}

auto main() -> int {
  // 1. 主线程准备十帧数据
  std::vector<ImuFrame> imu_frames(kFramesSize);
  {
    double i = 0.0;
    for (auto& frame : imu_frames) {
      // 修正 2：相信直觉，删掉多余的 std::move，直接利用拷贝消除
      i += 1.0;
      frame = ImuFrame{.accel{0.0, 0.1 * i, 0.0}, .gyro{1.0, -1.0, 0.0}};
    }
  }

  // 2. 启动子线程
  std::promise<std::vector<ImuFrame>> prom;
  std::future<std::vector<ImuFrame>> future_filtered_frames = prom.get_future();

  // 修正 3：使用 C++20 的 std::jthread，自带 RAII 特性，无需手动 join，保证异常安全
  std::jthread t_filter([prom = std::move(prom), frames = std::move(imu_frames)]() mutable {
    auto result = filter_imu_async(std::move(frames));
    prom.set_value(std::move(result));
  });

  std::cout << "子线程已启动(滤波线程), 主线程正在采集下一批 IMU 数据\n";

  // 3. 等待子线程的时候做点别的事
  for (std::size_t i = 0; i < kFramesSize * 2; ++i) {
    std::this_thread::sleep_for(kSamplingTime);
    std::cout << "又采集到第 " << i + 1 << " 帧原始数据\n";
  }

  // 4. 取出子线程处理好的数据
  auto filtered_frames = future_filtered_frames.get();
  {
    std::size_t i = 0;
    for (const auto& frame : filtered_frames) {
      std::cout << "第 " << ++i << " 帧 滤波结果为 " << frame.accel[0] << "\n";
    }
  }

  // std::jthread 会在此处自动 join，非常优雅！
  return 0;
}