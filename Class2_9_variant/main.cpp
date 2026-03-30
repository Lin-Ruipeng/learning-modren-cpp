#include <iostream>
#include <variant>
#include <vector>
#include <string>
#include <type_traits>

struct ImuFrame
{
    double accel_x;
    double accel_y;
};

struct GpsFrame
{
    double lat;
    double lon;
};

// 自定义类型别名
using SensorData = std::variant<ImuFrame, GpsFrame>;

auto main() -> int
{
    std::vector<SensorData> buffer;

    buffer.push_back(ImuFrame{1.0, 2.0});
    buffer.push_back(GpsFrame{30.0, 120.0});

    for (const auto &data : buffer)
    {
        // std::visit 会去看那个“标签”，然后把你写好的函数（Lambda）“丢进去”执行。
        std::visit(

            // auto&& arg 意思是：不管你里面装了什么类型，我先用一个万能引用接住它
            [](auto &&arg)
            {
                // 1. decay_t 剥离掉引用和 const，拿到最纯粹的真实类型 T
                using T = std::decay_t<decltype(arg)>;

                // 2. if constexpr 是编译期判断！
                // 如果 T 是 ImuFrame，下面这个分支会被编译；
                if constexpr (std::is_same_v<T, ImuFrame>)
                {
                    std::cout << "[IMU] ax: " << arg.accel_x << " ay: " << arg.accel_y << std::endl;

                } //  3. 同理，匹配GpsFrame
                else if constexpr (std::is_same_v<T, GpsFrame>)
                {
                    std::cout << "[GPS] lat: " << arg.lat << " lon: " << arg.lon << std::endl;
                }
            },
            data);
    }

    return 0;
}
