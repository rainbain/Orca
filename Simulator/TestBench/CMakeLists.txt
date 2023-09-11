add_library( TestBench
TestBench.cpp
AXI/AXILiteIF.cpp
AXI/AXIHostIF.cpp
AXI/AXIRefrence.cpp
)

target_include_directories(TestBench PUBLIC ${${GENERATED_SOURCES}})