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

#include "CP.h"

void CP::SetU32(uint32_t addr, uint32_t value){
    iface->WriteU32(addr, value & 0xFFFF, 0b11);
    iface->WriteU32(addr+2, value >> 16,  0b11);
}

CP::CP(AXILiteIF *iface){
    this->iface = iface;
}



void CP::SetFIFOBase(uint32_t addr){
    SetU32(CP_FIFO_BASE_L_ADDR, addr);
}

void CP::SetFIFOEnd(uint32_t addr){
    SetU32(CP_FIFO_END_L_ADDR, addr);
}


void CP::SetFIFOHighWatermak(uint32_t addr){
    SetU32(CP_FIFO_HWM_L_ADDR, addr);
}

void CP::SetFIFOLowWatermark(uint32_t addr){
    SetU32(CP_FIFO_LWM_L_ADDR, addr);
}

void CP::SetFIFOBreakpoint(uint32_t addr){
    SetU32(CP_FIFO_BP_L_ADDR, addr);
}


void CP::WriteU8(uint8_t v){
    iface->WriteU32(WPAR_BASE_ADDRESS, v, 0b0001);
}

void CP::WriteU16(uint16_t v){
    iface->WriteU32(WPAR_BASE_ADDRESS, v, 0b0011);
}

void CP::WriteU32(uint32_t v){
    iface->WriteU32(WPAR_BASE_ADDRESS, v, 0b1111);
}