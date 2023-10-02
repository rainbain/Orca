/*
 * CP
 * rainbain
 * 9/2/2023
 * Orca Emulator
 *
 * Simulator CP low level API acesss.
 * Simulator Version
*/

#include "CP.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

using namespace FlipperAPI;

void CP::SetU32(CPWord *word, uint32_t value){
    iface->WriteU32(AXI_CAST(word), value);
}

uint32_t CP::GetU32(CPWord *word){
    return iface->ReadU32(AXI_CAST(word));
}


CP::CP(AXILiteIF *iface) {
    this->iface = iface;

    this->cpRegisters = (CPRegisters*)FLIPPER_CP_OFFSET; // We use this just to generate offsets
    this->wpar = (void*)FLIPPER_WPAR_OFFSET;

    iface->WriteU16(AXI_CAST(&cpRegisters->CR), 0);
}

CP::~CP(){

}

uint16_t CP::GetStatus() {
    return iface->ReadU16(AXI_CAST(&cpRegisters->SR));
}

uint16_t CP::GetControl(){
    return iface->ReadU32(AXI_CAST(&cpRegisters->CR));
}

bool CP::IsBPInterrupt(uint16_t s){
    return (s & (1<<4)) > 0 ? true : false;
}

bool CP::IsGPCommandsIdle(uint16_t s) {
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
    uint16_t bit = iface->ReadU16(AXI_CAST(&cpRegisters->CR));

    if(enable)
        bit |= (1 << 5);
    else
        bit &= ~(1 << 5);

    iface->WriteU16(AXI_CAST(&cpRegisters->CR), bit);
}

void CP::SetGPLinkEnable(bool enable){
    uint16_t bit = iface->ReadU16(AXI_CAST(&cpRegisters->CR));

    if(enable)
        bit |= (1 << 4);
    else
        bit &= ~(1 << 4);

    iface->WriteU16(AXI_CAST(&cpRegisters->CR), bit);
}

void CP::SetFIFOUnderflowIRQEnable(bool enable){
    uint16_t bit = iface->ReadU16(AXI_CAST(&cpRegisters->CR));

    if(enable)
        bit |= (1 << 3);
    else
        bit &= ~(1 << 3);

    iface->WriteU16(AXI_CAST(&cpRegisters->CR), bit);
}

void CP::SetFIFOOverflowIRQEnable(bool enable){
    uint16_t bit = iface->ReadU16(AXI_CAST(&cpRegisters->CR));

    if(enable)
        bit |= (1 << 2);
    else
        bit &= ~(1 << 2);

    iface->WriteU16(AXI_CAST(&cpRegisters->CR), bit);
}

void CP::SetCPIRQEnable(bool enable){
    uint16_t bit = iface->ReadU16(AXI_CAST(&cpRegisters->CR));

    if(enable)
        bit |= (1 << 1);
    else
        bit &= ~(1 << 1);

    iface->WriteU16(AXI_CAST(&cpRegisters->CR), bit);
}

void CP::SetGPFIFOReadEnable(bool enable){
    uint16_t bit = iface->ReadU16(AXI_CAST(&cpRegisters->CR));

    if(enable)
        bit |= (1 << 0);
    else
        bit &= ~(1 << 0);

    iface->WriteU16(AXI_CAST(&cpRegisters->CR), bit);
}


void CP::ClearFIFOUnderflowIRQ(){
    uint16_t bit = iface->ReadU16(AXI_CAST(&cpRegisters->CLEAR));

    bit |= (1 << 1);

    iface->WriteU16(AXI_CAST(&cpRegisters->CLEAR), bit);
}

void CP::ClearFIFOOverflowIRQ() {
    uint16_t bit = iface->ReadU16(AXI_CAST(&cpRegisters->CLEAR));

    bit |= (1 << 0);

    iface->WriteU16(AXI_CAST(&cpRegisters->CLEAR), bit);
}

void CP::ClearAllIntrrupts() {
    uint16_t bit = iface->ReadU16(AXI_CAST(&cpRegisters->CLEAR));

    bit |= (1 << 1) || (1<<0);

    iface->WriteU16(AXI_CAST(&cpRegisters->CLEAR), bit);

    bit = iface->ReadU16(AXI_CAST(&cpRegisters->CR));

    bit |= (1 << 1);

    iface->WriteU16(AXI_CAST(&cpRegisters->CR), bit);
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

uint16_t CP::GetFIFOErrors() {
    return iface->ReadU16(AXI_CAST(&cpRegisters->FIFO_Errors));
}




void CP::WriteU8(uint8_t v){
    return iface->WriteU8(AXI_CAST(wpar), v);
}

void CP::WriteU16(uint16_t v){
    return iface->WriteU16(AXI_CAST(wpar), v);
}

void CP::WriteU32(uint32_t v){
    return iface->WriteU32(AXI_CAST(wpar), v);
}



GXFIFO CP::CreateGXFIFO() {
    GXFIFO fifo;

    fifo.size = getpagesize();

    // Alined per FIFO size as the FIFO has 32 bit addressing!
    fifo.buffer = aligned_alloc(fifo.size, fifo.size);

    fifo.physicalAddress = (uint64_t)fifo.buffer; // Everything stays in virtual memory in emulator

    return fifo;
}

void CP::DestroyFIFO(GXFIFO fifo) {
    free(fifo.buffer);
}


void CP::PrepareFIFORead(GXFIFO fifo){
    // We done need to do anything here
}

void CP::LoadFIFO(GXFIFO fifo) {
    SetFIFOAXIBase(fifo.physicalAddress >> 32);
    SetFIFOBase(fifo.physicalAddress & 0xFFFFFFFF);
    SetFIFOEnd(fifo.physicalAddress + fifo.size - 32);
}