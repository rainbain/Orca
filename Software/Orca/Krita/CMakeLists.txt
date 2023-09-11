add_library(Krita STATIC
    TrafficGenerator.cpp
    FanController.cpp
    Memory.cpp
    TemperatureSensors.cpp
    System.cpp
)

#target_link_libraries(Krita PRIVATE rt)