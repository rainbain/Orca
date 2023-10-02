// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * AXIHostIF
 * rainbain
 * 9/1/2023
 * Orca Emulator
 *
 * AIX Host Interface
*/

#include <stdio.h>

#include "AXIHostIF.h"

#define AXI_ALINEMENT ((1<<12) - 1) // 4 KB

uint32_t AXIHostIF::addAlinedAddress(uint32_t addr, uint8_t size){
    uint16_t section = addr & AXI_ALINEMENT;
    section += size;

    return (addr & (~AXI_ALINEMENT)) | section;
}


void AXIHostIF::WriteStateIdle(){
    if(dutob.awvalid && *dut.awready){
        *dut.awready = 0;

        writeBytesPerWord = AXISizeLookup[dutob.awsize & 0b111];
        writeTransferLength = dutob.awlen + 1;
        writeBurst = (AXIBurst)dutob.awburst;
        writePointer = dutob.awaddr;

        writeState = WS_DATA;
    }else {
        *dut.awready = 1;
    }
}

void AXIHostIF::WriteStateDate(){
    if(dutob.wvalid && *dut.wready){
        *dut.wready = ~dutob.wlast;
        callbackMutex.lock();
        axiWrite(writePointer+0, dutob.wdata[0], (dutob.wstrb >> 12));
        axiWrite(writePointer+4, dutob.wdata[1], (dutob.wstrb >> 8) & 0b1111);
        axiWrite(writePointer+8, dutob.wdata[2], (dutob.wstrb >> 4) & 0b1111);
        axiWrite(writePointer+12, dutob.wdata[3], (dutob.wstrb)      & 0b1111);
        callbackMutex.unlock();
        if(writeTransferLength == 0){
            printf("AXI write transfer length exceeds the given length!\n");

        }else {
            writeTransferLength--;
        }

        if(dutob.wlast){
            *dut.wready = 0;
            writeState = WS_BACK_RESPONCE;
        }

        writePointer = addAlinedAddress(writePointer, writeBytesPerWord);
        
    }else {
        *dut.wready = 1;
    }
}

void AXIHostIF::WriteStateBackResponce(){
    if(dutob.bready && *dut.bvalid){
        *dut.bvalid = 0;
        *dut.bresp = 0;

        writeState = WS_IDLE;
    }else {
        *dut.bvalid = dutob.bready;
    }
}

void AXIHostIF::WriteTick(){
    switch(writeState){
        case WS_IDLE:
            WriteStateIdle();
            break;
        case WS_DATA:
            WriteStateDate();
            break;
        case WS_BACK_RESPONCE:
            WriteStateBackResponce();
            break;
    }
}


void AXIHostIF::Reset(){
    *dut.awready = 0;
    *dut.wready = 0;
    *dut.bvalid = 0;
    *dut.arready = 0;
    *dut.rlast = 0;
    *dut.rvalid = 0;

    writeState = WS_IDLE;
}

void AXIHostIF::Observe(){
    dutob.awaddr = *dut.awaddr;
    dutob.awlen = *dut.awlen;
    dutob.awsize = *dut.awsize;
    dutob.awburst = *dut.awburst;
    dutob.awvalid = *dut.awvalid;

    dutob.wdata[0] = dut.wdata[0];
    dutob.wdata[1] = dut.wdata[1];
    dutob.wdata[2] = dut.wdata[2];
    dutob.wdata[3] = dut.wdata[3];
    dutob.wstrb = *dut.wstrb;
    dutob.wlast = *dut.wlast;
    dutob.wvalid = *dut.wvalid;

    dutob.bready = *dut.bready;

    dutob.araddr = *dut.araddr;
    dutob.arlen = *dut.arlen;
    dutob.arsize = *dut.arsize;
    dutob.arburst = *dut.arburst;
    dutob.arvalid = *dut.arvalid;

    dutob.rready = *dut.rready;
}

void AXIHostIF::Tick(){
    WriteTick();
}



AXIHostIF::AXIHostIF(AXIHostRefrence dut){
    this->dut = dut;
    Reset();
}


void AXIHostIF::OnPosedge(){
    if(*dut.resetn == 0){
        Reset();
    }else {
        Tick();
    }

    Observe();
}


void AXIHostIF::SetWriteCallback(AXIWrite axiWrite){
    callbackMutex.lock();
    this->axiWrite = axiWrite;
    callbackMutex.unlock();
}