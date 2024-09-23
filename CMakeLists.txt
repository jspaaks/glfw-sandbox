cmake_minimum_required(VERSION 3.21...3.28)

project(project LANGUAGES C)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set_property(CACHE CMAKE_INSTALL_PREFIX PROPERTY VALUE "${CMAKE_BINARY_DIR}/dist")
endif()

add_executable(tgt)

set_target_properties(tgt PROPERTIES OUTPUT_NAME "demo")

target_compile_definitions(
    tgt
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
)
target_compile_features(
    tgt
    PRIVATE
        c_std_23
)
target_compile_options(
    tgt
    PRIVATE
        -Wall
        -Wextra
        -pedantic
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Debug>:-O0>
        $<$<CONFIG:Release>:-Werror>
)
target_include_directories(
    tgt
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/thirdparty
)
target_link_libraries(
    tgt
    PRIVATE
        m
        glfw
        GL
)
target_sources(
    tgt
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/src/main.c
)

install(TARGETS tgt)
