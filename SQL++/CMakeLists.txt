cmake_minimum_required(VERSION 4.1)

project(SQL++)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)




# 设置Redis路径
set(Redis_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/redis-plus-plus)

# 设置Hiredis路径
set(Hiredis_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/hiredis)

# 设置include路径
include_directories(include)

set(REDIS_PLUS_PLUS_BUILD_TEST OFF CACHE BOOL "Build tests for redis++" FORCE)
set(DISABLE_TESTS ON CACHE BOOL "Disable hiredis tests" FORCE)

add_subdirectory(Redis++)
