// Copyright 2023 Orca Hardware Emulator
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * AXITrafficGenerator
 * rainbain
 * 9/8/2023
 * Orca Emulator
 *
 * Core to generate AXI traffic to test AXI interfaces.
*/

#pragma once

#include <stdint.h>

namespace ZynqUSP {
    struct AXITrafficGeneratorCMD {
        uint32_t address;
        uint32_t settings1;
        uint32_t settings2;
        uint32_t settings3;
    };

    struct AXITrafficGenerator{
        uint32_t masterControl; // 0000 - 0003
        uint32_t slaveControl;  // 0004 - 0007
        uint32_t errorStatus;   // 0008 - 000B
        uint32_t errorEnable;   // 000C - 000F
        uint32_t masterInterruptEnable; // 0010 - 0013
        uint32_t configStatus; // 0014 - 00
        uint32_t reserved3[0x27]; // 0018 - 00B3
        uint32_t slaveError; // 00B4 - 00B7

        uint32_t reserved1[0x3D2]; // 0000 - 0FFF

        uint32_t readParameters[0x100];  // 1000 - 13FF
        uint32_t writeParameters[0x100]; // 1400 - 17FF

        uint32_t reserved2[0x1A00]; // 1800 - 7FFF

        AXITrafficGeneratorCMD readCommands[0x100];  // 8000 - 8FFF
        AXITrafficGeneratorCMD writeCommands[0x100]; // 9000 - 9FFF

        uint32_t readAddresses[0x100]; //A000 - A3FF
        uint32_t writeAddresses[0x100];//A400 - A7FF

        uint32_t reserved4[0x600];

        uint32_t masterRam[0x200];
    };
}