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

#include "TestBench.h"

#include <chrono>

void TestBench::ServerThread(){
    Reset();
    while(GetRunning()){
        auto tickStart = std::chrono::steady_clock::now();

        for(uint32_t i = 0; i < frequency; i++)
            Clock();

        auto tickEnd = std::chrono::steady_clock::now();

        auto tickTimeMs = std::chrono::duration_cast<std::chrono::microseconds>(tickEnd - tickStart).count();
        int32_t sleepTime = 1000000 - tickTimeMs;
        if(sleepTime > 0)
            std::this_thread::sleep_for(std::chrono::microseconds(sleepTime));
        else
            printf("Server fallen behind by %d microseconds.\n", sleepTime);
    }
}



TestBench::TestBench(uint32_t frequency){
    this->running = false;
    this->frequency = frequency;
    this->useTracing = false;
    this->dut = new VFlipperTop;
    this->trace = new VerilatedVcdC;
    traceTime = 0;


    AXILiteDeviceRefrence liteInterface {
        &dut->clk, &dut->resetn,

        &dut->araddr_b, &dut->arvalid_b, &dut->arready_b,
        &dut->rdata_b, &dut->rresp_b, &dut->rvalid_b, &dut->rready_b,
        &dut->awaddr_b, &dut->awvalid_b, &dut->awready_b,
        &dut->wdata_b, &dut->wstrb_b, &dut->wvalid_b, &dut->wready_b,
        &dut->bresp_b, &dut->bvalid_b, &dut->bready_b

    };

    AXIHostRefrence hostInterface {
        &dut->clk, &dut->resetn,

        &dut->awaddr_a, &dut->awlen_a, &dut->awsize_a,
        &dut->awburst_a, &dut->awvalid_a, &dut->awready_a,
        dut->wdata_a, &dut->wstrb_a, &dut->wlast_a, &dut->wvalid_a, &dut->wready_a,
        &dut->bresp_a, &dut->bvalid_a, &dut->bready_a,
        &dut->araddr_a, &dut->arlen_a, &dut->arsize_a, &dut->arburst_a, &dut->arvalid_a, &dut->arready_a,
        dut->rdata_a, &dut->rresp_a, &dut->rlast_a,
        &dut->rvalid_a, &dut->rready_a
    };

    this->axiLiteIf = new AXILiteIF(liteInterface);
    this->axiHostIf = new AXIHostIF(hostInterface);
}

void TestBench::Clock(){
    dut->clk = 0;
    dut->eval();
    
    if(useTracing)trace->dump(traceTime); traceTime++;

    dut->clk = 1;
    dut->eval();
    OnPosedge();
    dut->eval();

    if(useTracing)trace->dump(traceTime); traceTime++;
}

void TestBench::Reset(){
    dut->resetn = 0;
    Clock();
    dut->resetn = 1;
    Clock();
}

void TestBench::OnPosedge(){
    axiLiteIf->OnPosedge();
    axiHostIf->OnPosedge();
}



TestBench::~TestBench(){
    if(useTracing)trace->close();
    dut->final();
    delete dut;
}


void TestBench::Open(){
    running = true;

    serverThread = new std::thread(&TestBench::ServerThread, this);
}

void TestBench::Close(){
    SetRunning(false);
    serverThread->join();

    delete serverThread;
}


void TestBench::TraceOpen(std::string file){
    dut->trace(trace, 5);
    trace->open(file.c_str());
    useTracing = true;
}

void TestBench::SetupAXICallbacks(AXIWrite axiHostWrite){
    axiHostIf->SetWriteCallback(axiHostWrite);
}



bool TestBench::GetRunning(){
    bool val;
    sharedLock.lock();
    val = running;
    sharedLock.unlock();
    return val;
}

void TestBench::SetRunning(bool running){
    sharedLock.lock();
    this->running = running;
    sharedLock.unlock();
}

AXILiteIF* TestBench::GetCPUInterface(){
    AXILiteIF* i;
    sharedLock.lock();
    i = axiLiteIf;
    sharedLock.unlock();
    return i;
}