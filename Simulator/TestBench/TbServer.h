// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * TbServer
 * rainbain
 * 7/9/2023
 * Orca Emulator
 *
 * A multithreaded test bench simular to server client model.
*/

#pragma once

#include <mutex>
#include <thread>

#include "TestBench.h"
#include "TbCommandInterface.h"

class TbServer {
    /*
     * Server Side
    */
    TestBench *tb;
    bool serverRunning;
    uint32_t ticksPerSecond;
    
    std::thread* serverThread;
    std::mutex serverMutex;

    void ServerFunction();
public:
    TbCommandInterface* controlInterface;

    TbServer();
    ~TbServer();

    /*
     * Server Control
    */

    void Open(uint32_t tickSpeed, std::string trace = "");
    void Close();

    void SetRunning(bool value);
    bool GetRunning();
};