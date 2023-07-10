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

#include "CommandBuffer.hpp"

#include <stdio.h>

CommandBuffer::CommandBuffer(CommandInterface* interface, uint32_t bufferSize) {
	this->buffer = new uint8_t[bufferSize];
	this->bufferSize = bufferSize;
	this->interface = interface;
}

CommandBuffer::~CommandBuffer() {
	delete this->buffer;
}

void CommandBuffer::Open() {
	this->interface->Open();
}

void CommandBuffer::Close() {
	this->interface->Close();
}



void CommandBuffer::PutU8(uint8_t v) {
	if (this->bufferPosition >= this->bufferSize) {
		throw("Command Buffer Out Of Room");
	}

	this->buffer[bufferPosition] = v;
	this->bufferPosition++;
}

void CommandBuffer::PutU16(uint16_t v) {
	PutU8(v >> 8);
	PutU8(v & 0xFF);
}

void CommandBuffer::PutU32(uint32_t v) {
	PutU8(v >> 24);
	PutU8((v >> 16) & 0xFF);
	PutU8((v >> 8) & 0xFF);
	PutU8(v & 0xFF);
}


void CommandBuffer::PushXF(uint16_t startAddress, uint8_t size, uint32_t* data){
	PutU8((uint8_t)GXCommand::LOAD_XF);
	if(size>16){
		printf("Can not write more than 16 bytes to XF. %d will be skipped!\n", size-16);
	}

	PutU16((size-1) & 0xF);
	PutU16(startAddress);
	for(uint32_t i = 0; i < size; i++)PutU32(data[i]);
}



void CommandBuffer::Flush() {
	// Pad until its 32 bit alined
	while(bufferPosition % 4 != 0)PutU8((uint8_t)GXCommand::NOP);

	// Anoying Swapping
	uint32_t* buffer32 = (uint32_t*)this->buffer;

	uint32_t newWord = 0;
	uint8_t* newWordArr = (uint8_t*)(&newWord);
	for(uint32_t i = 0; i < this->bufferPosition>>2; i++){
		newWordArr[3] = this->buffer[(i << 2) | 0];
		newWordArr[2] = this->buffer[(i << 2) | 1];
		newWordArr[1] = this->buffer[(i << 2) | 2];
		newWordArr[0] = this->buffer[(i << 2) | 3];
		buffer32[i] = newWord;
	}

	this->interface->WriteData((uint32_t*)this->buffer, this->bufferPosition>>2);
	this->bufferPosition = 0;
}

void CommandBuffer::ReadData(uint8_t* data, uint32_t size) {
	this->interface->ReadData((uint32_t*)data, size>>2);
}