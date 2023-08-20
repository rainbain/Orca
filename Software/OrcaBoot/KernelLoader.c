// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * KernelLoader
 * rainbain
 * 8/15/2023
 * Orca Emulator
 *
 * Loads and executes a ARM64 linux kernel.
*/

#include "KernelLoader.h"

void KernelLoader_Entry(uint64_t kernel_addr, uint64_t dtb_addr) {
    // Entry code by ChatGPT, its very impresive it got it right this confused me for a bit

    
    // Define function pointer type for the kernel entry point
    typedef void (*kernel_entry_t)(uint64_t, uint64_t);

    // Set up the kernel entry point and the device tree pointer
    kernel_entry_t kernel_entry = (kernel_entry_t)kernel_addr;
    uint64_t* dtb_ptr = (uint64_t*)dtb_addr;

    // Set up the arguments for the kernel
    uint64_t boot_args[] = {
        0
    };

    // Call the kernel entry point with the arguments
    kernel_entry((uint64_t)dtb_ptr, (uint64_t)&boot_args);
}