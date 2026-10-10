project(SQLite++)

add_library(
        ${PROJECT_NAME}
        STATIC
        SQLitepp.cpp
)

target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        ${INCLUDE_DIR}
)

target_link_libraries(
        ${PROJECT_NAME}
        PUBLIC
        SQLiteCpp
)

add_subdirectory(${SQLite_DIR} ${CMAKE_BINARY_DIR}/SQLpp/SQLiteCpp)
