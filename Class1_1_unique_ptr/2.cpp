#include <iostream>
#include <memory>

// IMU 数据结构定义
struct ImuRawData
{
    float ax, ay, az; // 加速度
    float gx, gy, gz; // 角速度
    long long timestamp;
};

// 1. 数据生产者：生成一个 IMU 数据包
// TODO: 定义函数签名，返回一个持有 ImuRawData 的 unique_ptr
// [你的代码在这里]
std::unique_ptr<ImuRawData> createImuData(float ax = 0,
                                          float ay = 0,
                                          float az = 0,
                                          float gx = 0,
                                          float gy = 0,
                                          float gz = 0,
                                          long long timestamp = 0);

// 2. 数据消费者：处理 IMU 数据包
// 要求：该函数需要“夺走”数据的所有权，处理完后自动销毁数据
// TODO: 定义函数签名，参数为 unique_ptr
// [你的代码在这里]
void processImuData(std::unique_ptr<ImuRawData> raw_data);

// 3. 主流程
int main()
{
    // 场景 A: 基础创建与访问
    // 创建一个 ImuRawData 对象，并初始化 ax=0.1, ay=0.2, az=9.8
    // TODO: 使用 make_unique 创建 pImu
    // [你的代码在这里]
    std::unique_ptr<ImuRawData> pImu = createImuData(0.1, 0.2, 9.8); // 生产者函数生产

    std::cout << "Accel X: " << pImu->ax << std::endl;

    // 场景 B: 所有权转移
    // 将 pImu 的所有权转移给另一个指针 pImuMoved
    // TODO: 执行移动语义
    // [你的代码在这里]
    std::unique_ptr<ImuRawData> pImuMoved = std::move(pImu); // 独占指针不可copy但可move

    if (!pImu)
    {
        std::cout << "Scene B: Original pointer is empty." << std::endl;
    }

    // 场景 C: 函数参数传递
    // 调用消费者函数处理 pImuMoved
    // TODO: 调用 processImuData 函数
    // [你的代码在这里]
    processImuData(std::move(pImuMoved)); // 因为是独占指针,所以只能移交所有权

    return 0;
}

// 函数实现部分
// [createImuData 实现请写在这里]
std::unique_ptr<ImuRawData> createImuData(float ax,
                                          float ay,
                                          float az,
                                          float gx,
                                          float gy,
                                          float gz,
                                          long long timestamp)
{
    return std::make_unique<ImuRawData>(
        ImuRawData{
            ax, ay, az,
            gx, gy, gz,
            timestamp});
}

// [processImuData 实现请写在这里，内部简单打印 timestamp 即可]
void processImuData(std::unique_ptr<ImuRawData> raw_data)
{
    std::cout << "timestamp: " << raw_data->timestamp << std::endl;
}
