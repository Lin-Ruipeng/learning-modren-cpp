#include <iostream>
#include <memory>
#include <vector>

// 配置参数结构
struct ProcessorConfig {
  int buffer_size = 10;        // 环形缓冲区大小
  double sample_rate = 200.0;  // 采样率
};

// IMU 数据结构
struct ImuRawData {
  float ax, ay, az;
  float gx, gy, gz;
  long long timestamp;
};

// 任务一：IMU 处理器类
// 要求：
// 1. 继承 enable_shared_from_this，以便能够安全地获取自身的 shared_ptr
// 2. 内部使用 unique_ptr 管理缓冲区数组
// 3. 接受一个 shared_ptr<ProcessorConfig> 作为配置
class ImuProcessor : public std::enable_shared_from_this<ImuProcessor> {
 private:
  std::shared_ptr<ProcessorConfig> config_;  // 共享配置
  std::unique_ptr<ImuRawData[]> buffer_;     // 环形缓冲区（独占）
  int write_index_ = 0;                      // 写入位置
  int count_ = 0;                            // 当前数据量

 public:
  // 构造函数
  // TODO: 实现构造函数，初始化配置和缓冲区
  // 提示：缓冲区大小从 config_ 中读取，使用 make_unique<ImuRawData[]> 分配数组
  ImuProcessor(std::shared_ptr<ProcessorConfig> config)
      : config_(config), write_index_(0), count_(0) {
    // [你的代码在这里]
    this->buffer_ = std::make_unique<ImuRawData[]>(
        this->config_->buffer_size);  // 直接构造就行,注意是数组
    // 分配 buffer_，大小为 config_->buffer_size
  }

  // 插入数据
  // TODO: 实现环形缓冲区的写入逻辑
  void pushData(const ImuRawData& data) {
    // [你的代码在这里]
    // 1. 将数据写入 buffer_[write_index_]
    this->buffer_[this->write_index_] = data;
    // 2. 更新 write_index_（环形：index = (index + 1) % size）
    this->write_index_ = (this->write_index_ + 1) % this->config_->buffer_size;
    // 3. 更新 count_（不超过 buffer_size）
    if (this->count_ < this->config_->buffer_size) this->count_++;
  }

  // 获取最新的一条数据
  // 返回 shared_ptr<ImuRawData>，方便外部安全使用
  std::shared_ptr<ImuRawData> getLatestData() {
    if (count_ == 0) return nullptr;

    // 注意：这里不能直接返回 buffer_ 中的指针，因为 buffer_ 是 unique_ptr
    // 管理的数组 我们需要创建一个新的 shared_ptr 来持有数据的副本
    // TODO: 创建一个新的 shared_ptr，拷贝最新数据
    // 最新数据的位置：(write_index_ - 1 + config_->buffer_size) %
    // config_->buffer_size [你的代码在这里]
    return std::make_shared<ImuRawData>(
        this->buffer_[(this->write_index_ - 1 + this->config_->buffer_size) %
                      this->config_->buffer_size]);
    // 直接make，同时传入的是值，也就是用这个临时拷贝的对象，右值引用过去
    // return nullptr; // 临时返回
  }

  // 注册到外部系统
  void registerToSystem() {
    // TODO: 使用 shared_from_this() 获取自身的 shared_ptr
    auto p = shared_from_this();
    // 打印 "Processor registered with buffer size: " << config_->buffer_size
    std::cout << "Processor registered with buffer size: "
              << p->config_->buffer_size << std::endl;
    // [你的代码在这里]
  }

  // 获取当前缓冲区数据量
  int getCount() const { return count_; }
};

// 任务二：测试主流程
int main() {
  std::cout << "=== Task: Integrated IMU Processor ===" << std::endl;

  // 1. 创建共享配置
  auto config = std::make_shared<ProcessorConfig>();
  config->buffer_size = 5;  // 测试用小缓冲区

  // 2. 创建处理器
  // TODO: 使用 make_shared 创建 ImuProcessor，传入 config
  // [你的代码在这里]
  auto processor = std::make_shared<ImuProcessor>(config);

  // 3. 注册到系统
  // TODO: 调用 registerToSystem()
  // [你的代码在这里]
  processor->registerToSystem();

  // 4. 模拟数据流
  std::cout << "\n--- Pushing Data ---" << std::endl;
  for (int i = 0; i < 8; ++i) {
    ImuRawData data{i * 1.0f, 0, 9.8f, 0, 0, 0, i * 10};
    // TODO: 调用 pushData 插入数据
    // [你的代码在这里]
    processor->pushData(data);

    std::cout << "Pushed data with timestamp: " << i * 10
              << ", Buffer count: " << processor->getCount() << std::endl;
  }

  // 5. 获取最新数据
  std::cout << "\n--- Getting Latest Data ---" << std::endl;
  // TODO: 调用 getLatestData() 并打印 timestamp
  // [你的代码在这里]
  {
    auto lastestData = processor->getLatestData();
    std::cout << "timestamp: " << lastestData->timestamp << std::endl;
  }
  return 0;
}
