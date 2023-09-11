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

#pragma once

#include "AXIRefrence.h"

#include <stdint.h>
#include <queue>
#include <functional>
#include <mutex>

struct AXIHost_Observed {
    uint32_t awaddr;
    uint8_t awlen;
    uint8_t awsize;
    uint8_t awburst;
    uint8_t awvalid;

    uint32_t wdata[4];
    uint16_t wstrb;
    uint8_t wlast;
    uint8_t wvalid;

    uint8_t bready;

    uint32_t araddr;
    uint8_t arlen;
    uint8_t arsize;
    uint8_t arburst;
    uint8_t arvalid;

    uint8_t rready;
};

class AXIHostIF {
private:
    enum WriteState {
        WS_IDLE,
        WS_DATA,
        WS_BACK_RESPONCE
    };

    // Utility
    uint32_t addAlinedAddress(uint32_t addr, uint8_t size);

    // Logic
    AXIHostRefrence dut;
    AXIHost_Observed dutob;

    // Write Settings
    uint8_t writeBytesPerWord;
    uint8_t writeTransferLength;
    AXIBurst writeBurst;
    uint32_t writePointer;

    AXIWrite axiWrite;

    // Threads
    std::mutex callbackMutex;

    // Write Logic
    WriteState writeState;
    void WriteStateIdle();
    void WriteStateDate();
    void WriteStateBackResponce();

    void WriteTick();

    // Edge Functions
    void Reset();
    void Observe();
    void Tick();
public:
    AXIHostIF(AXIHostRefrence dut);

    void OnPosedge();

    void SetWriteCallback(AXIWrite axiWrite);
};