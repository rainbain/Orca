// Copyright 2023 Orca Hardware Emulator
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * TrafficGenerator
 * rainbain
 * 9/9/2023
 * Orca Emulator
 *
 * Wrapper class for generating AXI Traffic
*/

#include "TrafficGenerator.h"

#include <stdio.h>
#include <chrono>
#include <thread>

using namespace ZynqUSP;

TrafficGenerator::TrafficGenerator(Memory *mem, uint64_t addr){
    this->mem = mem;
    this->generator = (AXITrafficGenerator*)this->mem->Map(addr, sizeof(AXITrafficGenerator));

    // Default Config Load
    this->generator->masterControl = 0;
    this->generator->slaveControl = 0;
    this->generator->errorEnable = 
        (0 << 31) | // Transfer Complete interrupt enable
        (0 << 20) | // Read ID error
        (0 << 19) | // Write ID error
        (1 << 18) | // Write Responce Error
        (1 << 17) | // Read Responce Error
        (1 << 16) | // Read Length Error
        (1 << 1) | // Write Strobe Error
        (1 << 0) ; // Write Length Error
    this->generator->masterInterruptEnable = 0; // No Interrupts

    Clear();
}

const uint16_t axiWidthLookup[] = {32, 64, 128, 256, 512, 0, 0, 0};

void TrafficGenerator::DisplayStatus(){
    printf("AXI Traffic Generator:\n  Settings:\n");
    uint8_t revision = this->generator->masterControl >> 24;
    uint32_t config = this->generator->configStatus;
    printf("    Revision:     0x%02x\n", revision);
    printf("    Master Width: %d\n", axiWidthLookup[(config >> 28) | 0b111]);
    printf("    Slave Width:  %d\n", axiWidthLookup[(config >> 25) | 0b111]);
    if((config & (1<<24)) > 0)
        printf("    ATG Mode:     Advanced\n");
    if((config & (1<<23)) > 0)
        printf("    ATG Mode:     Basic\n");
    
    printf("  Errors & Warnings:\n");
    if(revision != 0x20){
        printf("    Warning, expected traffic generation revision 0x20!\n");
    }

    uint32_t errorStatus = generator->errorStatus;
    if((errorStatus & (1<<20)) > 0) printf("    Master Read ID Error!\n");
    if((errorStatus & (1<<19)) > 0) printf("    Master Write ID Error!\n");
    if((errorStatus & (1<<18)) > 0) printf("    Master Write Responce Error!\n");
    if((errorStatus & (1<<17)) > 0) printf("    Master Read Responce Error!\n");
    if((errorStatus & (1<<16)) > 0) printf("    Master Read Length Error!\n");
    if((errorStatus & (1<<1)) > 0) printf("    Slave Write Strobe Error!\n");
    if((errorStatus & (1<<0)) > 0) printf("    Slave Write Length Error!\n");
    printf("  End Of AXI Traffic Generator Status!\n");
}


TrafficGenerator::~TrafficGenerator(){
    this->mem->UnMap(generator, sizeof(AXITrafficGenerator));
}


void TrafficGenerator::Clear(){
    readParamsSize = 0;
    writeParamsSize = 0;
    readCommandsSize = 0;
    writeCommandsSize = 0;
    readAddressesSize = 0;
    writeAddressesSize = 0;
    masterRAMIndex = 0;
}


void TrafficGenerator::PutReadParameter(uint32_t param){
    if(readParamsSize >= 255){
        printf("Can not fit another read parameter!\n");
        return;
    }

    generator->readParameters[readParamsSize] = param;
    readParamsSize++;
}

void TrafficGenerator::PutWriteParameter(uint32_t param){
    if(writeParamsSize >= 255){
        printf("Can not fit another write parameter!\n");
        return;
    }

    generator->writeParameters[writeParamsSize] = param;
    writeParamsSize++;
}

void TrafficGenerator::PutReadCommand(AXITrafficGeneratorCMD cmd){
    if(readCommandsSize >= 255){
        printf("Can not fit another read command!\n");
        return;
    }

    generator->readCommands[readCommandsSize] = cmd;
    readCommandsSize++;
}

void TrafficGenerator::PutWriteCommand(AXITrafficGeneratorCMD cmd){
    if(writeCommandsSize >= 255){
        printf("Can not fit another write command!\n");
        return;
    }

    generator->writeCommands[writeCommandsSize] = cmd;
    writeCommandsSize++;
}


void TrafficGenerator::PutReadAddress(uint32_t addr){
    if(readAddressesSize >= 255){
        printf("Can not fit another read address!\n");
        return;
    }

    generator->readAddresses[readAddressesSize] = addr;
    readAddressesSize++;
}

void TrafficGenerator::PutWriteAddress(uint32_t addr){
    if(writeAddressesSize >= 255){
        printf("Can not fit another write address!\n");
        return;
    }

    generator->writeAddresses[writeAddressesSize] = addr;
    writeAddressesSize++;
}


uint32_t TrafficGenerator::ReadU32(uint64_t addr){
    return 0;
}

void TrafficGenerator::WriteU32(uint64_t addr, uint32_t value){
    if(masterRAMIndex >= 1024*2){
        printf("Can not fit another write address!\n");
        return;
    }

    generator->masterRam[masterRAMIndex] = value;
    masterRAMIndex++;
}



void TrafficGenerator::PutSingleWriteCommand(uint64_t addr, uint32_t value){
    AXITrafficGeneratorCMD cmd;
    cmd.address = addr & 0xFFFFFFFF;
    cmd.settings1 = 
        (1<<31) |          // Valid Command
        (0b000 << 28) |    // All Bytes Valid
        (0b000 << 21) |    // a*_prot[2:0]
        (0b000000 << 15) | // a*_id[5:0]
        (0b010 << 12) |    // a*_size[2:0]
        (0b01 << 10) |     // a*_burst[1:0]
        (0 << 8) |         // a*_lock
        (0 << 0) ;          // a*_len[7:0]
    
    cmd.settings2 = 
        (0 << 22) |             // May Depend
        (0 << 13) |             // Other Depend
        ((masterRAMIndex * 4) << 12) ; // Master RAM index
    
    cmd.settings3 = 
        (0b0000 << 16) |    // qos[3:0]
        (0b00000000 << 8) | // user[7:0]
        (0b0000 << 4) |     // cache[3:0]
        (0b000) ;           // Expected Responce
    WriteU32(addr, value);
    PutWriteCommand(cmd);
    PutWriteAddress(addr >> 32);
    PutWriteParameter(0);
}


void TrafficGenerator::Flush(){
    // Exit Command
    AXITrafficGeneratorCMD cmd;
    cmd.address = 0;
    cmd.settings1 = 0;
    cmd.settings2 = 0;
    cmd.settings3 = 0;
    PutWriteCommand(cmd);

    generator->masterControl |= (1<<20); // Start
    auto start_time = std::chrono::steady_clock::now();
    auto max_wait_time = std::chrono::seconds(1);

    while ((generator->masterControl & 0x20) > 0) {
        auto current_time = std::chrono::steady_clock::now();
        if (current_time - start_time > max_wait_time) {
            printf("AXI Traffic Generator Flush Timmed Out!\n");
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // Clear all buffers
    Clear();
}