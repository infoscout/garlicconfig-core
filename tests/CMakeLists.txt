cmake_minimum_required(VERSION 3.12)
project(GarlicConfigTest)

find_package(GTest REQUIRED)

set(TEST_SOURCES test_repositories.cpp)

add_executable(GarlicConfigTest ${TEST_SOURCES})
target_link_libraries(GarlicConfigTest GarlicConfig ${GTEST_BOTH_LIBRARIES})