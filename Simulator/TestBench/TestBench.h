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

#include <stdint.h>

class TestBench {
	VFlipperTop* dut;

	void Clock();
	void OnPosedge();

public:
	TestBench();
	~TestBench();

	void Tick(uint32_t ticks);
	void Reset();
};