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

#include "CFox.h"

size_t strlen(const char *str){
    size_t len;
    while(*str){
        len++;
        str++;
    }
    return len;
}

void strflip(char *src, size_t len){
    size_t i, j;
    char temp;
    for (i = 0, j = len - 1; i < j; i++, j--) {
        temp = src[i];
        src[i] = src[j];
        src[j] = temp;
    }
}


const char strConversionTable[] = {'0', '1', '2', '3', '4',
                                   '5', '6', '7', '8', '9',
                                   'A', 'B', 'C', 'D', 'E', 'F'};

void tostr(char *str, size_t size, int number){
    size_t currentIndex = 0;

    if (number < 0) {
        *str = '-';
        *str++;
        number = -number;
    }

    do {
        str[currentIndex++] = strConversionTable[number % 10];
        number /= 10;
    } while (number != 0 && currentIndex < size - 1);

    str[currentIndex] = '\0'; // Null-terminate the string

    strflip(str, currentIndex); // Reverse the string
}

void tostrBase(char *str, size_t size, uint32_t number, uint8_t base){
    if (base < 2 || base > 16) {
        // Unsupported base, do not convert
        str[0] = '\0';
        return;
    }

    size_t currentIndex = 0;

    do {
        str[currentIndex++] = strConversionTable[number % base];
        number /= base;
    } while (number != 0 && currentIndex < size - 1);

    str[currentIndex] = '\0'; // Null-terminate the string

    strflip(str, currentIndex); // Reverse the string
}