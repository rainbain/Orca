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

#pragma once

#include <verilated.h>
#include "VFlipperTop.h"

#include "AXIRefrence.h"

#include <stdint.h>
#include <queue>
#include <functional>

enum class AXIBurst {
    FIXED = 0,
    INCRAMENT,
    WRAP
};

struct AXIReadRequest{
    uint32_t address;
    AXIBurst burst;
    uint8_t len;
    uint8_t size;

    uint64_t servedTick;
};

struct AXIReadIF_Observed {
    // Read Address Channel
    uint32_t araddrm;
    AXIBurst arburstm;
    uint8_t arlenm;
    uint8_t arsizem;
    uint8_t arvalidm;

    // Write Data Channel
    uint8_t rreadym;
};

class AXIReadIF {
public:
    using Callback = std::function<uint8_t(uint32_t)>;
private:
    AXIReadIFRefrence dut;
    uint64_t tick;
    uint8_t queSize;
    uint8_t requestLatency;
    uint8_t busWidth;

    AXIReadIF_Observed dutob;
    std::queue<AXIReadRequest> requestQueue;
    AXIReadRequest *outgoingRequest;

    Callback memoryReadCallback;

    // Utility
    uint8_t Read(uint32_t addr, uint8_t size);

    // Divided Edge Function
    void Reset();
    void Observe();
    void ManageIncoming();
    void ManageOutgoing();
    void UpdateOutputs();
public:
    AXIReadIF(AXIReadIFRefrence dut, uint8_t busWidth, uint8_t requestLatency, uint8_t queSize);

    void OnPosedge();
    
    void SetCallback(Callback cb);
};