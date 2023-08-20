// Copyright 2023 Orca Hardware Emulator
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * Memory
 * rainbain
 * 7/28/2023
 * Orca Emulator
 *
 * Get pointers to devices in the SoC address space via /dev/mem
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

        void* Map(uint64_t address, size_t size);
        void UnMap(void *memory, size_t size);
    };
};