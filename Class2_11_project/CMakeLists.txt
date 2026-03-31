cmake_minimum_required(VERSION 3.20)

project(myapp)

set(SRC "src/main.cpp")

add_executable(app ${SRC})

# 启用 C++20 (因为要用std::erase_if)
target_compile_features(app PRIVATE cxx_std_20)
