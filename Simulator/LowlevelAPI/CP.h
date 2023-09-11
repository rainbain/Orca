// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * CP
 * rainbain
 * 9/2/2023
 * Orca Emulator
 *
 * Simulator CP low level API acess.
*/

#pragma once

#include "../TestBench/AXI/AXILiteIF.h"

#define CP_BASE_ADDRESS 0xCC000000

#define CP_SR_ADDR          (CP_BASE_ADDRESS + 0x000)
#define CP_CR_ADDR          (CP_BASE_ADDRESS + 0x002)
#define CP_CLEAR_ADDR       (CP_BASE_ADDRESS + 0x004)
#define CP_TOKEN_ADDR       (CP_BASE_ADDRESS + 0x00E)
#define CP_BBOX_LEFT_ADDR   (CP_BASE_ADDRESS + 0x010)
#define CP_BBOX_RIGHT_ADDR  (CP_BASE_ADDRESS + 0x012)
#define CP_BBOX_TOP_ADDR    (CP_BASE_ADDRESS + 0x014)
#define CP_BBOX_BOTTOM_ADDR (CP_BASE_ADDRESS + 0x016)
#define CP_FIFO_BASE_L_ADDR (CP_BASE_ADDRESS + 0x020)
#define CP_FIFO_BASE_H_ADDR (CP_BASE_ADDRESS + 0x022)
#define CP_FIFO_END_L_ADDR  (CP_BASE_ADDRESS + 0x024)
#define CP_FIFO_END_H_ADDR  (CP_BASE_ADDRESS + 0x026)
#define CP_FIFO_HWM_L_ADDR  (CP_BASE_ADDRESS + 0x028)
#define CP_FIFO_HWM_H_ADDR  (CP_BASE_ADDRESS + 0x02A)
#define CP_FIFO_LWM_L_ADDR  (CP_BASE_ADDRESS + 0x02C)
#define CP_FIFO_LWM_H_ADDR  (CP_BASE_ADDRESS + 0x02E)
#define CP_FIFO_WRD_L_ADDR  (CP_BASE_ADDRESS + 0x030)
#define CP_FIFO_WRD_H_ADDR  (CP_BASE_ADDRESS + 0x032)
#define CP_FIFO_WRP_L_ADDR  (CP_BASE_ADDRESS + 0x034)
#define CP_FIFO_WRP_H_ADDR  (CP_BASE_ADDRESS + 0x036)
#define CP_FIFO_RDP_L_ADDR  (CP_BASE_ADDRESS + 0x038)
#define CP_FIFO_RDP_H_ADDR  (CP_BASE_ADDRESS + 0x03A)
#define CP_FIFO_BP_L_ADDR   (CP_BASE_ADDRESS + 0x03C)
#define CP_FIFO_BP_H_ADDR   (CP_BASE_ADDRESS + 0x03E)


#define WPAR_BASE_ADDRESS 0xCC008000


class CP{
    AXILiteIF *iface;

    void SetU32(uint32_t addr, uint32_t value);

public:
    CP(AXILiteIF *iface);

    void SetFIFOBase(uint32_t addr);
    void SetFIFOEnd(uint32_t addr);

    void SetFIFOHighWatermak(uint32_t addr);
    void SetFIFOLowWatermark(uint32_t addr);
    void SetFIFOBreakpoint(uint32_t addr);

    void WriteU8(uint8_t v);
    void WriteU16(uint16_t v);
    void WriteU32(uint32_t v);
};