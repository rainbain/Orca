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

#include "TestBench.h"

void TestBench::Clock() {
	dut->clk = 0;
	dut->eval();
	dut->clk = 1;
	dut->eval();
	OnPosedge();
}

void TestBench::OnPosedge() {
	if (dut->resetn == 0) {
		return;
	}

	dut->GXFIFOValid = dut->GXFIFORead;
	if (dut->GXFIFORead) {
		uint32_t randomDataIn = rand();
		printf("Fifo In: %08x!\n", randomDataIn);
		dut->GXFIFOData = randomDataIn;
	}
}



TestBench::TestBench() {
	this->dut = new VFlipperTop;

	Reset();
}

TestBench::~TestBench() {
	dut->final();
}


void TestBench::Tick(uint32_t ticks) {
	for (uint32_t i = 0; i < ticks; i++) {
		Clock();
	}
}

void TestBench::Reset() {
	dut->resetn = 0;
	dut->GXFIFOValid = 0;

	Clock();
	dut->resetn = 1;;
}