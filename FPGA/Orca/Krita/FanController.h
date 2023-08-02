// Copyright 2023 Orca Hardware Emulator
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * FanController
 * rainbain
 * 7/28/2023
 * Orca Emulator
 *
 * CPU Tripple Timmer is used to control the system fan.
*/

#pragma once

#include "Hardware/AXITimmer.h"

#include "Memory.h"

namespace ZynqUSP{

    class FanController {
        volatile AXITimmer *timmer;

        uint8_t channel;

        void SetupTimmer();

    public:
        FanController(AXITimmer* timmer);
        ~FanController();

        void Update();
        void SetSpeed(uint8_t percent);
    };

};