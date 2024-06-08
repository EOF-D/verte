include_directories(${CMAKE_SOURCE_DIR}/include)

# Find GTest and set it up
find_package(GTest REQUIRED)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Set test files
file(GLOB_RECURSE TEST_FILES
  verte/*.cpp 
)

add_executable(verte-tests ${TEST_FILES})

# Linking
target_link_libraries(verte-tests GTest::gtest_main VerteLib)

include(GoogleTest)
gtest_discover_tests(verte-tests)
