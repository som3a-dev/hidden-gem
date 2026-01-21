cmake_minimum_required(VERSION 3.0)
project(map_format C)

set(CMAKE_C_STANDARD 99)

# Include directory
set(PROJECT_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/include/")

# All .c source files
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/*.c")

# Define a static library target
add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})

# Include directories (PUBLIC so other targets can see it)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDE})

# Compiler warnings
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()