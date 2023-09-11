// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * AXILiteIF
 * rainbain
 * 7/16/2023
 * Orca Emulator
 *
 * A AXI lite implmentation.
 * This is not full but rather just implments a working interface.
 * Something to work on when the simulator is of higher priority.
*/

#pragma once

#include "AXIRefrence.h"

#include <mutex>
#include <condition_variable>

struct AXILiteIF_Observed {
    uint8_t arready;

    uint32_t rdata;
    AXIResponce rresp;
    uint8_t rvalid;

    uint8_t awready;

    AXIResponce bresp;
    uint8_t bvalid;
};

struct AXILiteReadRequest {
    uint8_t isWrite;
    uint32_t address;
    uint32_t data;
    uint8_t byteEnable;

    AXIResponce responce;
};

class AXILiteIF {
private:
    enum State {
        IDLE,
        WRITE_WRITE_ADDRESS,
        WRITE_READ_ADDRESS,
        GET_READ_RESPONCE,
        GET_WRITE_RESPONCE
    };

    // Request
    std::condition_variable requestWaiter;
    std::mutex requestMutex;
    bool requestFilled;
    std::mutex requestFilledMutex;
    AXILiteReadRequest request;

    // Logic System
    AXILiteDeviceRefrence dut;
    AXILiteIF_Observed dutob;
    State state;

    // States
    void StateIdle();
    void StateWriteWriteAddress();
    void StateWriteReadAddress();
    void StateGetReadResponce();
    void StateGetWriteResponce();

    // Divided Edge Functions
    void Reset();
    void Observe();
    void Tick();

    // Request System
    void PutRequest(AXILiteReadRequest request);
    void FillRequest();

    void SetRequestFilled(bool value);
    bool GetRequestFilled();
public:
    AXILiteIF(AXILiteDeviceRefrence dut);

    void OnPosedge();

    void WriteU32(uint32_t address, uint32_t value, uint8_t strobe = 15);
    uint32_t ReadU32(uint32_t address);
};