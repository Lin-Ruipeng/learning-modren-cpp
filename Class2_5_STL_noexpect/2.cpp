#include <iostream>
#include <vector>
#include <memory>
#include <type_traits> // 用于调试

class ImuDataFrame
{
private:
    std::vector<double> accel_;          // 三轴加速度
    std::vector<double> gyro_;           // 三轴角速度
    std::unique_ptr<int64_t> timestamp_; // 时间戳

public:
    ImuDataFrame(double ax = 1, double ay = 2, double az = 3,
                 double gx = -1, double gy = -2, double gz = -3,
                 int64_t timestamp = -1); //  默认构造函数

    // 显式声明移动操作（禁用拷贝，因为 unique_ptr 不可拷贝）
    ImuDataFrame(const ImuDataFrame &) = delete;
    ImuDataFrame &operator=(const ImuDataFrame &) = delete;

    ImuDataFrame(ImuDataFrame &&other);            // noexcept; // 移动构造
    ImuDataFrame &operator=(ImuDataFrame &&other); // noexcept; // 移动赋值

    // ImuDataFrame(ImuDataFrame &&) = default; // 移动构造
    // ImuDataFrame &operator=(ImuDataFrame &&) = default; // 移动赋值

    // void display() const; // 不修改成员就加 const
};

// 构造函数,只是负责初始化值
ImuDataFrame::ImuDataFrame(double ax, double ay, double az,
                           double gx, double gy, double gz,
                           int64_t timestamp)
    : accel_{ax, ay, az}, // 初始化列表
      gyro_{gx, gy, gz},  // 智能指针使用make_是最佳实践
      timestamp_(std::make_unique<int64_t>(timestamp))
{
    // 使用构造函数的初始化列表,比在函数体里写,不会创建临时对象,性能更优!
    std::cout << "[Class info] 对象构造! 初始化完毕!" << std::endl;
}

// 移动构造
ImuDataFrame::ImuDataFrame(ImuDataFrame &&other) // noexcept
    : accel_(std::move(other.accel_)),
      gyro_(std::move(other.gyro_)),
      timestamp_(std::move(other.timestamp_))
{
    std::cout << "[Class info] 移动构造被调用!" << std::endl;
}

// 移动赋值
ImuDataFrame &ImuDataFrame::operator=(ImuDataFrame &&other) // noexcept
{
    if (this == &other)
    {
        std::cout << "[warning] 移动赋值被调用,但是你在尝试自交换!不予执行!" << std::endl;
        return *this;
    }
    std::cout << "[Class info] 移动赋值被调用!" << std::endl;

    accel_ = std::move(other.accel_);
    gyro_ = std::move(other.gyro_);
    timestamp_ = std::move(other.timestamp_);
    return *this;
}

// 断言,检查调用的函数,报错才对,因为注释了noexcept
// static_assert(std::is_nothrow_move_constructible<ImuDataFrame>::value,
//   "错误: ImuDataFrame 的移动构造不是 noexcept, 无法提供强异常安全保证！");


auto main() -> int
{
    // 测试, 因为没有 noexcept 所以应该会编译失败
    // 但是实际上编译器会妥协采用其他方式实现,所以甚至可以编译运行
    // 那么为了检查调用的函数类型是否完全匹配,需要使用静态断言
    std::vector<ImuDataFrame> frames;
    frames.push_back(ImuDataFrame(1, 2, 3, 4, 5, 6, 100));
    frames.push_back(ImuDataFrame(1, 2, 3, 4, 5, 6, 200));
    frames.push_back(ImuDataFrame(1, 2, 3, 4, 5, 6, 300));

    return 0;
}