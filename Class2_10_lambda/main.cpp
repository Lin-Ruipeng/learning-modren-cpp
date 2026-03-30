// 需要C++20
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

void display(std::vector<double>& distances) {
  std::cout << "distance = [";
  for (const auto& d : distances) {
    std::cout << ' ' << d << ',';
  }
  std::cout << "]" << std::endl;
}

auto main() -> int {
  std::vector<double> distances = {1.5, 0.2, 8.9, 0.1, 5.5, 15.2};

  std::cout << "1. 创建时:" << std::endl;
  display(distances);

  // 小于 0.5 的是无效噪点，除去
  std::erase_if(distances, [](double d) { return d < 0.5; });

  std::cout << "2. 去噪后:" << std::endl;
  display(distances);

  // 每个数字求平方并且放回原地!
  std::transform(distances.begin(), distances.end(), distances.begin(),
                 [](double d) { return pow(d, 2); });

  std::cout << "3. 取平方后:" << std::endl;
  display(distances);

  std::cout << "4. 第 1 次打印tag:" << std::endl;
  std::string tag = "Radar_Squared";
  std::cout << "[tag 1 ] = " << tag << std::endl;

  std::for_each(distances.begin(), distances.end(),
                // [] 用移动捕获外界的tag
                [tag = std::move(tag)](double d) {
                  std::cout << "[" << tag << "] " << d << " ";
                });

  std::cout << std::endl;
  std::cout << "5. 第 2 次打印tag:" << std::endl;
  std::cout << "[tag 2 ] = " << tag << std::endl;
  // 这里会打印空字符串!因为tag被move了!

  return 0;
}
