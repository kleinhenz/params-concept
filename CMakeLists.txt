cmake_minimum_required(VERSION 3.8)

project(params CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED)

add_executable(main main.cpp)
target_link_libraries(main Boost::boost)
