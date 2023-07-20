add_library( TestBench
TestBench.cpp
AXI/AXIReadIF.cpp
AXI/AXILiteIF.cpp
)

target_include_directories(TestBench PUBLIC ${${GENERATED_SOURCES}})