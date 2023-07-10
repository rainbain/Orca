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
	if(tracingEnabled)trace->dump(simulationTime);
	simulationTime++;
	dut->clk = 1;
	dut->eval();
	OnPosedge();
	if(tracingEnabled)trace->dump(simulationTime);
	simulationTime++;
}

void TestBench::OnPosedge() {
	if (this->CPReadEnable) {
		if(this->CPBufferSize > this->CPBufferPosition){
			dut->GXFIFOValid = 1;
			dut->GXFIFOData = this->CPBuffer[this->CPBufferPosition];
			this->CPBufferPosition++;
		}else {
			dut->GXFIFOValid = 0;
		}
	} else {
		dut->GXFIFOValid = 0;
	}

	this->CPReadEnable = dut->GXFIFORead;
}



TestBench::TestBench() {
	Verilated::traceEverOn(true);
	simulationTime = 0;

	this->dut = new VFlipperTop;
	this->trace = new VerilatedVcdC;

	tracingEnabled = false;

	Reset();
}

TestBench::~TestBench() {
	trace->close();
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

	this->CPReadEnable = 0;
	this->CPBufferSize = 0;
	this->CPBufferPosition = 0;

	Clock();
	dut->resetn = 1;;
}

void TestBench::TraceOpen(std::string name){
	dut->trace(trace, 5);
	trace->open(name.c_str());
	tracingEnabled = true;
}


void TestBench::WriteCP(uint32_t* buffer, uint32_t size){
	this->CPBuffer = buffer;
	this->CPBufferSize = size;
	this->CPBufferPosition = 0;
}

bool TestBench::CpReady(){
	if(this->CPBufferSize == this->CPBufferPosition) return true;
	return false;
}