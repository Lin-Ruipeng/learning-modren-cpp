#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

std::mutex g_imu_mutex;
int g_imu_processed = 0;  // 共享：已处理IMU帧数

void add_processed_count() {
    // 自动加锁/解锁，保护临界区
    std::lock_guard<std::mutex> lock(g_imu_mutex);
    ++g_imu_processed;
}

void worker(int id) {
    for (int i = 0; i < 100; ++i) {
        add_processed_count();
        std::cout << "线程" << id << "处理1帧IMU数据\n";
    }
}

int main() {
    std::thread t1(worker, 1);
    std::thread t2(worker, 2);

    t1.join();
    t2.join();

    std::cout << "总处理帧数: " << g_imu_processed << std::endl;
    return 0;
}