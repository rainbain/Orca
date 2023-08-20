// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * String
 * rainbain
 * 8/15/2023
 * Orca Emulator
 *
 * String Functions
*/

#pragma once

extern size_t strlen(const char *str);
extern void strflip(char *src, size_t len);

extern void tostr(char *str, size_t size, int number);

extern void tostrBase(char *str, size_t size, uint32_t number, uint8_t base);