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

#include "Memory.h"

#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

using namespace ZynqUSP;


Memory::Memory(){
    valid = false;

    memFile = open("/dev/mem", O_RDWR | O_SYNC);

    if(memFile == -1){
        printf("Failed to open \"/dev/mem\"!\n");
        return;
    }

    valid = true;
}

Memory::~Memory(){
    if(!valid) return;

    close(memFile);
}

void* Memory::Map(uint64_t address, size_t size){
    if(!valid) return 0;

    void* mappedMemory = mmap(
        nullptr, size, PROT_READ | PROT_WRITE,
        MAP_SHARED, memFile, address
    );

    if(mappedMemory == MAP_FAILED){
        return 0;
    }

    return mappedMemory;
}

void Memory::UnMap(void *memory, size_t size){
    if(!valid) return;
    munmap(memory, size);
}