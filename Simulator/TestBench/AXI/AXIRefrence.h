// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * AXIRefrence
 * rainbain
 * 7/16/2023
 * Orca Emulator
 *
 * A refrence to some AXI port passed around by the software.
 * Also contains data strucutres and enums used by AXI
*/

#pragma once

#include <stdint.h>

struct AXIHostReadIFRefrence {
    uint8_t* resetn;
    uint8_t* clk;

    uint32_t *araddrm;
    uint8_t* arburstm;
    uint8_t* arlenm;
    uint8_t* arreadym;
    uint8_t* arsizem;
    uint8_t* arvalidm;

    uint32_t* rdatam;
    uint8_t* rreadym;
    uint8_t* rlastm;
    uint8_t* rrespm;
    uint8_t* rvalidm;
};

struct AXILiteDeviceRefrence {
    uint8_t* resetn;
    uint8_t* clk;

    uint32_t* araddr;
    uint8_t* arvalid;
    uint8_t* arready;

    uint32_t* rdata;
    uint8_t* rresp;
    uint8_t* rvalid;
    uint8_t* rready;

    uint32_t* awaddr;
    uint8_t* awvalid;
    uint8_t* awready;

    uint32_t* wdata;
    uint8_t* wstrb;

    uint8_t* bresp;
    uint8_t* bvalid;
    uint8_t* bready;
};

enum class AXIBurst {
    FIXED = 0,
    INCRAMENT,
    WRAP
};

enum class AXIResponce {
    OKAY = 0,
    EXCLUSIVE_OKAY,
    SLAVE_ERROR,
    DECODE_ERROR
};