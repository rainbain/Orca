// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * Fox
 * rainbain
 * 8/14/2023
 * Orca Emulator
 *
 * All CFox system specifc calls defined here in Orca Emulator.
*/

#include "CFox/CFox.h"
#include "Init.h"

// ttyS1
#define UART_BASE_ADDRESS (0x00FF010000)

#define UART_FIFO   (*((volatile char*)UART_BASE_ADDRESS + 0x30))
#define UART_STATUS (*((volatile char*)UART_BASE_ADDRESS + 0x2C))

#define UART_STATUS_BIT_TX_FULL  (1<<4)
#define UART_STATUS_BIT_RX_EMPTY (1<<1)

// ttc0
#define TTC_COUNT   (*((volatile uint32_t*)0x00FF110018))

void putchar(char c){
    while(UART_STATUS & UART_STATUS_BIT_TX_FULL);
    UART_FIFO = c;
}

char getchar(){
    while(UART_STATUS & UART_STATUS_BIT_RX_EMPTY);
    return UART_FIFO;
}

void sleep(uint32_t len){
    uint32_t endDate = TTC_COUNT + ((SLEET_TIMMER_PRESCALER_DIV/1000)*len);
    while(TTC_COUNT < endDate);
}