cmake_minimum_required(VERSION 3.20)
project(RMP VERSION 2.0)

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

set(RMP_include_dirs "${PROJECT_SOURCE_DIR}/include/")
add_library(search "${PROJECT_SOURCE_DIR}/source/rmp-all.cpp")
target_include_directories(search PRIVATE ${RMP_include_dirs})

if (MSVC)
	set(gtest_force_shared_crt ON)
endif()

add_subdirectory("googletest")
add_subdirectory("test")
