#include <atomic>
#include <iostream>
#include <thread>

// 原子计数：无锁线程安全
std::atomic<int> g_imu_processed{0};
// 原子启停标志：无锁控制线程
std::atomic<bool> g_stop{false};

void imu_worker(int id) {
  // 原子读：判断是否停止
  while (!g_stop) {
    // 原子自增：无锁安全
    g_imu_processed.fetch_add(1, std::memory_order_relaxed);
    printf("线程%d处理1帧IMU，总帧数：%d\n", id, g_imu_processed.load());
  }
}

int main() {
  std::thread t1(imu_worker, 1);
  std::thread t2(imu_worker, 2);

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  // 原子写：停止线程
  g_stop = true;

  t1.join();
  t2.join();
  std::cout << "最终处理总帧数：" << g_imu_processed << std::endl;
  return 0;
}