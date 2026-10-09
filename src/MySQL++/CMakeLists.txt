project(MySQL++)

add_library(
        ${PROJECT_NAME}
        SHARED
        MySQLpp.cpp
)

# Windows 导出符号（Linux/macOS 上该属性无效果，忽略）
if (WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif ()

target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        ${INCLUDE_DIR}
        ${MYSQL_CONCPP_INCLUDE_DIR}/jdbc
)

target_link_libraries(
        ${PROJECT_NAME}
        PUBLIC
        mysql::concpp-jdbc
)