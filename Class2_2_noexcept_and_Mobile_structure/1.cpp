// 代码示例：简易 IMU 缓冲区
// 假设我们有一个管理底层原生指针的类
// （为了看清本质，我们不用 vector，用原始指针）：

#include <iostream>
class SimpleImuBuffer {
 private:
  double* data_ptr_;
  size_t size_;

 public:
  // 普通构造：分配内存
  SimpleImuBuffer(size_t size) : size_(size), data_ptr_(new double[size]) {}

  // 析构函数：释放内存
  ~SimpleImuBuffer() {
    delete[] data_ptr_;
    // 工程常识：delete nullptr 是绝对安全的，这也是为什么移动后必须置空的原因！
  }

  // 【重点】移动构造函数 也就是move
  SimpleImuBuffer(SimpleImuBuffer&& other) noexcept
      : data_ptr_(other.data_ptr_), size_(other.size_) {
    // 第一步：浅拷贝（窃取指针）
    // 第二步：将源对象“掏空”（置空处理）
    // 如果不置空，当 other 析构时，会 delete[] 我们刚偷过来的内存，导致 double
    // free！
    other.data_ptr_ = nullptr;
    other.size_ = 0;
  }

  // 这两行一起禁用拷贝构造（实际 IMU 大内存对象通常不允许拷贝）
  // 这行禁用拷贝构造(创建时) SimpleImuBuffer a = b; 或 SimpleImuBuffer a(b);
  SimpleImuBuffer(const SimpleImuBuffer&) = delete;
  // 这行禁用拷贝赋值(创建之后) a = b;
  SimpleImuBuffer& operator=(const SimpleImuBuffer&) = delete;
};
