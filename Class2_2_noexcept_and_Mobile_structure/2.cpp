#include <iostream>

class ImuDataFrame {
 private:
  double* accel_ = nullptr;       // 三轴加速度
  double* gyro_ = nullptr;        // 三轴角速度
  int64_t* timestamp_ = nullptr;  // 时间戳

 public:
  ImuDataFrame(double ax = 1.0, double ay = 2.0, double az = 3.0,
               double gx = -1.0, double gy = -2.0, double gz = -3.0,
               int64_t timestamp = -1);  // 重构默认构造函数,为了实现RAII
  ~ImuDataFrame();                       // 重构默认析构函数,为了实现RAII

  // 禁用 copy语义的 "=" (小细节,这俩有const)
  ImuDataFrame(const ImuDataFrame& other) = delete;  // 1. 禁用拷贝构造
  ImuDataFrame& operator=(const ImuDataFrame& other) =
      delete;  // 2. 禁用拷贝赋值

  // 重载 move语义的 "=" (小细节,这俩没有const,但是有noexcept)
  ImuDataFrame(
      ImuDataFrame&& other) noexcept;  // 3. 重载右值引用的 移动构造函数
  ImuDataFrame& operator=(
      ImuDataFrame&& other) noexcept;  // 4. 重载右值引用的 移动赋值运算符

  void display();
};

// 构造函数,申请内存 RAII
ImuDataFrame::ImuDataFrame(double ax, double ay, double az, double gx,
                           double gy, double gz, int64_t timestamp) {
  accel_ = new double[3]{ax, ay, az};
  gyro_ = new double[3]{gx, gy, gz};
  timestamp_ = new int64_t(timestamp);
  std::cout << "[Class info] 对象构造! 内存申请完毕!" << std::endl;
}

// 析构函数,释放内存 RAII
ImuDataFrame::~ImuDataFrame() {
  delete[] accel_;
  delete[] gyro_;
  delete timestamp_;
  std::cout << "[Class info] 对象自动析构! 内存释放完毕!" << std::endl;
}

// 右值引用的移动构造函数,三步走: 1.noexcept 2.":"初始化列表浅拷贝 3.清除旧对象(避免双重释放)
ImuDataFrame::ImuDataFrame(ImuDataFrame&& other) noexcept
    : accel_(other.accel_), gyro_(other.gyro_), timestamp_(other.timestamp_) {
  other.accel_ = nullptr;
  other.gyro_ = nullptr;
  other.timestamp_ = nullptr;
  std::cout << "[Class info] 右值引用的移动构造函数被调用!" << std::endl;
}

// 右值引用的移动赋值运算符,这里请注意
// 命名空间写在哪里!(ImuDataFrame::作用于函数名)
ImuDataFrame& ImuDataFrame::operator=(ImuDataFrame&& other) noexcept {
  // 注意这个不是构造函数,不能使用初始化列表!

  // 1. 防御性编程,先确保other不是自己!
  if (this == &other) {
    std::cout << "[warning] 移动赋值运算符检查到尝试进行自我交换!" << std::endl;
    return *this;
  }

  std::cout << "[Class info] 右值引用的移动赋值运算符被调用!" << std::endl;

  // 2. 旧的资源先释放(不释放会内存泄漏!)
  delete[] accel_;
  delete[] gyro_;
  delete timestamp_;

  // 3. 浅拷贝(实现语义)
  accel_ = other.accel_;
  gyro_ = other.gyro_;
  timestamp_ = other.timestamp_;

  // 4. 再置空处理(不置空会二次释放!)
  other.accel_ = nullptr;
  other.gyro_ = nullptr;
  other.timestamp_ = nullptr;

  // 5. 返回
  return *this;
}

void ImuDataFrame::display() {
  if (timestamp_) {
    std::cout << "[data] timestamp_ = " << *timestamp_ << std::endl;
  } else {
    std::cout << "[warning] timestamp_ is nullptr!" << std::endl;
  }

  if (accel_) {
    for (int i = 0; i < 3; i++) {
      std::cout << "[data] accel_[" << i << "] = " << accel_[i] << std::endl;
    }
  } else {
    std::cout << "[warning] accel_ is nullptr!" << std::endl;
  }

  if (gyro_) {
    for (int i = 0; i < 3; i++) {
      std::cout << "[info] gyro_[" << i << "] = " << gyro_[i] << std::endl;
    }
  } else {
    std::cout << "[warning] gyro_ is nullptr!" << std::endl;
  }
}

auto main() -> int {
  // // 1. 创建一个原始对象 frame1
  // ImuDataFrame frame1(0.1, -0.1, 0.2, 0.3, 0.4, -9.8, 12345678);

  // // 2. 使用 std::move(frame1) 移动构造一个新对象 frame2
  // ImuDataFrame frame2(std::move(frame1));

  // // 3. 打印 frame2 中的数据以验证窃取成功
  // std::cout << "[info] frame2: " << std::endl;
  // frame2.display();

  // // 4. 打印 frame1 中的指针地址（应当是 0，即
  // nullptr），证明源对象被安全掏空 std::cout << "[info] frame1: " <<
  // std::endl; frame1.display();

  // // 5. 确保程序结束时正常退出，没有 double free
  // // 这个通过查看运行时的终端输出,看析构函数是否被正确调用

  // 1. 创建 frame1
  ImuDataFrame frame1(0.1, -0.1, 0.2, 0.3, 0.4, -9.8, 12345678);

  // 2. 用 frame1 移动构造 frame2（此时 frame1 应该被掏空）
  ImuDataFrame frame2(std::move(frame1));

  // 3. 创建一个临时右值 ImuDataFrame(9.9, 8.8, 7.7, -1.0, -2.0, -3.0, 999)
  // 4. 使用移动赋值，把这个临时右值赋给已经被掏空的 frame1
  // 5. 打印 frame1 的数据，验证它成功获得了新数据（即“拆旧建新”成功）
  std::cout << "检查move移出之后的frame1" << std::endl;
  frame1.display();
  frame1 = ImuDataFrame(9.9, 8.8, 7.7, -1.0, -2.0, -3.0, 999);
  std::cout << "检查move移入之后的frame1" << std::endl;
  frame1.display();

  // 6. 创建 frame3，执行一次自赋值操作：frame3 = std::move(frame3);
  ImuDataFrame frame3;
  frame3 = std::move(frame3);

  // 7. 打印 frame3 的数据，验证自赋值检查保护了它没有被掏空
  frame3.display();

  return 0;
}