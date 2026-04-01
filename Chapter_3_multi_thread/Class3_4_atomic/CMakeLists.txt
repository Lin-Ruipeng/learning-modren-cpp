cmake_minimum_required(VERSION 3.20)

# 1. 明确声明项目名称，并指明语言为 CXX (C++)，这能省去 C 语言编译器的无用探测
project(ImuPredictApp LANGUAGES CXX)

# 1. 极其关键：强制开启 C++17 护城河
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 关闭 GNU 扩展 (如 -std=gnu++17)，强制使用纯净标准 (-std=c++17)，保证跨平台绝对安全
set(CMAKE_CXX_EXTENSIONS OFF)

# 1. 选项全局置顶声明，并增加项目前缀防止命名污染
option(
    IMU_APP_BUILD_EXAMPLE
    "Build the example.cpp executable"
    OFF)
option(
    IMU_APP_BUILD_T
    "Build main.cpp executable"
    OFF)

# 1. 清爽的业务路由逻辑
if(IMU_APP_BUILD_EXAMPLE)
    add_executable(example src/example.cpp)
endif()

if(IMU_APP_BUILD_T)
    add_executable(app src/main.cpp)
endif()

# 优秀的兜底意识
if(NOT IMU_APP_BUILD_T AND NOT IMU_APP_BUILD_EXAMPLE)
    message(FATAL_ERROR "请至少指定 IMU_APP_BUILD_T=ON 或 IMU_APP_BUILD_EXAMPLE=ON!")
endif()
