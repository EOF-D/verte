include_directories(${CMAKE_SOURCE_DIR}/include)

# Find GTest and set it up
find_package(GTest REQUIRED)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Set test files
file(GLOB_RECURSE TEST_FILES
  excerpt/*.cpp 
  excerpt_utils/*.cpp
)

add_executable(excerpt-tests ${TEST_FILES})

# Linking
target_link_libraries(excerpt-tests GTest::gtest_main ExcerptLib)

include(GoogleTest)
gtest_discover_tests(excerpt-tests)
