// Copyright 2023 Orca Hardware Emulator
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * TemperatureSensors
 * rainbain
 * 8/1/2023
 * Orca Emulator
 *
 * Reads temperature data from linux.
*/

#pragma once

#include <string>
#include <stdint.h>

namespace ZynqUSP {
    class TemperatureSensors {
        std::string thermalZone0;
        std::string thermalZone1;

        float ReadZone(std::string thermalZone);

    public:
        TemperatureSensors(std::string temeratureZonesPath = "/sys/class/hwmon/hwmon1/");
        ~TemperatureSensors();

        float GetMaximum();
    };
};