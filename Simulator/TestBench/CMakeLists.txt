add_library( TestBench
	TestBench.cpp
)

target_include_directories(TestBench PUBLIC ${${GENERATED_SOURCES}})