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

#include "CFox.h"

void putstr(const char * str){
    while(*str){
        putchar(*str);
#ifdef ABSTRACT_CARRIGE_RETURN
        if(*str == '\n')putchar('\r');
#endif
        str++;
    }
}

void getstr(char * str, size_t len, char terminator, bool echo){
    len--; // Subtract one since we want to leave room for zero terminator.
    while(len > 0){
        *str = getchar();

        // Treat carrige return as new line
#ifdef ABSTRACT_CARRIGE_RETURN
        if(*str == '\r') *str = '\n';
#endif

        if(*str == terminator) break;
        if(echo)putchar(*str);

        len--;
        *str++;
    }
    *str = 0;
}