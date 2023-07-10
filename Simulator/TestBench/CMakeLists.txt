add_library( TestBench
	TestBench.cpp
	TbServer.cpp
	TbCommandInterface.cpp
)

target_include_directories(TestBench PUBLIC ${${GENERATED_SOURCES}})