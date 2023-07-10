cmake_minimum_required(VERSION 3.25)

project(Simulator)

set(CMAKE_BUILD_TYPE Debug)

find_package(verilator HINTS $ENV{VERILATOR_ROOT} ${VERILATOR_ROOT})

add_executable(Testing Testing.cpp)

add_subdirectory("CommandInterface")
add_subdirectory("TestBench")

verilate(Testing TestBench
  TRACE
  INCLUDE_DIRS ../RTL/Flipper/Control
  INCLUDE_DIRS ../RTL/Flipper/XF
  SOURCES ../RTL/Flipper/FlipperTop.v
          )

set(VDIR "${CMAKE_BINARY_DIR}/CMakeFiles/Testing.dir/VFlipperTop.dir" "${VERILATOR_ROOT}/include" "${VERILATOR_ROOT}/include/vltstd")
target_include_directories(TestBench PRIVATE ${VDIR})

target_link_libraries(Testing PRIVATE TestBench CommandInterface)