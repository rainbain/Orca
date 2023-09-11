// Copyright 2023 Orca Hardware Emulator
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * Memory
 * rainbain
 * 7/28/2023
 * Orca Emulator
 *
 * Manages memory between the hardware and user space software.
*/

#pragma once

#include <cstddef>
#include <stdint.h>

namespace ZynqUSP {
    class Memory {
        int memFile;
        bool valid;

    public:
        Memory();
        ~Memory();

        /*
         * System Hardware Acess
        */

        // Maps a hardware memory address into virtual memory
        void* Map(uint64_t address, size_t size);

        // Unmaps a hardware memory address out of virtual memory
        void UnMap(void *memory, size_t size);


        /*
         * Hardware memory allocation.
        */

       // Gets the size of one page.
       // To create buffers larger than this linux "hudge pages" must be used.
       size_t GetPageSize();

       // Locks a page from being moved or swapped
       int LockPage(void *pg, size_t len);
       int UnlockPage(void *pg, size_t len);

       // Gets the physical address of some virtual address memory.
       // Must not go over one page, page should be locked.
       uint64_t GetPhysicalAddress(void *pg);

       // Must be used before accessing memory changed by the DMA
       void InvalidateCache(void *pg, size_t size);
    };
};