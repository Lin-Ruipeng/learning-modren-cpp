// 假设你有一个 IMU 预处理模块，
// 对于从传感器实时读出来的缓存（左值），你只想读取；
// 对于通过数学变换临时生成的一大段仿真数据（右值），
// 你想要直接窃取它的内存。
#include <iostream>
#include <vector>

// 重载 1：接收左值，只读不窃取
void preprocessImuData(const std::vector<double>& data) {
  std::cout << "[左值引用版本] 接收到持久数据，大小: " << data.size()
            << "，仅做读取滤波处理。" << std::endl;
}

// 重载 2：接收右值，准备窃取资源
void preprocessImuData(std::vector<double>&& data) {
  std::cout << "[右值引用版本] 接收到临时/将亡数据，大小: " << data.size()
            << "，准备转移其底层内存！" << std::endl;
  // 注意：data 在这里有名字，按语法它是左值。
  // 如果要在本函数内把它转移给其他对象，必须用 std::move(data)。
}

int main() {
  // 1. 左值：真实的传感器数据缓冲区
  std::vector<double> real_sensor_data = {1.0, 2.0, 3.0, 4.0, 5.0};
  preprocessImuData(real_sensor_data);  // 精准命中 重载 1

  // 2. 右值：纯右值，临时构造的巨大仿真数据块
  preprocessImuData(std::vector<double>(10000, 0.5));  // 精准命中 重载 2

  // 3. 右值：通过 std::move 将左值强制转换为右值
  preprocessImuData(std::move(real_sensor_data));  // 精准命中 重载 2

  return 0;
}
