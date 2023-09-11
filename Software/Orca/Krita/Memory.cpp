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



size_t Memory::GetPageSize(){
    return sysconf(_SC_PAGESIZE);
}


int Memory::LockPage(void *pg, size_t len){
    return mlock(pg, len);
}

int Memory::UnlockPage(void *pg, size_t len){
    return munlock(pg, len);
}

// Credit to https://blakerain.com/blog/allocating-memory-for-dma-in-linux
uint64_t Memory::GetPhysicalAddress(void *pg){
    size_t pageSize = GetPageSize();

    int fd = open("/proc/self/pagemap", O_RDONLY);
    if(fd == -1) return 0;

    int res = ::lseek64(fd, (uintptr_t)pg / pageSize * sizeof(uintptr_t), SEEK_SET);
    if(res == -1) return 0;

    uintptr_t phy = 0;
    res = read(fd, &phy, sizeof(uintptr_t));

    if(res != sizeof(uintptr_t)) return 0;

    close(fd);

    if((phy & ((uintptr_t)1<<63)) == 0) return 0;

    return (phy & 0x7fffffffffffffULL) * pageSize + (uintptr_t)pg % pageSize;
}

void Memory::InvalidateCache(void *pg, size_t size){
    size_t cache_line_size = 64;
    void* end_addr = static_cast<uint8_t*>(pg) + size;

    for (void* p = pg; p < end_addr; p = static_cast<uint8_t*>(p) + cache_line_size) {
        asm volatile("DC CIVAC, %0" : : "r"(p));
    }

    asm volatile("DSB SY");
    asm volatile("ISB");
}