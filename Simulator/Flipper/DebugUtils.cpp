// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * DebugUtils
 * rainbain
 * 9/4/2023
 * Orca Emulator
 *
 * A few utilitys to help get debug information.
*/

#include "DebugUtils.h"

#include <string>
#include <sstream>

#include <iostream>
#include <ctime>
#include <iomanip>

#define SSPUT_HEX_32(v) std::setw(8) << std::setfill('0') << std::hex << (v)
#define SSPUT_HEX_16(v) std::setw(4) << std::setfill('0') << std::hex << (v)
#define SSPUT_HEX_8(v) std::setw(2) << std::setfill('0') << std::hex << (v)

using namespace FlipperAPI;

std::string DebugUtils::FormattedTime(){
    std::time_t currentTime = std::time(nullptr);
    struct std::tm* timeInfo = std::localtime(&currentTime);
    std::stringstream ss;
    ss << std::put_time(timeInfo, "%Y/%m/%d %H:%M:%S");
    return ss.str();
}


std::string DebugUtils::DumpCurrentFIFO(CP* cp){
    std::ostringstream formattedStream;

    formattedStream << "Hardware FIFO Dump On [" << FormattedTime() << "]\n";

    uint64_t FIFOBase = ((uint64_t)cp->GetFIFOAXIBase() << 32) | cp->GetFIFOBase();
    uint64_t FIFOEnd = ((uint64_t)cp->GetFIFOAXIBase() << 32) | cp->GetFIFOEnd();

    formattedStream << "  Full FIFO Base Address:   0x" << SSPUT_HEX_32(FIFOBase>>32) << "_" << SSPUT_HEX_32(FIFOBase & 0xFFFFFFFF) << "\n";
    formattedStream << "  FIFO End Address:         0x" << SSPUT_HEX_32(FIFOEnd & 0xFFFFFFFF) << "\n";
    formattedStream << "  FIFO Write Pointer:       0x" << SSPUT_HEX_32(cp->GetFIFOWritePointer()) << "\n";
    formattedStream << "  FIFO Read Pointer:        0x" << SSPUT_HEX_32(cp->GetFIFOReadPointer()) << "\n";
    formattedStream << "  FIFO Write Read Distance: 0x" << SSPUT_HEX_32(cp->GetIFFOReadWriteDistance()) << "\n";
    formattedStream << "  FIFO Break Point:         0x" << SSPUT_HEX_32(cp->GetFIFOBreakpoint()) << "\n";
    formattedStream << "  FIFO Low Watermark:       0x" << SSPUT_HEX_32(cp->GetFIFOLowWatermark()) << "\n";
    formattedStream << "  FIFO High Watermark:      0x" << SSPUT_HEX_32(cp->GetFIFOHighWatermark()) << "\n";
    uint32_t fifoSize = FIFOEnd - FIFOBase;
    if(FIFOBase % 4096 != 0){
        formattedStream << "  FIFO base is not 4096 byte alined!\n";
    }
    if(fifoSize % 32 != 0){
        formattedStream << "  FIFO size is not 32 byte alined!\n";
    }
    if(fifoSize > 4096){
        formattedStream << "  FIFO size can not be greater than 4096 bytes!\n";
    }

    uint16_t fifoErrors = cp->GetFIFOErrors();

    uint8_t axiErrors = fifoErrors & 0b11;
    if(axiErrors == 0b10){
        formattedStream << "  FIFO memory buss reported device error!\n";
    }
    if(axiErrors == 0b10){
        formattedStream << "  FIFO memory buss reported decode error!\n";
    }

    formattedStream << "End of Hardware FIFO dump.\n";

    return formattedStream.str();
}

std::string DebugUtils::DumpFIFOData(CP* cp, GXFIFO fifo, size_t fifoDumpSize){
    std::ostringstream formattedStream;
    formattedStream << "FIFO DATA Dump On [" << FormattedTime() << "]\n";

    uint64_t virtualAddress = (uint64_t)fifo.buffer;

    formattedStream << "  Virtual Address:  0x" << SSPUT_HEX_32(virtualAddress>>32) << "_" << SSPUT_HEX_32(virtualAddress & 0xFFFFFFFF) << "\n";
    formattedStream << "  Physical Address: 0x" << SSPUT_HEX_32(fifo.physicalAddress>>32) << "_" << SSPUT_HEX_32(fifo.physicalAddress & 0xFFFFFFFF) << "\n";

    if(fifoDumpSize != 0xFFFFFFFFFFFFFFFF){
        fifoDumpSize *= 32;
        if(fifo.size < fifoDumpSize)fifoDumpSize = fifo.size;
    }

    formattedStream << "  | ADDR | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |\n";
    formattedStream << "  |------|-------------------------------------------------|\n";

    volatile uint8_t* fifoData = (volatile uint8_t*)fifo.buffer;
    cp->PrepareFIFORead(fifo);

    for(uint32_t i = 0; i < fifoDumpSize; i+=16){
        formattedStream << "  | " << SSPUT_HEX_16(i) << " |";
        for(uint8_t j = 0; j < 16; j++){
            uint32_t d = fifoData[j+i];
            formattedStream << " " << SSPUT_HEX_8(d);
        }
        formattedStream << " |\n";
    }

    formattedStream << "End Of FIFO Data Dump.\n";
    return formattedStream.str();
}