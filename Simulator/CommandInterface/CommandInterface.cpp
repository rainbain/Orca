// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * CommandBuffer
 * rainbain
 * 7/3/2023
 * Orca Emulator
 *
 * Buffers commands as needed.
*/

#include "CommandInterface.hpp"

#include <stdio.h>

void CommandInterface::Open(){
    printf("Template CommandInterface functions should never be called!\n");
}
void CommandInterface::Close(){
    printf("Template CommandInterface functions should never be called!\n");
}

void CommandInterface::WriteData(uint32_t* data, uint32_t size){
    printf("Template CommandInterface functions should never be called!\n");
}

void CommandInterface::ReadData(uint32_t* data, uint32_t size){
    printf("Template CommandInterface functions should never be called!\n");
}