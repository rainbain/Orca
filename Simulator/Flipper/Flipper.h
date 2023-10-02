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

#include "GX/CP.h"

#include "../TestBench/AXI/AXILiteIF.h"
#include "../TestBench/Interrupt.h"

namespace FlipperAPI{
    class Flipper{
        CP cp;

        void FlipperIRQHandler(uint32_t interruptCount);
    public:
        Flipper(AXILiteIF *cpu, Interrupt *irq);
        ~Flipper();

        CP* GetCP();
    };
}