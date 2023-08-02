// Copyright 2023 Orca Hardware Emulator
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * System
 * rainbain
 * 7/28/2023
 * Orca Emulator
 *
 * Manages the KR260 and its components.
*/

#include <fstream>
#include <chrono>

#include "System.h"

#include "../nlohmann/json.hpp"

using namespace ZynqUSP;
using json = nlohmann::json;

void System::ServerFunction(){
    while(GetRunning()){
        auto startTime = std::chrono::high_resolution_clock::now();
        printf("hello!\n");
        auto endTime = std::chrono::high_resolution_clock::now();
        int32_t elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        int32_t remainingTime = (1000/5)-elapsedTime;
        if(remainingTime > 0){
            std::this_thread::sleep_for(std::chrono::milliseconds(remainingTime));
        }else {
            printf("System update thread fallen behind by %d miliseconds!\n", -remainingTime);
        }
    }
}


void System::LoadConfig(){
    try {
        std::ifstream f("assets/config/system.json");
        json data = json::parse(f);
    } catch(std::exception& e){
        printf("Error loading system config: \"%s\"\n", e.what());
    }
}


System::System() :
sensors(), mem() {
    LoadConfig();

    Start();
}

System::~System(){
    Stop();
}


void System::Start(){
    SetRunning(true);
    serverThread = std::thread(&System::ServerFunction, this);
}

void System::Stop(){
    SetRunning(false);
    serverThread.join();
}


void System::SetRunning(bool running){
    mtx.lock();
    this->running = running;
    mtx.unlock();
}

bool System::GetRunning(){
    bool ret;
    mtx.lock();
    ret = this->running;
    mtx.unlock();
    return ret;
}