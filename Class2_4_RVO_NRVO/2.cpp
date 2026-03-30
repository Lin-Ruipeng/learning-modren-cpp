// 结论：如果调用工厂函数的时候看到移动构造日志 = RVO 失败，退化为隐式移动。

#define TEST_NRVO  // 请仅在观察NRVO的时候取消注释

#include <iostream>
#include <memory>
#include <vector>

class ImuDataFrame {
 private:
  std::vector<double> accel_;           // 三轴加速度
  std::vector<double> gyro_;            // 三轴角速度
  std::unique_ptr<int64_t> timestamp_;  // 时间戳

 public:
  ImuDataFrame(double ax = 1, double ay = 2, double az = 3, double gx = -1,
               double gy = -2, double gz = -3,
               int64_t timestamp = -1);  //  默认构造函数

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
ImuDataFrame createImuFrame(bool use_high_precision);

// 构造函数,只是负责初始化值
ImuDataFrame::ImuDataFrame(double ax, double ay, double az, double gx,
                           double gy, double gz, int64_t timestamp)
    : accel_{ax, ay, az},  // 初始化列表
      gyro_{gx, gy, gz},   // 智能指针使用make_是最佳实践
      timestamp_(std::make_unique<int64_t>(timestamp)) {
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
  if (timestamp_) {
    std::cout << "[data] timestamp_ = " << *timestamp_ << std::endl;
  } else {
    std::cout << "[warning] timestamp_ is nullptr!" << std::endl;
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
ImuDataFrame createImuFrame(bool use_high_precision) {
#ifdef TEST_NRVO
  // 此时就算是调用了工厂函数,你也无法看到调用了移动构造
  ImuDataFrame high_res_frame(0.01, 0.02, 0.03, -0.01, -0.02, -0.03, 123456);
  return high_res_frame;
#else
  if (use_high_precision) {
    ImuDataFrame high_res_frame(0.01, 0.02, 0.03, -0.01, -0.02, -0.03, 123456);
    return high_res_frame;
  } else {
    ImuDataFrame low_res_frame(0.1, 0.2, 0.3, -0.1, -0.2, -0.3, 123456);
    return low_res_frame;
  }
#endif
}

auto main() -> int {
  std::cout << "=== 测试 RVO/隐式移动 ===" << std::endl;

  std::cout << "=== 测试工厂函数 ===" << std::endl;

  auto frame = createImuFrame(true);  // 结果显示: 构造器+移动构造

  frame.display();  // 测试工厂函数

  std::cout << "=== 测试移动 ===" << std::endl;

  auto f1 = createImuFrame(false);  // 结果显示: 构造器+移动构造
  auto f2 = std::move(f1);          // 结果显示: 只用了移动构造

  f1.display();
  f2.display();

  std::cout << "=== 测试自移动 ===" << std::endl;

  auto f3 = ImuDataFrame(0, 0, 0);  // 结果显示: 只用了构造器
  f3 = std::move(f3);               // 显示了移动赋值自交换!
  f3.display();                     // 数据不变!

  return 0;
}