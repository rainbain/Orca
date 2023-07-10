// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * TbCommandInterface
 * rainbain
 * 7/9/2023
 * Orca Emulator
 *
 * Standured command interface for the TestBech Server
*/

#include "TbCommandInterface.h"

#include <chrono>

bool TbCommandInterface::GetCpReady(){
    bool cpReady;
    systemMutex->lock();
    cpReady = tb->CpReady();
    systemMutex->unlock();
    return cpReady;
}



TbCommandInterface::TbCommandInterface(std::mutex* systemMutex, TestBench *tb){
    this->systemMutex = systemMutex;
    this->tb = tb;
}

TbCommandInterface::~TbCommandInterface(){

}



void TbCommandInterface::Open(){

}

void TbCommandInterface::Close(){

}


void TbCommandInterface::WriteData(uint32_t* data, uint32_t size){
    while(!this->GetCpReady())std::this_thread::sleep_for(std::chrono::milliseconds(1));
    systemMutex->lock();
    tb->WriteCP(data, size);
    systemMutex->unlock();
}

void TbCommandInterface::ReadData(uint32_t* data, uint32_t size){

}