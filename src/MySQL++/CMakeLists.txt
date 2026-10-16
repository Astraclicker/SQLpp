project(MySQL++)

add_library(
        ${PROJECT_NAME}
        INTERFACE
        MySQLpp.h
)

target_include_directories(
        ${PROJECT_NAME}
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
        ${MYSQL_CONCPP_INCLUDE_DIR}/jdbc
)

target_link_libraries(
        ${PROJECT_NAME}
        INTERFACE
        mysql::concpp-jdbc
)

target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})