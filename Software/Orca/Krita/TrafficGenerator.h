// Copyright 2023 Orca Hardware Emulator
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * TrafficGenerator
 * rainbain
 * 9/9/2023
 * Orca Emulator
 *
 * Wrapper class for generating AXI Traffic
*/

#pragma once

#include "Memory.h"

#include "Hardware/AXITrafficGenerator.h"

namespace ZynqUSP {
    class TrafficGenerator {
            AXITrafficGenerator *generator;
            Memory *mem;

            uint8_t readParamsSize;
            uint8_t writeParamsSize;
            uint8_t readCommandsSize;
            uint8_t writeCommandsSize;
            uint8_t readAddressesSize;
            uint8_t writeAddressesSize;
            uint16_t masterRAMIndex;

        public:
            TrafficGenerator(Memory *mem, uint64_t addr);
            ~TrafficGenerator();

            void DisplayStatus();

            /*
             * Buffers
            */

            // Emptys all buffers
            void Clear();

            void PutReadParameter(uint32_t param);
            void PutWriteParameter(uint32_t param);

            void PutReadCommand(AXITrafficGeneratorCMD cmd);
            void PutWriteCommand(AXITrafficGeneratorCMD cmd);

            void PutReadAddress(uint32_t addr);
            void PutWriteAddress(uint32_t addr);

            uint32_t ReadU32(uint64_t addr);
            void WriteU32(uint64_t addr, uint32_t value);

            /*
             * Control
            */

           void PutSingleWriteCommand(uint64_t addr, uint32_t value);

           void Flush();
    };
};