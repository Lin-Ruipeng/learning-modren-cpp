#include <iostream>
#include <thread>
#include <memory>

// 模拟IMU数据处理函数
void process_imu_data(std::unique_ptr<double[]> imu_data, int data_len) {
    for (int i = 0; i < data_len; ++i) {
        printf("处理IMU数据: %.2f\n", imu_data[i]);
    }
}

int main() {
    // 模拟生成一组IMU原始数据（加速度+角速度）
    const int len = 5;
    auto imu_buf = std::make_unique<double[]>(len);
    for (int i = 0; i < len; ++i) imu_buf[i] = i * 0.1;

    // 1. 创建线程，move转移智能指针所有权（零拷贝）
    std::thread imu_thread(process_imu_data, std::move(imu_buf), len);

    // 2. 等待线程执行完毕（IMU场景必用）
    imu_thread.join();

    return 0;
}
