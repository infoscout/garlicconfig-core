cmake_minimum_required(VERSION 3.11)
project(GarlicConfigTest)

find_package(GTest REQUIRED)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(TEST_SOURCES main.cpp test_repositories.cpp test_layer.cpp)

add_executable(GarlicConfigTest ${TEST_SOURCES})
target_link_libraries(GarlicConfigTest GarlicConfig Threads::Threads ${GTEST_BOTH_LIBRARIES})
