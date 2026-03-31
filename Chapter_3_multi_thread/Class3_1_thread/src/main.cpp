#include <array>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

struct ImuFrame {
  std::array<double, 3> accel;
  std::array<double, 3> gyro;
};

auto data_generation(size_t) -> std::vector<ImuFrame>;
void data_display(std::vector<ImuFrame>);

auto main() -> int {
  // 主线程生成数据
  size_t data_length = 10;
  auto data_buffer = data_generation(data_length);

  // 创建子线程打印数据
  std::thread process_imu_data_thread(data_display, std::move(data_buffer));

  // 等待子线程运行结束
  process_imu_data_thread.join();

  return 0;
}

auto data_generation(size_t length) -> std::vector<ImuFrame> {
  auto data_buffer = std::vector<ImuFrame>(length);
  size_t i = 1;
  for (auto& data : data_buffer) {
    data =
        ImuFrame{{0.01 * i, -0.02 * i, 0.03 * i}, {0.1 * i, 0.2 * i, -0.3 * i}};
    ++i;
  }
  return data_buffer;
}

void data_display(std::vector<ImuFrame> data_buffer) {
  for (const auto& data : data_buffer) {
    std::cout << "[data frame] : " << std::endl;
    // 打印加速度
    std::cout << "[accel] : ";
    for (const auto& acc : data.accel) {
      std::cout << acc << ", ";
    }
    std::cout << std::endl;
    // 打印角速度
    std::cout << "[gyro] : ";
    for (const auto& gyro : data.gyro) {
      std::cout << gyro << ", ";
    }
    std::cout << std::endl;
  }
}
