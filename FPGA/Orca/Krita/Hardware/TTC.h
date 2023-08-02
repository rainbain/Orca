// Copyright 2023 Orca Hardware Emulator
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * TTC
 * rainbain
 * 7/28/2023
 * Orca Emulator
 *
 * Zynq Ultrascale+ MoSoC Tripple Timmer Counter Orca Implmentation
*/
#pragma once

#include <stdint.h>

namespace ZynqUSP {
    struct TTC {
        uint32_t clk_ctrl[3];
        uint32_t cnt_ctrl[3];
        uint32_t cnt_val[3];
        uint32_t interval_cnt[3];
        uint32_t match0[3];
        uint32_t match1[3];
        uint32_t match2[3];
        uint32_t isr[3];
        uint32_t ier[3];
        uint32_t evt_ctrl[3];
        uint32_t evt_cycle[3];
    };
};