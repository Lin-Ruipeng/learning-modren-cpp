#include <iostream>
#include <vector>
#include <memory>

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

    ImuDataFrame(ImuDataFrame &&) = default;            // 移动构造
    ImuDataFrame &operator=(ImuDataFrame &&) = default; // 移动赋值

    void display() const; // 不修改成员就加 const
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

void ImuDataFrame::display() const
{
    if (timestamp_)
    {
        std::cout << "[data] timestamp_ = " << *timestamp_ << std::endl;
    }
    else
    {
        std::cout << "[warning] timestamp_ is nullptr!" << std::endl;
    }

    if (!accel_.empty())
    {
        // 用迭代器比用for(int i)更好!
        // 而且尤其注意是 const &
        std::cout << "[data] accel: ";
        for (const auto &val : accel_)
        {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }
    else
    {
        std::cout << "[warning] accel_ is empty!" << std::endl;
    }

    if (!gyro_.empty())
    {
        std::cout << "[data] gyro: ";
        for (const auto &val : gyro_)
        {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }
    else
    {
        std::cout << "[warning] gyro_ is empty!" << std::endl;
    }
}

auto main() -> int
{
    // 测试移动构造
    ImuDataFrame frame1(0.1, -0.1, 0.2, 0.3, 0.4, -9.8, 12345678);

    std::cout << "\n=== 移动构造 frame2 ===" << std::endl;
    ImuDataFrame frame2(std::move(frame1));

    std::cout << "\nframe2 (移入后):" << std::endl;
    frame2.display();

    std::cout << "\nframe1 (移出后):" << std::endl;
    frame1.display(); // ✅ 现在 timestamp_ 为 nullptr，accel/gyro 被移走

    // 测试移动赋值
    std::cout << "\n=== 移动赋值给 frame1 ===" << std::endl;
    frame1 = ImuDataFrame(9.9, 8.8, 7.7, -1.0, -2.0, -3.0, 999);

    std::cout << "\nframe1 (重新赋值后):" << std::endl;
    frame1.display();

    // 测试自赋值（编译器生成的 default 移动赋值有保护）
    std::cout << "\n=== 自赋值测试 ===" << std::endl;
    ImuDataFrame frame3(1, 2, 3, 4, 5, 6, 100);
    frame3.display();

    frame3 = std::move(frame3); // 有保护，不会崩溃

    std::cout << "\nframe3 自赋值后:" << std::endl;
    frame3.display(); // 未指明状态，可能空也可能有值

    return 0;
}