cmake_minimum_required(VERSION 3.16.3)
project(exerptc)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set the build type to Release if not specified
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Set the include directory
include_directories(include/excerpt)
include_directories(include/excerpt_utils)

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS 
  include/excerpt/*.hpp
  include/excerpt/*.h
  include/excerpt_utils/*.hpp
  include/excerpt_utils/*.h)

add_library(ExcerptLib ${SOURCES} ${HEADERS})

# Option to enable/disable unit testing
option(BUILD_TESTS "Build unit tests" OFF)

# If unit tests are enabled, add the tests directory
if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} ExcerptLib)
