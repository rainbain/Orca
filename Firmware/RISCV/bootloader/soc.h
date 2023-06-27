#pragma once

#include <stdint.h>

#define SYSTEM_CLOCK_SPEED 50000000


#define HARDWARE_BEGIN 0x20000000

#define GPIO_LEDS (*(volatile uint8_t*)(HARDWARE_BEGIN+0))
#define GPIO_BUTTONS (*(volatile uint8_t*)(HARDWARE_BEGIN+4))

#define GPIO_BUTTONS_KEY1 1<<0


#define UART_CALCULATE_BITRATE(bt) (((SYSTEM_CLOCK_SPEED/4) / (bt*2)) - 1)

#define UART_FLAG_BIT_TX_READY (1<<0)
#define UART_FLAG_BIT_RX_READY (1<<1)

#define UART_BEGIN HARDWARE_BEGIN + 0x00100000
#define UART_FLAGS (*(volatile uint8_t*)(UART_BEGIN+0))
#define UART_RXTX (*(volatile uint8_t*)(UART_BEGIN+8))
#define UART_CLOCK_DIV (*(volatile uint32_t*)(UART_BEGIN+12))

#define TIMMER_BEGIN HARDWARE_BEGIN + 0x00200000
#define TIMMER_PERIOD (*(volatile uint32_t*)(TIMMER_BEGIN+0))
#define TIMMER_DIVIDER (*(volatile uint32_t*)(TIMMER_BEGIN+4))
#define TIMMER_PRESCALER 64