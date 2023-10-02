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

#define AXI_CAST(x) ((uint32_t)(uint64_t)(x))

struct AXIHostRefrence {
    uint8_t* resetn;
    uint8_t* clk;

    uint32_t *awaddr;
    uint8_t* awlen;
    uint8_t* awsize;

    uint8_t* awburst;
    uint8_t* awvalid;
    uint8_t* awready;

    uint32_t* wdata;
    uint16_t* wstrb;
    uint8_t* wlast;
    uint8_t* wvalid;
    uint8_t* wready;

    uint8_t* bresp;
    uint8_t* bvalid;
    uint8_t* bready;

    uint32_t* araddr;
    uint8_t* arlen;
    uint8_t* arsize;
    uint8_t* arburst;
    uint8_t* arvalid;
    uint8_t* arready;

    uint32_t* rdata;
    uint8_t* rresp;
    uint8_t* rlast;

    uint8_t* rvalid;
    uint8_t* rready;
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
    uint8_t* wvalid;
    uint8_t* wready;

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

extern const uint8_t AXISizeLookup[];
extern const uint8_t AXI16StrobeLookup[];

typedef void (*AXIWrite)(uint32_t addr, uint32_t data, uint8_t strobe);
typedef uint32_t (*AXIRead)(uint32_t addr);