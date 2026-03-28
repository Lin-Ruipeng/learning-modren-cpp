#include <iostream>
#include <memory>
#include <cstdlib> // for malloc, free

// IMU 数据结构
struct ImuSensorData
{
    float ax, ay, az;
    long long timestamp;

    // 自定义构造器(初始化列表)
    ImuSensorData(float ax = 0, float ay = 0, float az = 0, long long timestamp = 0) : ax(ax), ay(ay), az(az), timestamp(timestamp)
    {
    }
};

// 模拟数据处理函数
void processFilter(std::shared_ptr<ImuSensorData> data)
{
    if (!data)
        return;
    std::cout << "[Filter] Processing data at " << data->timestamp << std::endl;
    // 模拟处理...
}

// 模拟日志记录函数
void logData(std::shared_ptr<ImuSensorData> data)
{
    if (!data)
        return;
    std::cout << "[Logger] Logging data: " << data->ax << std::endl;
    // 模拟记录...
}

// 任务一：数据分发
// 要求：创建一个 shared_ptr，分别传递给 processFilter 和 logData
// 打印出引用计数的变化，观察共享行为
void task1_shared_ownership()
{
    std::cout << "--- Task 1: Shared Ownership ---" << std::endl;

    // 1. 创建一个 shared_ptr，初始化 timestamp 为 100, ax 为 9.8
    // TODO: 使用 make_shared 创建 pData
    std::shared_ptr<ImuSensorData> pData = std::make_shared<ImuSensorData>(9.8, 0, 0, 100);

    std::cout << "Count after creation: " << pData.use_count() << std::endl;

    // 2. 将数据传递给两个处理函数
    // 注意：思考这里是值传递还是引用传递？值传递会增加引用计数吗？
    // TODO: 调用 processFilter 和 logData
    std::cout << "Count before : " << pData.use_count() << std::endl; // 调用前打印
    // 应该是值传递，相当于Copy，会使得引用计数增加
    processFilter(pData);
    logData(pData);

    // 稍等，如果这里直接调用，函数返回后引用计数就会下降。
    // 请在调用前打印一次计数，调用后打印一次计数。
    std::cout << "Count after : " << pData.use_count() << std::endl; // 调用后打印
}

// 任务二：管理 C 风格资源
// 要求：使用 shared_ptr 管理由 malloc 分配的原始内存
// 场景：某些底层驱动返回的是 malloc 的内存，必须用 free 释放
void task2_custom_deleter()
{
    std::cout << "\n--- Task 2: Custom Deleter ---" << std::endl;

    // 模拟驱动分配的原始内存
    void *raw_buffer = malloc(1024);

    // TODO: 创建一个 shared_ptr<void> 来管理 raw_buffer
    // 提示：构造函数第二个参数传入 lambda 表达式调用 free()
    // [你的代码在这里]
    std::shared_ptr<void> buffer(
        raw_buffer, // 传入原指针
        [](void *p) // lambda释放
        {
            std::free(p);
            // p = nullptr; // 不用写,这里立马就结束了生命周期
        });
    raw_buffer = nullptr; // 防止悬垂

    std::cout << "Buffer managed by shared_ptr." << std::endl;
    // 当 pBuf 离开作用域时，应该自动调用 free()
}

int main()
{
    task1_shared_ownership();
    task2_custom_deleter();
    return 0;
}
