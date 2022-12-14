cmake_minimum_required(VERSION 3.23)
project(hw1_LRU_Cache)

set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)

FetchContent_MakeAvailable(googletest)

add_executable(hw1_LRU_Cache main.cpp)

enable_testing()

add_executable(
    lru_cache_test
    test/lru_cache_test.cc
    lru_cache/lru_cache.h)
target_link_libraries(
    lru_cache_test
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(lru_cache_test)
