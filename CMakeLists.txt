cmake_minimum_required(VERSION 4.1)

project(example)

#配置输出
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/$<CONFIG>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/$<CONFIG>)


add_subdirectory(example)
add_subdirectory(SQL++)
