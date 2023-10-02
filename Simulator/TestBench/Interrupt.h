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

#pragma once

#include <stdint.h>
#include <functional>

#define SIMULATOR_INTERRUPT_RISING_EDGE  (1<<0)
#define SIMULATOR_INTERRUPT_FALLING_EDGE (1<<1)

class Interrupt {
    public:
        typedef std::function<void(uint32_t)> IRQHandler;
    private:

        uint8_t *irqLine;
        uint8_t observed;
        IRQHandler handler;
        uint8_t edge;

        uint32_t numInterrupts;

    public:
        Interrupt(uint8_t* irqLine, uint8_t edge);

        void OnPosedge();

        void SetHandler(IRQHandler handler);
};