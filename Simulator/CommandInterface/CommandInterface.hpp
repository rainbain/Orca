// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * CommandInterface
 * rainbain
 * 7/3/2023
 * Orca Emulator
 *
 * A standured virtual class between the emulator and Flippers Command Processor
*/

#include <stdint.h>

#pragma once

class CommandInterface {
public:
	virtual void Open();
	virtual void Close();

	virtual void WriteData(uint32_t* data, uint32_t size);
	virtual void ReadData(uint32_t* data, uint32_t size);
};