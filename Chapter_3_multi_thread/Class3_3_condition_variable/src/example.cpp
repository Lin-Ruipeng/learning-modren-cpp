#include <array>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

struct ImuFrame {
  std::array<double, 3> accel{};
  std::array<double, 3> gyro{};
};

std::mutex mtx;
std::queue<ImuFrame> imu_queue;
std::condition_variable cv;
bool stop_flag = false;

// 生产者：生成IMU数据
void producer() {
  for (int i = 0; i < 10; ++i) {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    ImuFrame frame{{0.1 * i, 0, 0}, {0, 0.2 * i, 0}};

    std::unique_lock<std::mutex> lock(mtx);
    imu_queue.push(frame);
    cv.notify_one();
  }
  std::lock_guard<std::mutex> lock(mtx);
  stop_flag = true;
  cv.notify_one();
}

// 消费者：处理IMU数据
void consumer() {
  while (true) {
    std::unique_lock<std::mutex> lock(mtx);
    // 防止虚假唤醒 + 阻塞等待
    cv.wait(lock, [] { return !imu_queue.empty() || stop_flag; });

    if (stop_flag && imu_queue.empty()) break;

    auto frame = imu_queue.front();
    imu_queue.pop();
    lock.unlock();

    printf("处理IMU: accel=%.2f\n", frame.accel[0]);
  }
}

int main() {
  std::thread t_prod(producer);
  std::thread t_cons(consumer);
  t_prod.join();
  t_cons.join();
  return 0;
}