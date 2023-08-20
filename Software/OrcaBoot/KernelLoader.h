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

#pragma once

#include "CFox/CFox.h"

void KernelLoader_Entry(uint64_t kernel_addr, uint64_t dtb_addr);