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
    for(uint8_t i = 0; i < 3; i++)
        ttc->match[i][channel] = 0xFFFFFFFF;

    ttc->interval_cnt[channel] = 100000000 / ZYNQUSP_FANCONTROLLER_PWM_SPEED_HERTZ;

    SetSpeed(85);
    ttc->clk_ctrl[channel] =
                                (0<<0)      | // Prescaler disable
                                (0b0000<<1) | // Prescaler = 0
                                (0<<5)      | // LPD_LSBUS_CLK clock source
                                (0<<6)      ; // No external clock edge

    ttc->cnt_ctrl[channel] =
                                (0<<0)      | // Counter enable
                                (1<<1)      | // Interval Mode Enable
                                (0<<2)      | // Count up
                                (1<<3)      | // Match Enable
                                (1<<4)      | // Resets counter
                                (0<<5)      | // Wave enable.
                                (0<<6)      ; // Invert wave since fan is inverted.

}

void FanController::SetSpeed(uint8_t percent){
    ttc->match[0][channel] = 100000000 / ZYNQUSP_FANCONTROLLER_PWM_SPEED_HERTZ / 100 * percent;
}

FanController::FanController(){
}

void FanController::Initalize(TTC* tcc, uint8_t channel){
    this->ttc = tcc;
    this->channel = channel;

    SetupTimmer();
}

FanController::~FanController(){

}