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

#include "Hardware/TTC.h"

#include "Memory.h"

#define ZYNQUSP_FANCONTROLLER_PWM_SPEED_HERTZ 20000

namespace ZynqUSP{

    class FanController {
        volatile TTC *ttc;
        uint8_t channel;

        void SetupTimmer();

    public:
        FanController();
        ~FanController();

        void Initalize(TTC* tcc, uint8_t channel);

        void SetSpeed(uint8_t percent);
    };

};