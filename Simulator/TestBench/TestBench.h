// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * TestBench
 * rainbain
 * 7/15/2023
 * Orca Emulator
 *
 * A testbench server that holds on to the main test bench components.
*/

#pragma once

#include <verilated.h>
#include <verilated_vcd_c.h>
#include "VFlipperTop.h"

#include "AXI/AXIReadIF.h"
#include "AXI/AXILiteIF.h"

#include <mutex>
#include <thread>

class TestBench {
    /*
     * Server Side
    */
   bool useTracing;
   uint32_t frequency;
   VFlipperTop *dut;
   VerilatedVcdC *trace;
   uint64_t traceTime;

   AXIReadIF *axiReadIf;
   AXILiteIF *axiLiteIf;

   void ServerThread();
   void Clock();
   void Reset();
   void OnPosedge();

   /*
    * Shared
   */
   std::mutex sharedLock; 
   bool running;

   /*
    * Clinet
   */
   
   std::thread* serverThread;

public:
    TestBench(uint32_t frequency);
    ~TestBench();

    void Open();
    void Close();

    /*
     * Setup, Must be called before opening the server.
    */

    void TraceOpen(std::string file);
    void SetupAXICallbacks(AXIReadIF::Callback axiReadCb);

    /*
     * Setters And Getters
    */
   
   bool GetRunning();
   void SetRunning(bool running);

   AXILiteIF* GetCPUInterface(); 
};