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

#define NORMAL_DSP_BEGIN HARDWARE_BEGIN + 0x00300000
#define NORMAL_DSP_NM00 (*(volatile uint32_t*)(NORMAL_DSP_BEGIN+0*4))
#define NORMAL_DSP_NM10 (*(volatile uint32_t*)(NORMAL_DSP_BEGIN+1*4))
#define NORMAL_DSP_NM20 (*(volatile uint32_t*)(NORMAL_DSP_BEGIN+2*4))
#define NORMAL_DSP_NM01 (*(volatile uint32_t*)(NORMAL_DSP_BEGIN+3*4))
#define NORMAL_DSP_NM11 (*(volatile uint32_t*)(NORMAL_DSP_BEGIN+4*4))
#define NORMAL_DSP_NM21 (*(volatile uint32_t*)(NORMAL_DSP_BEGIN+5*4))
#define NORMAL_DSP_NM02 (*(volatile uint32_t*)(NORMAL_DSP_BEGIN+6*4))
#define NORMAL_DSP_NM12 (*(volatile uint32_t*)(NORMAL_DSP_BEGIN+7*4))
#define NORMAL_DSP_NM22 (*(volatile uint32_t*)(NORMAL_DSP_BEGIN+8*4))

#define NORMAL_DSP_NNX (*(volatile uint32_t*)(NORMAL_DSP_BEGIN+9*4))
#define NORMAL_DSP_NNY (*(volatile uint32_t*)(NORMAL_DSP_BEGIN+10*4))
#define NORMAL_DSP_NNZ (*(volatile uint32_t*)(NORMAL_DSP_BEGIN+11*4))
#define NORMAL_DSP_NTX (*(volatile uint32_t*)(NORMAL_DSP_BEGIN+12*4))
#define NORMAL_DSP_NTY (*(volatile uint32_t*)(NORMAL_DSP_BEGIN+13*4))
#define NORMAL_DSP_NTZ (*(volatile uint32_t*)(NORMAL_DSP_BEGIN+14*4))
#define NORMAL_DSP_NBX (*(volatile uint32_t*)(NORMAL_DSP_BEGIN+15*4))
#define NORMAL_DSP_NBY (*(volatile uint32_t*)(NORMAL_DSP_BEGIN+16*4))
#define NORMAL_DSP_NBZ (*(volatile uint32_t*)(NORMAL_DSP_BEGIN+17*4))

#define NORMAL_DSP_OP (*(volatile uint8_t*)(NORMAL_DSP_BEGIN+18*4))

#define NORMAL_DSP_ONX (*(volatile uint32_t*)(NORMAL_DSP_BEGIN+19*4))
#define NORMAL_DSP_ONY (*(volatile uint32_t*)(NORMAL_DSP_BEGIN+20*4))
#define NORMAL_DSP_ONZ (*(volatile uint32_t*)(NORMAL_DSP_BEGIN+21*4))
#define NORMAL_DSP_OTX (*(volatile uint32_t*)(NORMAL_DSP_BEGIN+22*4))
#define NORMAL_DSP_OTY (*(volatile uint32_t*)(NORMAL_DSP_BEGIN+23*4))
#define NORMAL_DSP_OTZ (*(volatile uint32_t*)(NORMAL_DSP_BEGIN+24*4))
#define NORMAL_DSP_OBX (*(volatile uint32_t*)(NORMAL_DSP_BEGIN+25*4))
#define NORMAL_DSP_OBY (*(volatile uint32_t*)(NORMAL_DSP_BEGIN+26*4))
#define NORMAL_DSP_OBZ (*(volatile uint32_t*)(NORMAL_DSP_BEGIN+27*4))