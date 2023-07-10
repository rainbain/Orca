// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * CommandBuffer
 * rainbain
 * 7/3/2023
 * Orca Emulator
 *
 * Buffers commands as needed.
*/

#pragma once

#include "CommandInterface.hpp"

//
//
//
enum class GXCommand {
	NOP = 0x00,
	LOAD_BP = 0x61,
	LOAD_XF = 0x10,
	LOAD_CP = 0x08,

	LOAD_INDEXED_A = 0x20,
	LOAD_INDEXED_B = 0x28,
	LOAD_INDEXED_C = 0x30,
	LOAD_INDEXED_D = 0x38,

	CALL_DL = 0x40,
	METRICS = 0x44,
	INVL_VC = 0x48,

	PRIMITIVE_START = 0x80,
	PRIMITIVE_END = 0xBF
};


class CommandBuffer {
	uint8_t* buffer;
	uint32_t bufferSize;
	uint32_t bufferPosition;

	CommandInterface* interface;
public:
	CommandBuffer(CommandInterface* interface, uint32_t bufferSize);
	~CommandBuffer();

	void Open();
	void Close();

	/*
	 * Data Push Functions
	*/

	void PutU8(uint8_t v);
	void PutU16(uint16_t v);
	void PutU32(uint32_t v);

	/*
	 * Command Push Functions
	*/

	// Writes data to the XFMemory
	void PushXF(uint16_t startAddress, uint8_t size, uint32_t* data);

	void Flush();

	// Reading data is unbuffered
	void ReadData(uint8_t* data, uint32_t size);
};