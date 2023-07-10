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

#include "TbServer.h"

#include <chrono>

void TbServer::ServerFunction(){
    uint32_t tickDuration = 1000000 / ticksPerSecond;

    while(GetRunning()){
        auto tickStart = std::chrono::steady_clock::now();

        tb->Tick(1000);

        auto tickEnd = std::chrono::steady_clock::now();

        auto tickTimeMs = std::chrono::duration_cast<std::chrono::microseconds>(tickEnd - tickStart).count();

        int32_t sleepTimeMs = tickDuration - tickTimeMs;
        if(sleepTimeMs > 0){
            std::this_thread::sleep_for(std::chrono::microseconds(sleepTimeMs));
        }else {
            printf("Logic server behind by %d microseconds.\n", sleepTimeMs);
        }
    }
}



TbServer::TbServer(){
    this->tb = new TestBench();
    this->controlInterface = new TbCommandInterface(&serverMutex, tb);
}

TbServer::~TbServer(){
    delete controlInterface;
    delete tb;
}


void TbServer::Open(uint32_t tickSpeed, std::string trace){
    if(trace != ""){
        tb->TraceOpen(trace);
    }

    tb->Reset();

    this->serverRunning = true;
    this->ticksPerSecond = tickSpeed;
    this->serverThread = new std::thread(&TbServer::ServerFunction, this);
}

void TbServer::Close(){
    SetRunning(false);
    serverThread->join();

    delete serverThread;
}


void TbServer::SetRunning(bool value){
    serverMutex.lock();
    serverRunning = value;
    serverMutex.unlock();
}

bool TbServer::GetRunning(){
    bool running;
    serverMutex.lock();
    running = serverRunning;
    serverMutex.unlock();
    return running;
}