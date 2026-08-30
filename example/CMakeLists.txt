project(test_main)

add_executable(
    ${PROJECT_NAME}
    main.cpp
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ${CMAKE_SOURCE_DIR}/SQL++
    ${CMAKE_SOURCE_DIR}/SQL++/include
)

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
    Redis++
)


