#include <iostream>
#include <memory>

struct ImuData {
  double accel[3];
  ImuData(double x, double y, double z) {
    accel[0] = x;
    accel[1] = y;
    accel[2] = z;
  }
  ~ImuData() { std::cout << "ImuData Destroyed\n"; }
};

void demo_shared_ptr() {
  // [构造] 推荐 make_shared：一次内存分配（对象+控制块），性能更优且异常安全
  auto p1 = std::make_shared<ImuData>(1.0, 2.0, 3.0);
  std::cout << "p1 use_count: " << p1.use_count() << std::endl;  // 1

  {
    // [共享] 拷贝构造，引用计数 +1
    std::shared_ptr<ImuData> p2 = p1;
    std::cout << "p1 use_count inside scope: " << p1.use_count()
              << std::endl;  // 2

    // [重置] p2 离开作用域，引用计数 -1
  }

  std::cout << "p1 use_count outside scope: " << p1.use_count()
            << std::endl;  // 1

  // [自定义删除器] 常用于对接 C 语言接口或特殊硬件资源
  // 例如：管理由 malloc 分配的内存，或文件句柄
  std::shared_ptr<FILE> file_ptr(fopen("log.txt", "w"), [](FILE* f) {
    if (f) {
      fclose(f);
      std::cout << "File closed safely.\n";
    }
  });
}
