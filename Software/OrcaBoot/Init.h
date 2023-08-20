// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * Init
 * rainbain
 * 8/19/2023
 * Orca Emulator
 *
 * Initalizes the main system enough for the kernel to start.
*/

#pragma once

#define SLEEP_TIMMER_PRESCALER 4

// 100000000 / 2^(N)
#define SLEET_TIMMER_PRESCALER_DIV (100000000 / 32)

extern void Init();
