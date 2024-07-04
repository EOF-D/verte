cmake_minimum_required(VERSION 3.16.3)
project(vertec)

# Set the install prefix
if(DEFINED ENV{DEVENV_PROFILE})
    set(DEFAULT_INSTALL_PREFIX "$ENV{DEVENV_PROFILE}")
    message(STATUS "A devenv environment was detected.")
elseif(DEFINED ENV{HOME})
    set(DEFAULT_INSTALL_PREFIX "$ENV{HOME}/.local")
    message(STATUS "A Non-devenv environment was detected.")
else()
    set(DEFAULT_INSTALL_PREFIX "/usr/local")
    message(STATUS "Falling back to /usr/local for installation prefix.")
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${DEFAULT_INSTALL_PREFIX}" CACHE PATH "Installation prefix" FORCE)
endif()

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

if (INSTALL_VERTE)
    install(TARGETS ${PROJECT_NAME} DESTINATION bin)
    install(TARGETS VerteLib DESTINATION lib)
    install(FILES ${HEADERS} DESTINATION include)
    message(STATUS "Verte will be installed to: ${CMAKE_INSTALL_PREFIX}")
endif()
