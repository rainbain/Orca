cmake_minimum_required(VERSION 3.25)

project(Simulator)

# Build FlipperAPI with simulator
set(SIMULATOR_ENABLED ON)

set(CMAKE_BUILD_TYPE Debug)

find_package(verilator HINTS $ENV{VERILATOR_ROOT} ${VERILATOR_ROOT})

add_executable(Testing Testing.cpp)

add_subdirectory(TestBench)
add_subdirectory(Flipper)

verilate(Testing TestBench
  TRACE
  INCLUDE_DIRS ../RTL/Flipper/CP
  INCLUDE_DIRS ../RTL/Flipper/XF
  INCLUDE_DIRS ../RTL/Flipper/
  SOURCES ../RTL/Flipper/SimulatorRapper.v
          )

set(VDIR "${CMAKE_BINARY_DIR}/CMakeFiles/Testing.dir/VSimulatorRapper.dir" "${VERILATOR_ROOT}/include" "${VERILATOR_ROOT}/include/vltstd")
target_include_directories(TestBench PRIVATE ${VDIR})

target_link_libraries(Testing PRIVATE TestBench Flipper)