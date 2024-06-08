cmake_minimum_required(VERSION 3.16.3)
project(vertec)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the build type to Release if not specified
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Set the include directory
include_directories(include)

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS include/*.hpp include/*.h)

add_library(VerteLib ${SOURCES} ${HEADERS})

# Find LLVM and link
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})
target_link_libraries(VerteLib LLVM)

# Option to enable/disable unit testing
option(BUILD_TESTS "Build unit tests" OFF)

# If unit tests are enabled, add the tests directory
if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} VerteLib)
