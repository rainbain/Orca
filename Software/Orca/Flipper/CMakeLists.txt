add_subdirectory(GX)

add_library(Flipper STATIC
    DebugUtils.cpp
)

target_link_libraries(Flipper PRIVATE GX)