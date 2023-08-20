// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * CFox
 * rainbain
 * 8/14/2023
 * Orca Emulator
 *
 * A "offbrand" libc like implmentation. This code will be added to all my embedded platforms.
*/

#pragma once

// If defined, the putstr function will add a carrige return automaticly.
// It will also cause getstr to treat them as a new line
#define ABSTRACT_CARRIGE_RETURN

// Externally defined
extern void putchar(char c);
extern char getchar();
extern void sleep(uint32_t len); //ms

// CFox implmented functions
extern void putstr(const char * str);
extern void getstr(char * str, size_t len, char terminator, bool echo);