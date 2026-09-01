project(MySQL++)

add_library(
    ${PROJECT_NAME}
    SHARED
    MySQLpp.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ${MySQL_INCLUDE_DIR}
)

target_link_directories(
    ${PROJECT_NAME}
    PUBLIC
    ${MySQL_LIB_DIR}
)

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
    mysqlcppconn
)

# 构建后自动将 MySQL 依赖 DLL 复制到可执行文件输出目录
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<IF:$<CONFIG:Debug>,${MySQL_DLL_DIR_DEBUG},${MySQL_DLL_DIR_RELEASE}>/mysqlcppconn-10-vs14.dll
    $<IF:$<CONFIG:Debug>,${MySQL_SSL_DLL_DIR_DEBUG},${MySQL_SSL_DLL_DIR_RELEASE}>/libssl-3-x64.dll
    $<IF:$<CONFIG:Debug>,${MySQL_SSL_DLL_DIR_DEBUG},${MySQL_SSL_DLL_DIR_RELEASE}>/libcrypto-3-x64.dll
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
