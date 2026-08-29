cmake_minimum_required(VERSION 4.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(SQL++)

# 设置MySQL路径（按构建配置区分 Release/Debug）
#mysql_release
set(MySQL_INCLUDE_DIR_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/lib/mysql_release/include/jdbc)
set(MySQL_LIB_DIR_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/lib/mysql_release/lib64/vs14)
set(MySQL_DLL_DIR_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/lib/mysql_release/lib64)
#mysql_debug
set(MySQL_INCLUDE_DIR_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/lib/mysql_debug/include/jdbc)
set(MySQL_LIB_DIR_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/lib/mysql_debug/lib64/debug/vs14)
set(MySQL_DLL_DIR_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/lib/mysql_debug/lib64/debug)

# OpenSSL DLL 两个版本都在 lib64 根目录下
set(MySQL_SSL_DLL_DIR_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/lib/mysql_release/lib64)
set(MySQL_SSL_DLL_DIR_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/lib/mysql_debug/lib64)

# Debug 用 debug 版本
set(MySQL_INCLUDE_DIR
    $<$<CONFIG:Debug>:${MySQL_INCLUDE_DIR_DEBUG}>
    $<$<NOT:$<CONFIG:Debug>>:${MySQL_INCLUDE_DIR_RELEASE}>
)
set(MySQL_LIB_DIR
    $<$<CONFIG:Debug>:${MySQL_LIB_DIR_DEBUG}>
    $<$<NOT:$<CONFIG:Debug>>:${MySQL_LIB_DIR_RELEASE}>
)
set(MySQL_DLL_DIR
    $<$<CONFIG:Debug>:${MySQL_DLL_DIR_DEBUG}>
    $<$<NOT:$<CONFIG:Debug>>:${MySQL_DLL_DIR_RELEASE}>
)

# 设置include路径
include_directories(include)

add_subdirectory(MySQL++)
