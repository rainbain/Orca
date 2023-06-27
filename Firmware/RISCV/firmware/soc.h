#pragma once

#include <stdint.h>

#define SYSTEM_CLOCK_SPEED 50000000


#define HARDWARE_BEGIN 0x20000000

#define GPIO_LEDS (*(volatile uint8_t*)(HARDWARE_BEGIN+0))


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

#define MATRIX_DSP_BEGIN HARDWARE_BEGIN + 0x00300000
#define MATRIX_DSP_X (*(volatile uint32_t*)(MATRIX_DSP_BEGIN+0))
#define MATRIX_DSP_Y (*(volatile uint32_t*)(MATRIX_DSP_BEGIN+(1*4)))
#define MATRIX_DSP_Z (*(volatile uint32_t*)(MATRIX_DSP_BEGIN+(2*4)))
#define MATRIX_DSP_M00 (*(volatile uint32_t*)(MATRIX_DSP_BEGIN+(3*4)))
#define MATRIX_DSP_M10 (*(volatile uint32_t*)(MATRIX_DSP_BEGIN+(4*4)))
#define MATRIX_DSP_M01 (*(volatile uint32_t*)(MATRIX_DSP_BEGIN+(5*4)))
#define MATRIX_DSP_M11 (*(volatile uint32_t*)(MATRIX_DSP_BEGIN+(6*4)))
#define MATRIX_DSP_M02 (*(volatile uint32_t*)(MATRIX_DSP_BEGIN+(7*4)))
#define MATRIX_DSP_M12 (*(volatile uint32_t*)(MATRIX_DSP_BEGIN+(8*4)))

#define MATRIX_DSP_OPERATION (*(volatile uint8_t*)(MATRIX_DSP_BEGIN+(9*4)))

#define MATRIX_DSP_OUT_X (*(volatile uint32_t*)(MATRIX_DSP_BEGIN+(10*4)))
#define MATRIX_DSP_OUT_Y (*(volatile uint32_t*)(MATRIX_DSP_BEGIN+(11*4)))
#define MATRIX_DSP_OUT_Z (*(volatile uint32_t*)(MATRIX_DSP_BEGIN+(12*4)))

#define FLOAT_DSP_BEGIN HARDWARE_BEGIN + 0x00400000
#define FLOAT_DSP_A (*(volatile uint32_t*)(FLOAT_DSP_BEGIN+0))
#define FLOAT_DSP_B (*(volatile uint32_t*)(FLOAT_DSP_BEGIN+4))
#define FLOAT_DSP_O (*(volatile uint8_t*)(FLOAT_DSP_BEGIN+8))
#define FLOAT_DSP_R (*(volatile uint32_t*)(FLOAT_DSP_BEGIN+12))