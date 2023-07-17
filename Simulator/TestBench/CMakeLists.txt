add_library( TestBench
TestBench.cpp
AXI/AXIReadIF.cpp
)

target_include_directories(TestBench PUBLIC ${${GENERATED_SOURCES}})