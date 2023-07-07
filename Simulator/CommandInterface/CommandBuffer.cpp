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

CommandBuffer::CommandBuffer(CommandInterface* interface, uint32_t bufferSize) {
	this->buffer = new uint8_t[bufferSize];
	this->bufferSize = bufferSize;
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

void CommandBuffer::Flush() {
	this->interface->WriteData(this->buffer, this->bufferSize);
	this->bufferPosition = 0;
}

void CommandBuffer::ReadData(uint8_t* data, uint32_t size) {
	this->interface->ReadData(data, size);
}