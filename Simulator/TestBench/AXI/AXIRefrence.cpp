// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * AXIRefrence
 * rainbain
 * 7/16/2023
 * Orca Emulator
 *
 * A refrence to some AXI port passed around by the software.
 * Also contains data strucutres and enums used by AXI
*/

#include "AXIRefrence.h"

const uint8_t AXISizeLookup[] = {
    1, 2, 4, 8, 16, 32, 64, 128
};

const uint8_t AXI16StrobeLookup[] = {
    0b0011, 0, 0b1100, 0
};