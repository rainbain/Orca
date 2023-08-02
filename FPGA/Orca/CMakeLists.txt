cmake_minimum_required(VERSION 3.0)

project(Orca CXX)

# For GCC and Clang
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(Krita)

# Add the main.cpp source file to the executable
add_executable(Orca Orca.cpp)

target_link_libraries(Orca PRIVATE pthread Krita)