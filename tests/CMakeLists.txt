cmake_minimum_required(VERSION 3.16)


list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)

# Gather source files
file(GLOB TEST_SOURCES "*.cpp")

file(GLOB EXTERNAL_SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

# Create a single test binary
add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests PRIVATE lisp bolt_vm GTest::gtest)
message(STATUS "Test sources: ${TEST_SOURCES}")

include(GoogleTest)
gtest_discover_tests(tests)
