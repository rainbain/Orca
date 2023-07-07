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

	void PutU8(uint8_t v);
	void PutU16(uint16_t v);
	void PutU32(uint32_t v);

	void Flush();

	// Reading data is unbuffered
	void ReadData(uint8_t* data, uint32_t size);
};