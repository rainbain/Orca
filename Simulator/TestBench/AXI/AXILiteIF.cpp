// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * AXILiteIF
 * rainbain
 * 7/17/2023
 * Orca Emulator
 *
 * A AXI lite implmentation.
 * This is not full but rather just implments a working interface.
 * Something to work on when the simulator is of higher priority.
*/
#include <stdio.h>

#include "AXILiteIF.h"

void AXILiteIF::StateIdle(){
    // If no request dont do anything.
    if(GetRequestFilled())return;

    // Begin read or write depending on the request.
    if(request.isWrite){
        state = WRITE_WRITE_ADDRESS;
    }else {
        state = WRITE_READ_ADDRESS;
    }
}

void AXILiteIF::StateWriteWriteAddress(){
    // Write our write address when the host is ready
    *dut.awaddr = request.address;
    *dut.wdata = request.data;
    *dut.wstrb = request.byteEnable;

    if(*dut.awvalid && *dut.awready && dut.wvalid && dut.wready){
        state = GET_WRITE_RESPONCE;
    }
    *dut.awvalid = 1;
    *dut.wvalid = 1;
}

void AXILiteIF::StateWriteReadAddress(){
    // Same as the write but with just reading
    *dut.araddr = request.address;

    if(*dut.arvalid && dutob.arready){
        state = GET_READ_RESPONCE;
    }else {
        *dut.arvalid = 1;
    }
}

void AXILiteIF::StateGetReadResponce(){
    if(*dut.rready && dutob.rvalid){
        *dut.rready = 0;
        *dut.arvalid = 0;
        state = IDLE;

        request.data = dutob.rdata;
        request.responce = dutob.rresp;

        FillRequest();
    }else {
        *dut.rready = 1;
    }
}

void AXILiteIF::StateGetWriteResponce(){
    if(*dut.bready && dutob.bvalid){
        *dut.bready = 0;
        *dut.awvalid = 0;
        *dut.wvalid = 0;
        request.responce = dutob.bresp;
        state = IDLE;
        FillRequest();
    }else {
        *dut.bready = 1;
    }
}


void AXILiteIF::Reset(){
    *dut.arvalid = 0;
    *dut.rready = 0;
    *dut.awvalid = 0;
    *dut.bready = 0;
}

void AXILiteIF::Observe(){
    dutob.arready = *dut.arready;

    dutob.rdata = *dut.rdata;
    dutob.rresp = (AXIResponce)*dut.rresp;
    dutob.rvalid = *dut.rvalid;

    dutob.awready = *dut.awready;

    dutob.bresp = (AXIResponce)*dut.bresp;
    dutob.bvalid = *dut.bvalid;
}

void AXILiteIF::Tick(){
    switch(state){
        case IDLE:
            StateIdle();
            break;
        case WRITE_WRITE_ADDRESS:
            StateWriteWriteAddress();
            break;
        case WRITE_READ_ADDRESS:
            StateWriteReadAddress();
            break;
        case GET_READ_RESPONCE:
            StateGetReadResponce();
            break;
        case GET_WRITE_RESPONCE:
            StateGetWriteResponce();
            break;
        default:
            printf("AXI Lite inerface in unknow state.\n");
            state = IDLE;
            break;
    }
}


void AXILiteIF::PutRequest(AXILiteReadRequest request){
    this->request = request;
    std::unique_lock<std::mutex> lock(requestMutex);
    SetRequestFilled(false);
    if (!requestWaiter.wait_for(lock, std::chrono::seconds(1), [this]{ return GetRequestFilled(); })) {
        printf("AXILite request timmed out!\n");
    }
}

void AXILiteIF::FillRequest(){
    {
        std::lock_guard<std::mutex> lock(requestMutex);
        SetRequestFilled(true);
    }
    requestWaiter.notify_one();
}

void AXILiteIF::SetRequestFilled(bool value){
    requestFilledMutex.lock();
    requestFilled = value;
    requestFilledMutex.unlock();
}

bool AXILiteIF::GetRequestFilled(){
    bool value;
    requestFilledMutex.lock();
    value = requestFilled;
    requestFilledMutex.unlock();
    return value;
}



AXILiteIF::AXILiteIF(AXILiteDeviceRefrence dut){
    this->dut = dut;
    this->state = IDLE;
    this->requestFilled = true;
}

void AXILiteIF::OnPosedge(){
    if(*dut.resetn == 0){
        Reset();
    }else {
        Tick();
    }

    Observe();
}

void AXILiteIF::WriteU32(uint32_t address, uint32_t value, uint8_t strobe){
    AXILiteReadRequest req;
    req.isWrite = 1;
    req.address = address;
    req.byteEnable = strobe;
    req.data = value;
    PutRequest(req);
}

uint32_t AXILiteIF::ReadU32(uint32_t address){
    AXILiteReadRequest req;
    req.isWrite = 0;
    req.address = address;
    req.byteEnable = 15;
    PutRequest(req);
    return this->request.data;
}

void AXILiteIF::WriteU16(uint32_t address, uint16_t value){
    uint32_t alliinedAddress = address & (~0b11);

    uint8_t alinement = address % 4;
    uint8_t strobe = AXI16StrobeLookup[alinement];

    uint32_t longValue = value;

    WriteU32(address, alinement == 0 ? longValue : longValue << 16, strobe);
}

uint16_t AXILiteIF::ReadU16(uint32_t address){
    uint32_t alliinedAddress = address & (~0b11);
    uint8_t alinement = address % 4;

    uint32_t longValue = ReadU32(alliinedAddress);

    return alinement == 0 ? longValue : longValue >> 16;
}


void AXILiteIF::WriteU8(uint32_t address, uint8_t value){
    uint32_t alliinedAddress = address & (~0b11);

    uint8_t alinement = address % 4;
    uint8_t strobe = 0b1 << alinement;

    uint8_t shiftedValue = value << (alinement * 8);

    WriteU32(address, shiftedValue, strobe);
}

uint8_t AXILiteIF::ReadU8(uint32_t address){
    uint32_t alliinedAddress = address & (~0b11);
    uint8_t alinement = address % 4;

    uint32_t longValue = ReadU32(alliinedAddress);
    
    return longValue >> (alinement * 8);
}