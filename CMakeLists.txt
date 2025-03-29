cmake_minimum_required(VERSION 3.0)
project(particl)
# set(CMAKE_C_STANDARD 99)

SET(COMPILE_FLAGS "-g")

# Adding Raylib
include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games

FetchContent_Declare(
    bgfx
    GIT_REPOSITORY "https://github.com/bkaradzic/bgfx.cmake.git"
    GIT_TAG "master"
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(bgfx)

FetchContent_Declare(
    kompute
    GIT_REPOSITORY "https://github.com/KomputeProject/kompute.git"
    GIT_TAG "master"
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(kompute)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY "https://github.com/glfw/glfw.git"
    GIT_TAG "master"
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(glfw)

include_directories(${kompute_SOURCE_DIR}/src/include)
include_directories(${bgfx_SOURCE_DIR}/bgfx/include)
include_directories(${bgfx_SOURCE_DIR}/bx/include)
include_directories(${glfw_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/)

vulkan_compile_shader(
    INFILE src/shader.comp
    OUTFILE src/shader.hpp
    NAMESPACE "shader")

add_library(shader INTERFACE "src/shader.hpp")
target_include_directories(shader INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

bgfx_compile_shaders(
  TYPE VERTEX
  SHADERS ${CMAKE_SOURCE_DIR}/src/shader.vs
  VARYING_DEF ${CMAKE_SOURCE_DIR}/src/varying.def.sc
  OUTPUT_DIR ${CMAKE_SOURCE_DIR}
)
bgfx_compile_shaders(
  TYPE FRAGMENT
  VARYING_DEF ${CMAKE_SOURCE_DIR}/src/varying.def.sc
  SHADERS ${CMAKE_SOURCE_DIR}/src/shader.fs
  OUTPUT_DIR ${CMAKE_SOURCE_DIR}
)

# Adding our source files
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp") # Define PROJECT_SOURCES as a list of all source files
set(PROJECT_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/src/") # Define PROJECT_INCLUDE to be the path to the include directory of the project

# Declaring our executable
add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCES} ${CMAKE_SOURCE_DIR}/src/shader.fs ${CMAKE_SOURCE_DIR}/src/shader.vs)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDE})
target_link_libraries(${PROJECT_NAME} PRIVATE bgfx)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE shader kompute::kompute)
target_compile_options(particl PRIVATE -g)

# Setting ASSETS_PATH
target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/") # Set the asset path macro to the absolute path on the dev machine
#target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="./assets") # Set the asset path macro in release mode to a relative path that assumes the assets folder is in the same directory as the game executable

message(${bgfx_SOURCE_DIR}/bgfx/src/include)