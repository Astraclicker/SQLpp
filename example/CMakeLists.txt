project(test_main)

add_executable(
        ${PROJECT_NAME}
        main.cpp
)

target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        ${CMAKE_SOURCE_DIR}/SQL++
)

target_link_libraries(
        ${PROJECT_NAME}
        PUBLIC
        SQLite++
)


