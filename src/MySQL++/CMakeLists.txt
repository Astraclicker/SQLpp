project(MySQL++)

add_library(
        ${PROJECT_NAME}
        INTERFACE
        MySQLpp.h
)

target_include_directories(
        ${PROJECT_NAME}
        INTERFACE
        ${INCLUDE_DIR}
        ${MYSQL_CONCPP_INCLUDE_DIR}/jdbc
)

target_link_libraries(
        ${PROJECT_NAME}
        INTERFACE
        mysql::concpp-jdbc
)