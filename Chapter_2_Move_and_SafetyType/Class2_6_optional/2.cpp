#include <iostream>
#include <memory>
#include <optional>
#include <vector>

class ImuDataFrame {
 private:
  std::vector<double> accel_;         // 三轴加速度
  std::vector<double> gyro_;          // 三轴角速度
  std::optional<int64_t> timestamp_;  // 时间戳

 public:
  ImuDataFrame(
      double ax = 1, double ay = 2, double az = 3, double gx = -1,
      double gy = -2, double gz = -3,
      std::optional<int64_t> timestamp = std::nullopt);  //  默认构造函数

  // 显式声明移动操作（禁用拷贝，因为 unique_ptr 不可拷贝）
  ImuDataFrame(const ImuDataFrame&) = delete;
  ImuDataFrame& operator=(const ImuDataFrame&) = delete;

  ImuDataFrame(ImuDataFrame&& other) noexcept;             // 移动构造
  ImuDataFrame& operator=(ImuDataFrame&& other) noexcept;  // 移动赋值

  // ImuDataFrame(ImuDataFrame &&) = default; // 移动构造
  // ImuDataFrame &operator=(ImuDataFrame &&) = default; // 移动赋值

  void display() const;  // 不修改成员就加 const
};

//  工厂函数声明
std::optional<ImuDataFrame> createImuFrame(bool is_sensor_online);

// 构造函数,只是负责初始化值
ImuDataFrame::ImuDataFrame(double ax, double ay, double az, double gx,
                           double gy, double gz,
                           std::optional<int64_t> timestamp)
    : accel_{ax, ay, az},  // 初始化列表
      gyro_{gx, gy, gz},   // make_optional
      timestamp_(timestamp) {
  // 使用构造函数的初始化列表,比在函数体里写,不会创建临时对象,性能更优!
  std::cout << "[Class info] 对象构造! 初始化完毕!" << std::endl;
}

// 移动构造
ImuDataFrame::ImuDataFrame(ImuDataFrame&& other) noexcept
    : accel_(std::move(other.accel_)),
      gyro_(std::move(other.gyro_)),
      timestamp_(std::move(other.timestamp_)) {
  std::cout << "[Class info] 移动构造被调用!" << std::endl;
}

// 移动赋值
ImuDataFrame& ImuDataFrame::operator=(ImuDataFrame&& other) noexcept {
  if (this == &other) {
    std::cout << "[warning] 移动赋值被调用,但是你在尝试自交换!不予执行!"
              << std::endl;
    return *this;
  }
  std::cout << "[Class info] 移动赋值被调用!" << std::endl;

  accel_ = std::move(other.accel_);
  gyro_ = std::move(other.gyro_);
  timestamp_ = std::move(other.timestamp_);
  return *this;
}

void ImuDataFrame::display() const {
  if (timestamp_.has_value()) {
    std::cout << "[data] timestamp_ = " << *timestamp_ << std::endl;
  } else {
    std::cout << "[warning] timestamp_ is nullopt!" << std::endl;
  }

  if (!accel_.empty()) {
    // 用迭代器比用for(int i)更好!
    // 而且尤其注意是 const &
    std::cout << "[data] accel: ";
    for (const auto& val : accel_) {
      std::cout << val << " ";
    }
    std::cout << std::endl;
  } else {
    std::cout << "[warning] accel_ is empty!" << std::endl;
  }

  if (!gyro_.empty()) {
    std::cout << "[data] gyro: ";
    for (const auto& val : gyro_) {
      std::cout << val << " ";
    }
    std::cout << std::endl;
  } else {
    std::cout << "[warning] gyro_ is empty!" << std::endl;
  }
}

// 工厂函数定义
std::optional<ImuDataFrame> createImuFrame(bool is_sensor_online) {
  if (is_sensor_online) {
    return ImuDataFrame{0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 666};
  } else {
    return std::nullopt;
  }
}

auto main() -> int {
  if (auto f1 = createImuFrame(true); f1.has_value()) {
    f1->display();
  } else {
    std::cout << "[warning] 传感器掉线，无数据" << std::endl;
  }

  if (auto f2 = createImuFrame(false); f2.has_value()) {
    f2->display();
  } else {
    std::cout << "[warning] 传感器掉线，无数据" << std::endl;
  }

  return 0;
}