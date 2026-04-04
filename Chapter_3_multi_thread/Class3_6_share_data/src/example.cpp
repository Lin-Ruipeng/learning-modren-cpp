#include <array>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

struct ImuFrame {
  std::array<double, 3> accel{};
  std::array<double, 3> gyro{};
};

// 批量IMU数据块（大数据，必须指针转移）
using ImuBatch = std::vector<ImuFrame>;

// 子线程：接收unique_ptr零拷贝数据
void process_batch(std::unique_ptr<ImuBatch> batch_ptr) {
  std::cout << "子线程处理批量IMU数据，帧数：" << batch_ptr->size() << "\n";
  for (const auto& frame : *batch_ptr) {
    // 模拟IMU插值处理
  }
}

int main() {
  // 主线程生成100帧批量IMU数据
  auto batch = std::make_unique<ImuBatch>(100);
  for (int i = 0; i < 100; ++i) {
    (*batch)[i].accel = {0.1 * i, 0, 0};
  }

  // 零拷贝转移给子线程，主线程不再持有
  std::thread t(process_batch, std::move(batch));
  t.join();

  return 0;
}