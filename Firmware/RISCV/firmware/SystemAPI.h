#pragma once

#include <stdint.h>

#include "soc.h"

#define API_DATA_SOURCE (UART_RXTX)
#define API_RX_READY ((UART_FLAGS & UART_FLAG_BIT_RX_READY) > 0)
#define API_TX_READY ((UART_FLAGS & UART_FLAG_BIT_TX_READY) > 0)

typedef enum {
	HOST_COMMAND_NONE,
	HOST_COMMAND_GET,
	HOST_COMMAND_PUT
} HostCommand;

typedef enum {
	DEVICE_COMMAND_NONE,
	DEVICE_COMMAND_ACK,
	DEVICE_COMMAND_NACK
} DeviceCommand;

typedef enum {
	DEVICE_NACK_CRC_FAIL,
	DEVICE_NACK_UNKNOWN_COMMAND
} NackReason;

typedef struct {
	HostCommand command;
	uint32_t size;
	uint32_t crc;
} SystemAPIState;


// Reads a 4 byte word into pointer
void SystemAPIReadWord(uint32_t *word);

// Reads a 4 byte word into pointer and accumulates CRC
void SystemAPIReadWordCRC(SystemAPIState *s, uint32_t *word);

// Writes a 4 byte word with crc
void SystemAPIWriteWord(uint32_t *word);

// Gets a command into state and is blocking
void SystemAPIGetCommand(SystemAPIState *s);

// Download Some Data
void SystemAPIDownload(SystemAPIState *s, uint32_t *data, uint32_t size);

// End CRC
void SystemAPIEnd(SystemAPIState *s);

// Send End Code
void SystemAPISendEndCode(DeviceCommand cmd, uint8_t param1, uint8_t param2, uint8_t param3);