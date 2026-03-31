cmake_minimum_required(VERSION 3.20)

project(app)

option(EXAMPLE "build example.cpp" OFF)

if (EXAMPLE)
    set(SRC "./src/example.cpp")
    add_executable(example ${SRC})
else()
    set(SRC "./src/main.cpp")
    add_executable(app ${SRC})
endif()
