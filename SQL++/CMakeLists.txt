cmake_minimum_required(VERSION 4.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(SQL++)

# 设置SQLite路径
set(SQLite_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/SQLiteCpp)

# 设置include路径
include_directories(include)

add_subdirectory(SQLite++)

