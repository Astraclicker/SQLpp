cmake_minimum_required(VERSION 4.1)

project(example)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#配置输出：按构建配置分别输出到 bin/<Config>
#（set() 不会展开生成器表达式 $<CONFIG>，必须用按配置区分的 *_OUTPUT_DIRECTORY_<CONFIG> 变量）
foreach(_cfg Debug Release RelWithDebInfo MinSizeRel)
    string(TOUPPER "${_cfg}" _CFG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${_CFG} ${PROJECT_SOURCE_DIR}/bin/${_cfg})
endforeach()
unset(_cfg)
unset(_CFG)



add_subdirectory(example)
add_subdirectory(SQL++)
