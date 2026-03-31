#include <algorithm>
#include <iostream>
#include <tuple>
#include <type_traits>
#include <variant>
#include <vector>

// 重载 overloaded()
template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

struct ImuFrame {
  double accel;
  bool is_valid;
};
// 重载 overloaded()

struct GpsFrame {
  double lat;
  double lon;
};

using SensorData = std::variant<ImuFrame, GpsFrame>;

auto processBatch(std::vector<SensorData> buffer)
    -> std::tuple<size_t, size_t, std::vector<SensorData>>;

auto main() -> int {
  std::vector<SensorData> raw = {ImuFrame{1.0, true}, GpsFrame{0.0, 0.0},
                                 ImuFrame{2.0, false}, GpsFrame{30.0, 120.0}};

  auto [imu_num, gps_num, clean_data] = processBatch(std::move(raw));

  // 打印统计结果（Valid -> IMU: 1, GPS: 1）。
  std::cout << "Valid -> IMU: " << imu_num << ", GPS: " << gps_num << std::endl;

  // 最后遍历 clean_data
  for (const auto& data : clean_data) {
    // 因为 visit 只能处理单个的variant,所以需要配合for each
    std::visit(overloaded{[](const ImuFrame& imu) {
                            std::cout << "[IMU Data] accel: " << imu.accel
                                      << std::endl;
                          },
                          [](const GpsFrame& gps) {
                            std::cout << "[GPS Data] lat = " << gps.lat
                                      << ", lon = " << gps.lon << std::endl;
                          }},
               data);
  }

  return 0;
}

auto processBatch(std::vector<SensorData> buffer)
    -> std::tuple<size_t, size_t, std::vector<SensorData>> {
  // 1. 过滤:剔除 is_valid==false的IMU，剔除lat==0.0&&lon==0.0的无效GPS
  // 剔除元素就用erase_if(vector, lambda)
  std::erase_if(buffer, [](const auto& data) {
    // 为了处理多种类型,就需要使用visit
    // 接着配合overloaded,这个需要自己重载! C++17
    return std::visit(
        overloaded{[](const ImuFrame& imu) { return imu.is_valid == false; },
                   [](const GpsFrame& gps) {
                     return gps.lat == 0.0 && gps.lon == 0.0;
                   }},
        data);  // 针对data的不同类型调用前面不同的lambda
  });

  // 2. 转换与统计
  size_t imu_cnt = 0, gps_cnt = 0;
  // for_each(起始迭代器,结束迭代器,lambda)
  std::for_each(buffer.begin(), buffer.end(),
                // 引用捕获!嵌套时每一层都需要进行捕获!
                [&imu_cnt, &gps_cnt](SensorData& data) {
                  std::visit(
                      overloaded{[&imu_cnt](ImuFrame& imu) {
                                   imu.accel *= 9.8;
                                   imu_cnt++;
                                 },
                                 [&gps_cnt](GpsFrame& gps) {
                                   gps.lat *= 1.1;
                                   gps.lon *= 1.1;
                                   gps_cnt++;
                                 }},
                      data);  // 因为是在lambda表达式的{}中,所以是语句要写;
                });

  // 3. 返回,匿名 RVO优化!
  return {imu_cnt, gps_cnt, std::move(buffer)};
}