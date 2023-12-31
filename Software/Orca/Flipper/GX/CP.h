// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * CP
 * rainbain
 * 9/2/2023
 * Orca Emulator
 *
 * Simulator CP low level API acess.
*/

#pragma once

#include "../../Krita/System.h"

namespace FlipperAPI {
    struct CPWord {
        uint16_t L;
        uint16_t H;
    };

    struct CPRegisters {
        uint16_t SR;           // 0x000
        uint16_t CR;           // 0x002
        uint16_t CLEAR;        // 0x004
        uint16_t reserved1[3]; // 0x006
        uint16_t FIFO_Errors;  // 0x00C
        uint16_t TOKEN;        // 0x00E
        uint16_t BBOX_LEFT;    // 0x010
        uint16_t BBOX_RIGHT;   // 0x012
        uint16_t BBOX_TOP;     // 0x014
        uint16_t BBOX_BOTTOM;  // 0x016
        uint16_t reserved2[2]; // 0x018
        CPWord FIFO_AXIBase;   // 0x01C (Orca Only Register)
        CPWord FIFO_BASE;      // 0x020
        CPWord FIFO_END;       // 0x024
        CPWord FIFO_HWM;       // 0x028
        CPWord FIFO_LWM;       // 0x02C
        CPWord FIFO_WRD;       // 0x030
        CPWord FIFO_WRP;       // 0x034
        CPWord FIFO_RDP;       // 0x038
        CPWord FIFO_BP;        // 0x03C
    };

    struct GXFIFO {
        void *buffer;
        
        uint64_t physicalAddress;
        uint32_t size;
    };

    class CP{
        volatile CPRegisters* cpRegisters;
        volatile void *wpar;

        ZynqUSP::Memory *mem;

        void SetU32(volatile CPWord *word, uint32_t value);
        uint32_t GetU32(volatile CPWord *word);

    public:
        CP();
        CP(ZynqUSP::System *system, ZynqUSP::UIO* flipperUIO);
        ~CP();

        /*
         * Status/Control Wrappers
        */

       uint16_t GetStatus();
       uint16_t GetControl();

       // Pass status register
       bool IsBPInterrupt(uint16_t s);
       bool IsGPCommandsIdle(uint16_t s);
       bool IsGPReadIdle(uint16_t s);
       bool IsFIFOUnderflowInterrupt(uint16_t s);
       bool IsFIFOOverflowInterrupt(uint16_t s);
       
       // Pass Controll Register
       bool IsBPEnabled(uint16_t c);
       bool IsGPLinkEnabled(uint16_t c);
       bool IsFIFOOverflowIRQEnabled(uint16_t c);
       bool IsFIFOUnderflowIRQEnabled(uint16_t c);
       bool IsCPIRQEnabled(uint16_t c);
       bool IsGPFIFOReadEnabled(uint16_t c);

       // Enable CP IRQ to clear BP interrupts
       void SetBPEnable(bool enable);
       void SetGPLinkEnable(bool enable);
       void SetFIFOUnderflowIRQEnable(bool enable);
       void SetFIFOOverflowIRQEnable(bool enable);
       void SetCPIRQEnable(bool enable);
       void SetGPFIFOReadEnable(bool enable);

       void ClearFIFOUnderflowIRQ();
       void ClearFIFOOverflowIRQ();
       void ClearAllIntrrupts(); // Warning, turns on interrupts!


        /*
         * FIFO Wrapper
        */

        void SetFIFOBase(uint32_t addr);
        void SetFIFOEnd(uint32_t addr);

        void SetFIFOAXIBase(uint32_t addr);
        void SetFIFOHighWatermak(uint32_t addr);
        void SetFIFOLowWatermark(uint32_t addr);
        void SetFIFOBreakpoint(uint32_t addr);


        uint32_t GetFIFOBase();
        uint32_t GetFIFOEnd();

        uint32_t GetFIFOAXIBase();
        uint32_t GetFIFOHighWatermark();
        uint32_t GetFIFOLowWatermark();
        uint32_t GetFIFOBreakpoint();

        uint32_t GetFIFOWritePointer();
        uint32_t GetFIFOReadPointer();
        uint32_t GetIFFOReadWriteDistance();

        uint16_t GetFIFOErrors();

        void WriteU8(uint8_t v);
        void WriteU16(uint16_t v);
        void WriteU32(uint32_t v);

        /*
         * FIFO Management
        */

        // Create and destroy FIFOs from memory.
        // The size is outside of our control due to memory page limitations.
        GXFIFO CreateGXFIFO();
        void DestroyFIFO(GXFIFO fifo);

        // Must be called before the user reads from a fifo. Writing is not recommended.
        void PrepareFIFORead(GXFIFO fifo);

        // Load a FIFO onto the hardware
        void LoadFIFO(GXFIFO fifo);
    };
};