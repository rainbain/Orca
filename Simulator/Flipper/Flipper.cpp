// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * Flipper
 * rainbain
 * 9/2/2023
 * Orca Emulator
 *
 * Simulator Flipper API
*/

#include "Flipper.h"

using namespace FlipperAPI;

void Flipper::FlipperIRQHandler(uint32_t interruptCount){
        printf("Interrupt recived!\n");
}



Flipper::Flipper(AXILiteIF *cpu, Interrupt *irq) :
        cp(cpu) {
    irq->SetHandler(std::bind(&Flipper::FlipperIRQHandler, this, std::placeholders::_1));
}

Flipper::~Flipper(){
    
}

CP* Flipper::GetCP(){
        return &cp;
}