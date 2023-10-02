// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * Flipper
 * rainbain
 * 9/3/2023
 * Orca Emulator
 *
 * Flipper Base Class
*/

#include "Flipper.h"

#include "MemoryMap.h"

using namespace FlipperAPI;

void Flipper::FlipperIRQHandler(uint32_t interruptCount){
    printf("Interrupt Recived!\n");
}


Flipper::Flipper(ZynqUSP::System *sys) {
    ZynqUSP::UIOIndexing* indexor = sys->GetUIOIndexing();

    indexor->IndexUIO(&flipperUIO, FLIPPER_INSTALL_ADDRESS, FLIPPER_ADDRESS_SPACE, FLIPPER_INSTALL_NAME);

    flipperUIO.SetupInterrupts(std::bind(&Flipper::FlipperIRQHandler, this, std::placeholders::_1));

    cp = CP(sys, &flipperUIO);
}

Flipper::~Flipper(){

}

CP* Flipper::GetCP(){
    return &this->cp;
}