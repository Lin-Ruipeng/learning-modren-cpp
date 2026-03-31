#include <iostream>
#include <vector>

class Class_t {
 public:
  std::vector<double> data;
};

Class_t obj;

// 函数重载,左值引用
void calibrateImu(const std::vector<double>& data) {
  std::cout << "[Lvalue Reference]" << std::endl;
  if (data.size() == 0) {
    std::cout << "para: none! data.size() == 0" << std::endl;
  } else {
    std::cout << "para: data[0] = " << data[0] << std::endl;
  }
  std::cout << std::endl;
}

// 函数重载,右值引用
void calibrateImu(std::vector<double>&& data) {
  std::cout << "[Rvalue Reference]" << std::endl;
  if (data.size() == 0) {
    std::cout << "para: none! data.size() == 0" << std::endl;
  } else {
    std::cout << "para: data[0] = " << data[0] << std::endl;
  }
  std::cout << std::endl;

  obj.data = std::move(data);
  // 函数签名里写了&&,代表外界传入函数参数时在外界看来是右值
  // 但是在本函数体里,这个参数有名字data,那么就是左值
  // 想把左值传给 obj->data 的移动赋值运算符，就必须用 std::move 把它强转回右值
  // “有名字就是左值，想当右值必须 move”
}

auto main() -> int {
  // 1. 定义左值
  std::vector<double> raw_data = {0.1, 0.2};

  // 2. 调用左值版本
  calibrateImu(raw_data);  // 直接传入左值名称就是左值引用

  // 3. 调用右值版本(copy方式)
  // calibrateImu(std::vector<double>{raw_data}); //
  // 创建一个没有名字的临时对象,就是右值了 // 会引发深拷贝,性能很差!
  calibrateImu(
      std::vector<double>{0.1, 0.2});  // 创建一个没有名字的临时对象,就是右值了

  // 4. 调用右值版本(move方式)
  calibrateImu(std::move(raw_data));  // 使用move就是右值引用
  // 解释: move可以把左值转换成右值类型,但是因为这块内存的名字还在所以还是左值
  // 只不过,编译器会在move之后,vector类型会自动将raw_data指向nullptr防止出问题
  // 这是vector容器的善后处理,避免了很多问题,比如双重释放

  // 5. 检验move,既要看移出,又要看移入
  // 看移出,此时的raw_data处于有效但是未指定的状态
  if (raw_data.size() == 0) {
    std::cout << "move移出正确!" << std::endl;
  } else {
    std::cout << "move移出错误!" << std::endl;
  }
  // 看移入
  if (obj.data.size() != 0) {
    std::cout << "move移入成功!" << std::endl;
    std::cout << "额外检查: data[0] = " << obj.data[0] << std::endl
              << "--------; data[1] = " << obj.data[1] << std::endl;
  } else {
    std::cout << "move移入失败!" << std::endl;
  }

  // 6. 尝试将左值传递给右值引用参数
  // 注意,为了做到这一点,需要将左值引用版本的函数重载注释起来
  // 然后直接编译就可以,因为在 1. 处就是传入左值参数,所以不需要额外的代码
}
