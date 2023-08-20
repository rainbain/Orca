// Copyright 2023 Orca Hardware Emulator
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * AXITimmer
 * rainbain
 * 7/28/2023
 * Orca Emulator
 *
 * Xilinx AXI Timmer Implmentation
*/
#pragma once

#include <stdint.h>

namespace ZynqUSP {
    struct AXITimmer {
        uint32_t tim0_ctrl;
        uint32_t tim0_load;
        uint32_t tim0_count;
        uint32_t tim0_reserved;
        uint32_t tim1_ctrl;
        uint32_t tim1_load;
        uint32_t tim1_count;
        uint32_t tim1_reserved;
    };
};