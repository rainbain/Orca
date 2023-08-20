// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * OrcaBoot
 * rainbain
 * 8/13/2023
 * Orca Emulator
 *
 * Orca Bootloader in charge of system configuration.
*/

#include <stdint.h>

#include "CFox/CFox.h"

#include "Init.h"

#include "KernelLoader.h"
#include "SplashScreen.h"
#include "SplashScreen.h"

#define XPPU_CTRL (*((volatile uint32_t*)0x00FF110020))

int main(){
    SplashScreen_Display();
    putstr("Initalizing Hardware\n");
    Init();
    putstr("Initalization Done\n");


    // A quick dirty fix to make sure we flush anything in the FIFO before linux clears it
    putstr("Booting in 3s.\n");
    sleep(1000);
    putstr("Booting in 2s.\n");
    sleep(1000);
    putstr("Booting in 1s.\n");
    sleep(1000);

    KernelLoader_Entry(0x00400000, 0x00200000);
    return 0;
}