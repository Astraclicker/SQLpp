project(MySQL++)

add_library(
        ${PROJECT_NAME}
        STATIC
        MySQLpp.cpp
)


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