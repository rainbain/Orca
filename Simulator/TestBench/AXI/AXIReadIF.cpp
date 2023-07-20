// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * AXIReadIF
 * rainbain
 * 7/15/2023
 * Orca Emulator
 *
 * Simulated AXI Read Interface
*/
#include <stdio.h>
#include <cstring>

#include "AXIReadIF.h"

// Size lookup table. It may just be n^s but its faster.
uint8_t AXISizeLookup[] = {
    1, 2, 4, 8, 16, 32, 64, 128
};

uint8_t AXIReadIF::Read(uint32_t addr, uint8_t size){
    uint8_t* arr = (uint8_t*)dut.rdatam;

    // No Handler, just send 0s
    if(!memoryReadCallback){
        printf("AXI Read callback not set, sending zeros.\n");
        memset(arr, 0, busWidth);
        return 0;
    }

    uint8_t bytes = AXISizeLookup[size & 0b111];
    if(bytes > busWidth){
        printf("Axi Read capped at %d bytes.\n", busWidth);
        bytes = busWidth;
    }

    for(uint8_t i = 0; i < bytes; i++){
        arr[i] = memoryReadCallback(addr+i);
    }

    //Pad the rest with zeros
    if(bytes < busWidth)memset(arr+bytes, 0, busWidth-bytes);
    return bytes;
}


void AXIReadIF::Reset(){
    // Hate that clear method
    while(!requestQueue.empty()) requestQueue.pop();
    this->outgoingRequest = 0;

    // Clear outputs
    *dut.arreadym = 0;
    *dut.rlastm = 0;
    *dut.rvalidm = 0;
}

void AXIReadIF::Observe(){
    dutob.araddrm = *dut.araddrm;
    dutob.arburstm = (AXIBurst)*dut.arburstm;
    dutob.arlenm = *dut.arlenm;
    dutob.arsizem = *dut.arsizem;
    dutob.arvalidm = *dut.arvalidm;
    dutob.rreadym = *dut.rreadym;
}


void AXIReadIF::ManageIncoming(){
    // Write Handshake
    if(dutob.arvalidm & *dut.arreadym){
        if(requestQueue.size() >= this->queSize){
            printf("AXI Read Address FIFO full but we still accepted a request. Thats a bug.\n");
        }else {
            // Push to Queue
            AXIReadRequest request;
            request.address = dutob.araddrm;
            request.burst = dutob.arburstm;
            request.len = dutob.arlenm;
            request.size = dutob.arsizem; // N-1 size.
            request.servedTick = tick + requestLatency;

            requestQueue.push(request);

            // Check if format is valid
            switch(request.burst){
                case AXIBurst::FIXED:
                    if(request.len != 0)printf("AXI Read Address got burst type fixed with a non-zero length.\n");
                    break;
                case AXIBurst::INCRAMENT:
                    break;
                case AXIBurst::WRAP:
                    printf("AXI Read Wrap Burst Unsuppored.\n");
                default:
                    printf("Reserved AXI burst type called.\n");
            }
        }
    }
}

void AXIReadIF::ManageOutgoing(){
    // Clear for now, will get set lator if a memory tansfer does happen
    *dut.rvalidm = 0;
    *dut.rlastm = 0;
    *dut.rrespm = 0; //Always 0.

    //If not ready dont do anything
    if(*dut.rreadym == 0)return;

    // Attempt to dispatch a request if needed
    if(outgoingRequest == 0 && !requestQueue.empty()){
        outgoingRequest = &requestQueue.front();
    }

    // Exit if we dont have a contender. Or if our continer has latency
    if(outgoingRequest == 0) return;
    if(outgoingRequest->servedTick > tick) return;

    // Write Data
    outgoingRequest->address += Read(outgoingRequest->address, outgoingRequest->size);
    *dut.rvalidm = 1;

    // Clear request if its done
    if(outgoingRequest->burst == AXIBurst::FIXED || outgoingRequest->len == 0){
        *dut.rlastm = 1;
        outgoingRequest = 0;
        requestQueue.pop();
    }else {
        *dut.rlastm = 0;
        outgoingRequest->len--;
    }
}

void AXIReadIF::UpdateOutputs(){
    if(requestQueue.size() >= this->queSize){
        *dut.arreadym = 0;
    } else {
        *dut.arreadym = 1;
    }
}


AXIReadIF::AXIReadIF(AXIHostReadIFRefrence dut, uint8_t busWidth, uint8_t requestLatency, uint8_t queSize){
    this->dut = dut;
    this->busWidth = busWidth;
    this->requestLatency = requestLatency;
    this->queSize = queSize;
    this->tick = 0;
    this->outgoingRequest = 0;
}

void AXIReadIF::OnPosedge(){
    tick++;
    if(*dut.resetn == 0){
        Reset();
    }else {
        ManageIncoming();
        ManageOutgoing();
    }

    UpdateOutputs();

    Observe();
}

void AXIReadIF::SetCallback(Callback cb){
    this->memoryReadCallback = cb;
}