cmake_minimum_required(VERSION 3.16)

project(earleycpp)

add_compile_options(-pedantic -Werror -Wextra -fsanitize=address -fsanitize=undefined -std=c++17)
add_link_options(-pedantic -Werror -Wextra -fsanitize=address -fsanitize=undefined -std=c++17)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
#set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks=-*,readability-*)
add_executable(earleycpp main.cpp tests.cpp)
enable_testing()

add_test(NAME earleycpp COMMAND ${Testing_SOURCE_DIR}/bin/testing)

target_link_libraries(earleycpp Threads::Threads ${GTEST_LIBRARIES} ${GMOCK_BOTH_LIBRARIES})
