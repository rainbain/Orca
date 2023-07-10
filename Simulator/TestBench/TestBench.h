// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * TestBench
 * rainbain
 * 7/5/2023
 * Orca Emulator
 *
 * Top Class that holds the Flipper test benching enviroment.
*/

#pragma once

#include <verilated.h>
#include "VFlipperTop.h"

#include <verilated_vcd_c.h>

#include <stdint.h>
#include <string>

class TestBench {
	VFlipperTop* dut;
	VerilatedVcdC *trace;

	bool tracingEnabled;
	uint64_t simulationTime;

	void Clock();
	void OnPosedge();

	// CP FIFO
	uint8_t CPReadEnable;
	uint32_t *CPBuffer;
	uint32_t CPBufferSize;
	uint32_t CPBufferPosition;

public:
	TestBench();
	~TestBench();

	void Tick(uint32_t ticks);
	void Reset();

	void TraceOpen(std::string name);

	// CP FIFO
	void WriteCP(uint32_t* buffer, uint32_t size);
	bool CpReady();
};