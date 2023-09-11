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

#include <stdio.h>
#include <stdlib.h>

using namespace Flipper;

void CP::SetU32(volatile CPWord *word, uint32_t value){
    word->H = value >> 16;
    word->L = value & 0xFFFF;
}

uint32_t CP::GetU32(volatile CPWord *word){
    uint32_t w = word->H;
    w <<= 16;
    w |= word->L;
    return w;
}


CP::CP(ZynqUSP::Memory *iface){
    this->mem = iface;

    cpMemorySpace = mem->Map(CP_BASE_ADDRESS, 0xFFFF);

    this->cpRegisters = (volatile CPRegisters*)cpMemorySpace;
    this->wpar = (void*)((size_t)cpMemorySpace + 0x8000);
}

CP::~CP(){
    mem->UnMap((void*)cpMemorySpace, 0xFFFF);
}


void CP::SetFIFOAXIBase(uint32_t addr){
    SetU32(&cpRegisters->FIFO_AXIBase, addr);
}

void CP::SetFIFOBase(uint32_t addr){
    SetU32(&cpRegisters->FIFO_BASE, addr);
}

void CP::SetFIFOEnd(uint32_t addr){
    SetU32(&cpRegisters->FIFO_END, addr);
}


void CP::SetFIFOHighWatermak(uint32_t addr){
    SetU32(&cpRegisters->FIFO_HWM, addr);
}

void CP::SetFIFOLowWatermark(uint32_t addr){
    SetU32(&cpRegisters->FIFO_LWM, addr);
}

void CP::SetFIFOBreakpoint(uint32_t addr){
    SetU32(&cpRegisters->FIFO_BP, addr);
}



uint32_t CP::GetFIFOBase(){
    return GetU32(&cpRegisters->FIFO_BASE);
}

uint32_t CP::GetFIFOEnd(){
    return GetU32(&cpRegisters->FIFO_END);
}


uint32_t CP::GetFIFOAXIBase(){
    return GetU32(&cpRegisters->FIFO_AXIBase);
}

uint32_t CP::GetFIFOHighWatermark(){
    return GetU32(&cpRegisters->FIFO_HWM);
}

uint32_t CP::GetFIFOLowWatermark(){
    return GetU32(&cpRegisters->FIFO_LWM);
}

uint32_t CP::GetFIFOBreakpoint(){
    return GetU32(&cpRegisters->FIFO_BP);
}


uint32_t CP::GetFIFOWritePointer(){
    return GetU32(&cpRegisters->FIFO_WRP);
}

uint32_t CP::GetFIFOReadPointer(){
    return GetU32(&cpRegisters->FIFO_RDP);
}

uint32_t CP::GetIFFOReadWriteDistance(){
    return GetU32(&cpRegisters->FIFO_WRD);
}


uint16_t CP::GetFIFOErrors(){
    return cpRegisters->FIFO_Errors;
}



void CP::WriteU8(uint8_t v){
    volatile uint8_t *gp = (volatile uint8_t*)wpar;
    *gp = v;
}

void CP::WriteU16(uint16_t v){
    volatile uint16_t *gp = (volatile uint16_t*)wpar;
    *gp = v;
}

void CP::WriteU32(uint32_t v){
    volatile uint32_t *gp = (volatile uint32_t*)wpar;
    *gp = v;
}



GXFIFO CP::CreateGXFIFO(){
    GXFIFO fifo;

    fifo.size = mem->GetPageSize();

    // Alined per FIFO size as the FIFO has 32 bit addressing!
    fifo.buffer = aligned_alloc(fifo.size, fifo.size);
    
    mem->LockPage(fifo.buffer, fifo.size);
    fifo.physicalAddress = mem->GetPhysicalAddress(fifo.buffer);

    return fifo;
}

void CP::DestroyFIFO(GXFIFO fifo){
    mem->UnlockPage(fifo.buffer, fifo.size);
    free(fifo.buffer);
}


void CP::PrepareFIFORead(GXFIFO fifo){
    mem->InvalidateCache(fifo.buffer, fifo.size);
}

void CP::LoadFIFO(GXFIFO fifo){
    SetFIFOAXIBase(fifo.physicalAddress >> 32);
    SetFIFOBase(fifo.physicalAddress & 0xFFFFFFFF);
    SetFIFOEnd(fifo.physicalAddress + fifo.size - 32);
}