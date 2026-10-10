project(SQLite++)

add_library(
        ${PROJECT_NAME}
        INTERFACE
        SQLitepp.h
)

target_include_directories(
        ${PROJECT_NAME}
        INTERFACE
        ${INCLUDE_DIR}
)

target_link_libraries(
        ${PROJECT_NAME}
        INTERFACE
        SQLiteCpp
)

add_subdirectory(${SQLite_DIR} ${CMAKE_BINARY_DIR}/SQLpp/SQLiteCpp)
