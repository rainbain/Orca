// Copyright 2023 Orca Hardware Emulator
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * FanController
 * rainbain
 * 7/28/2023
 * Orca Emulator
 *
 * CPU Tripple Timmer 0 Channel 0 is used to control the system fan.
*/

#include "FanController.h"

#include <stdio.h>
#include <stdint.h>

using namespace ZynqUSP;

void FanController::SetupTimmer() {
    SetSpeed(100);

    timmer->tim0_ctrl = 0b001000000100;
    timmer->tim1_ctrl = 0b011000000100;
}

void FanController::SetSpeed(uint8_t percent){
    timmer->tim0_load = 500000000;
    timmer->tim1_load = 1000000000;
}

FanController::FanController(AXITimmer* timmer){
    this->timmer = timmer;

    SetupTimmer();
}

FanController::~FanController(){

}

void FanController::Update(){
    printf("%d \n", this->timmer->tim0_count);
}