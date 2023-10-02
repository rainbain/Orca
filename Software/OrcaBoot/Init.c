// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * Init
 * rainbain
 * 8/19/2023
 * Orca Emulator
 *
 * Initalizes the main system enough for the kernel to start.
*/

#include "Init.h"

#include "CFox/CFox.h"

#include <stdint.h>

uint32_t address;

void PutRegister(uint64_t addr, uint32_t value){
    volatile uint32_t* mem = (uint32_t*)addr;
    *mem = value;
}

uint32_t GetRegister(uint64_t addr){
    volatile uint32_t* mem = (uint32_t*)addr;
    return *mem;
}


void InitalizeInterface(){
    // Initalize PL Clock 0.
    // Targets PL0_REF_CTRL
    // 0x00FF5E00C0, 0x00FF5E00D0, 0x00FF5E00D4
    PutRegister(0x00FF5E00C0,
    (1<<24) |  // Enabled
    (1<<16) |  // Divider 1 is /1
    (10<<8) |  // Divider 0 is /10
    (0b000<<0) // IOPLL source
    );

    // Reset AXI FPD 0
    // Targets RST_FPD_TOP

    putstr("Resetting Flipper\n");
    PutRegister(0x00FD1A0100,
    (1<<9) //  AXI HPM0 Reset On
    );

    sleep(15); // Wait 15 ms

    PutRegister(0x00FD1A0100,
    0      //  AXI HPM0 Reset Off
    );
}

void InitalizeTTC(){
    //TTC 0, chan 0, used for sleeping.

    // Clock Setup
    // Targets Clock_Control_1
    PutRegister(0x00FF110000,
        (1<<0)                          | // Prescaler enable
        (SLEEP_TIMMER_PRESCALER<<1)     | // Prescaler value is / 128
        (0 << 5)                        | // LPD_LSBUS_CLK source
        (0 << 6)                          // No ext trigger
    );

    // Counter Setup
    // Targets Counter_Control_1
    PutRegister(0x00FF11000C,
        (0<<0)      | // Timmer enable
        (0<<1)      | // No Interval mode
        (0<<2)      | // Count up
        (0<<3)      | // Match disable
        (1<<4)      | // Reset counter
        (1<<5)      | // Disable wave
        (0<<6)        // Wave polarity. 
    );
}

void Init(){
    InitalizeTTC();
    InitalizeInterface();
}
