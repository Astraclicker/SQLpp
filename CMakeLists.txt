cmake_minimum_required(VERSION 3.16)

project(SQLpp)

if (MSVC)
    add_compile_options(/utf-8)
else ()
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif ()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

#导入MySQL包
find_package(mysql-concpp REQUIRED)

# 设置SQLite路径
set(SQLite_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/SQLiteCpp)

# 设置Redis路径
set(Redis_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/redis-plus-plus)
# 设置Hiredis路径
set(Hiredis_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/hiredis)

# 关闭lib测试输出
set(REDIS_PLUS_PLUS_BUILD_TEST OFF CACHE BOOL "Build tests for redis++" FORCE)
set(DISABLE_TESTS ON CACHE BOOL "Disable hiredis tests" FORCE)

add_subdirectory(src)
