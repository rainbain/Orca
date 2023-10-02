// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * Interrupt
 * rainbain
 * 9/30/2023
 * Orca Emulator
 *
 * Interrupts on the test bench. Monitors a signal for an edge then triggers a handler.
*/

#include "Interrupt.h"

Interrupt::Interrupt(uint8_t* irqLine, uint8_t edge){
    this->irqLine = irqLine;
    handler = 0;
    this->edge = edge;
    this->observed = 0;
    this->numInterrupts = 0;
}

void Interrupt::OnPosedge(){
    if((*irqLine > 0) && (observed == 0) && ((edge & SIMULATOR_INTERRUPT_RISING_EDGE) > 0)){
        handler(numInterrupts);
        numInterrupts++;
    }

    if((*irqLine == 0) && (observed > 0) && ((edge & SIMULATOR_INTERRUPT_FALLING_EDGE) > 0)){
        handler(numInterrupts);
        numInterrupts++;
    }

    this->observed = *irqLine;
}

void Interrupt::SetHandler(IRQHandler handler) {
    this->handler = handler;
}