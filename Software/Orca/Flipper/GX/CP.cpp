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

#include "../MemoryMap.h"

using namespace FlipperAPI;

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

CP::CP(){
    cpRegisters = 0;
    wpar = 0;
    mem = 0;
}

CP::CP(ZynqUSP::System *system, ZynqUSP::UIO* flipperUIO){
    mem = system->GetMemory();

    this->cpRegisters = (volatile CPRegisters*)flipperUIO->GetMemory(FLIPPER_CP_OFFSET);
    this->wpar = (volatile void*)flipperUIO->GetMemory(FLIPPER_WPAR_OFFSET);

    this->cpRegisters->CR = 0xFF;
}

CP::~CP(){
}

uint16_t CP::GetStatus(){
    return cpRegisters->SR;
}

uint16_t CP::GetControl(){
    return cpRegisters->CR;
}

bool CP::IsBPInterrupt(uint16_t s){
    return (s & (1<<4)) > 0 ? true : false;
}

bool CP::IsGPCommandsIdle(uint16_t s){
    return (s & (1<<3)) > 0 ? true : false;
}

bool CP::IsGPReadIdle(uint16_t s){
    return (s & (1<<2)) > 0 ? true : false;
}

bool CP::IsFIFOUnderflowInterrupt(uint16_t s){
    return (s & (1<<1)) > 0 ? true : false;
}

bool CP::IsFIFOOverflowInterrupt(uint16_t s){
    return (s & (1<<0)) > 0 ? true : false;
}

bool CP::IsBPEnabled(uint16_t c){
    return (c & (1<<5)) > 0 ? true : false;
}

bool CP::IsGPLinkEnabled(uint16_t c){
    return (c & (1<<4)) > 0 ? true : false;
}

bool CP::IsFIFOOverflowIRQEnabled(uint16_t c){
    return (c & (1<<3)) > 0 ? true : false;
}

bool CP::IsFIFOUnderflowIRQEnabled(uint16_t c){
    return (c & (1<<2)) > 0 ? true : false;
}

bool CP::IsCPIRQEnabled(uint16_t c){
    return (c & (1<<1)) > 0 ? true : false;
}

bool CP::IsGPFIFOReadEnabled(uint16_t c){
    return (c & (1<<0)) > 0 ? true : false;
}


void CP::SetBPEnable(bool enable){
    if(enable)
        cpRegisters->CR |= (1 << 5);
    else
        cpRegisters->CR &= ~(1 << 5);
}

void CP::SetGPLinkEnable(bool enable){
    if(enable)
        cpRegisters->CR |= (1 << 4);
    else
        cpRegisters->CR &= ~(1 << 4);
}

void CP::SetFIFOUnderflowIRQEnable(bool enable){
    if(enable)
        cpRegisters->CR |= (1 << 3);
    else
        cpRegisters->CR &= ~(1 << 3);
}

void CP::SetFIFOOverflowIRQEnable(bool enable){
    if(enable)
        cpRegisters->CR |= (1 << 2);
    else
        cpRegisters->CR &= ~(1 << 2);
}

void CP::SetCPIRQEnable(bool enable){
    if(enable)
        cpRegisters->CR |= (1 << 1);
    else
        cpRegisters->CR &= ~(1 << 1);
}

void CP::SetGPFIFOReadEnable(bool enable){
    if(enable)
        cpRegisters->CR |= (1 << 0);
    else
        cpRegisters->CR &= ~(1 << 0);
}


void CP::ClearFIFOUnderflowIRQ(){
    cpRegisters->CLEAR |= (1<<1);
}

void CP::ClearFIFOOverflowIRQ(){
    cpRegisters->CLEAR |= (1<<0);
}

void CP::ClearAllIntrrupts(){
    cpRegisters->CLEAR |= (1<<1) | (1<<0);
    cpRegisters->CR |= (1 << 1);
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