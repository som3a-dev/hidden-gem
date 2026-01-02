cmake_minimum_required(VERSION 3.0)
project(level_editor C)
set(CMAKE_C_STANDARD 99)

# Adding Raylib
include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games

FetchContent_Declare(
    raylib
    GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
    GIT_TAG "master"
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(raylib)

add_library(nk_impl STATIC
    source/nuklear_impl.c)

target_include_directories(nk_impl PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/source
)

target_link_libraries(nk_impl PRIVATE raylib)

# Adding our source files
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/source/*.c") # Define PROJECT_SOURCES as a list of all source files
list(FILTER PROJECT_SOURCES
     EXCLUDE REGEX ".*/nuklear_impl\\.c$")

set(PROJECT_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/source/") # Define PROJECT_INCLUDE to be the path to the include directory of the project

# Declaring our executable
add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDE})
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
target_link_libraries(${PROJECT_NAME} PRIVATE nk_impl)

# Setting ASSETS_PATH
target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/") # Set the asset path macro to the absolute path on the dev machine
#target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="./assets") # Set the asset path macro in release mode to a relative path that assumes the assets folder is in the same directory as the game executable

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
  target_compile_options(nk_impl PRIVATE /W0)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()