// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * Memory Map
 * rainbain
 * 9/29/2023
 * Orca Emulator
 *
 * Flipper Memory Addresses. Used to prevent circular includes
*/

#pragma once

#define FLIPPER_INSTALL_ADDRESS 0x4CC000000
#define FLIPPER_ADDRESS_SPACE   0x10000
#define FLIPPER_INSTALL_NAME "Flipper"

#define FLIPPER_CP_OFFSET       0x00000000
#define FLIPPER_WPAR_OFFSET     (FLIPPER_CP_OFFSET | 0x00008000)