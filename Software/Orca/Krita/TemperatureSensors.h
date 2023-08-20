// Copyright 2023 Orca Hardware Emulator
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * TemperatureSensors
 * rainbain
 * 8/1/2023
 * Orca Emulator
 *
 * Reads temperature data from linux.
 * Also contains the code to maintain a thermal graph
*/

#pragma once

#include <string>
#include <vector>
#include <stdint.h>

#include "../nlohmann/json.hpp"

namespace ZynqUSP {
    class ThermalGraph {
            struct ThermalPoint {
                float temp;
                uint8_t fan;
            };

            std::string getPointName(size_t number);


            std::vector<ThermalPoint> thermalPoints;
        public:
            ThermalGraph();

            uint8_t GetPoint(float p);

            void LoadConfig(nlohmann::json j);
    };

    class TemperatureSensors {
        std::string thermalZone0;
        std::string thermalZone1;
        ThermalGraph graph;

        float ReadZone(std::string thermalZone);

    public:
        TemperatureSensors(std::string temeratureZonesPath = "/sys/class/hwmon/hwmon1/");
        ~TemperatureSensors();

        void LoadConfig(nlohmann::json j);

        float GetMaximum();
        uint8_t GetFanSpeed();
    };
};