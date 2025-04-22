cmake_minimum_required(VERSION 3.10)

project(GetSec)

set(CMAKE_CXX_STANDARD 23)
add_executable(getsec main.cpp)

add_executable(test_target test_code.cpp)

# Add the main executable as a test
enable_testing()
add_test(NAME getsec_test COMMAND $<TARGET_FILE:getsec> $<TARGET_FILE:test_target>)
