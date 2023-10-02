// Copyright 2023 Orca Hardware Emulator
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * AXIGPIO
 * rainbain
 * 10/1/2023
 * Orca Emulator
 *
 * Xilinx AXI GPIO Implmentation
*/
#pragma once

#include <stdint.h>

namespace ZynqUSP {
    struct AXIGPIOBlock {
        uint32_t data;
        uint32_t tristate;
    };

    struct AXIGPIO {
        AXIGPIOBlock gpio[2];
    };
};