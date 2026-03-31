#include <iostream>
#include <memory>

// 正确用法示例
class CallbackHandler : public std::enable_shared_from_this<CallbackHandler> {
 public:
  void registerCallback() {
    // 获取指向自己的 shared_ptr，安全地传递给外部
    auto self = shared_from_this();
    std::cout << "Registered self." << std::endl;
  }
};

void demo_enable_shared_from_this() {
  // 必须先让 shared_ptr 接管对象
  auto handler = std::make_shared<CallbackHandler>();
  handler->registerCallback();

  // 错误示范：如果 handler 是栈对象或裸指针，调用 shared_from_this() 会崩溃！
  // CallbackHandler bad_handler;
  // bad_handler.registerCallback(); // 抛出 std::bad_weak_ptr 异常
}
