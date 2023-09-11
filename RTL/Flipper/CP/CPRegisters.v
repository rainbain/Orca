// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * CPRegisters
 * rainbain
 * 7/19/2023
 * Orca Emulator
 *
 * The command processors registers and their logic.
*/

module CPRegisters(
    //
    // Top Level
    //
    input wire clk, input wire resetn,


    //
    // CPU Interface
    //
    input wire CPURead, input wire CPUWrite, input wire[5:0] CPUAddress,
    output wire[31:0] CPUReadData, input wire[31:0] CPUWriteData, input wire[3:0] CPUStrobe,

    //
    // BBox
    //
    input wire[15:0] BBoxLeft, input wire[15:0] BBoxRight,
    input wire[15:0] BBoxTop, input wire[15:0] BBoxBottom,

    //
    // FIFO
    //    
    output reg[31:0] FIFOBase, output reg[31:0] FIFOEnd,
    output reg[31:0] FIFOHighWatermark, output reg[31:0] FIFOLowWatermark,
    input wire[31:0] FIFORWDistance, input wire[31:0] FIFOWritePointer, input wire[31:0] FIFOReadPointer,
    output reg[31:0] FIFOBreakpoint,

    //
    // Interrupt Vector And Control
    //

    input wire IntBP, input wire IntFIFOverflow, input wire IntFIFOUnderflow,
    input wire StatGPIdle, input wire StatGPReadIdle,

    output reg EnBP, output reg EnGPLink, output reg EnFIFOUnderflow, output reg EnFIFOOverflow,
    output reg CpIRQEn, output reg EnGPFIFO,
    
    output reg FIFONewBase,
    
    //
    // Custom Orca Only Registers
    //
    
    output reg[31:0] FIFOAXIBase, input wire[15:0] FIFOErrors
    
);

// Address Decoder
wire[3:0] shortAlinedAddr = CPUAddress[5:2]; // Remove the last bit as its 32 bit alined

wire REGISTER_BANK_0 = shortAlinedAddr == 4'h00;
wire REGISTER_BANK_1 = shortAlinedAddr == 4'h01;
wire REGISTER_BANK_2 = shortAlinedAddr == 4'h02;
wire REGISTER_BANK_3 = shortAlinedAddr == 4'h03;
wire REGISTER_BANK_4 = shortAlinedAddr == 4'h04;
wire REGISTER_BANK_5 = shortAlinedAddr == 4'h05;
wire REGISTER_BANK_6 = shortAlinedAddr == 4'h06;
wire REGISTER_BANK_7 = shortAlinedAddr == 4'h07;
wire REGISTER_BANK_8 = shortAlinedAddr == 4'h08;
wire REGISTER_BANK_9 = shortAlinedAddr == 4'h09;
wire REGISTER_BANK_A = shortAlinedAddr == 4'h0A;
wire REGISTER_BANK_B = shortAlinedAddr == 4'h0B;
wire REGISTER_BANK_C = shortAlinedAddr == 4'h0C;
wire REGISTER_BANK_D = shortAlinedAddr == 4'h0D;
wire REGISTER_BANK_E = shortAlinedAddr == 4'h0E;
wire REGISTER_BANK_F = shortAlinedAddr == 4'h0F;

// Status Register
wire ClearBreakpoint;
wire ClearFIFOUnderflow;
wire ClearFIFOOverflow;

wire[15:0] StatusRegister;
assign StatusRegister[15:5] = 0;
reg BpInterruptMet;
reg FIFOUnderflowMet;
reg FIFOOverflowMet;

always @ (posedge clk) begin
    if(ClearBreakpoint | (~resetn))BpInterruptMet <= 0;
    else if(IntBP)BpInterruptMet <= 1;

    if(ClearFIFOUnderflow | (~resetn))FIFOUnderflowMet <= 0;
    else if(IntFIFOUnderflow)FIFOUnderflowMet <= 1;

    if(ClearFIFOOverflow | (~resetn))FIFOOverflowMet <= 0;
    else if(IntFIFOverflow)FIFOOverflowMet <= 1;
end

// Control Register And Clear Logic
wire[15:0] ControlRegister;
assign ControlRegister[15:6] = 0;
assign ControlRegister[5] = EnBP;
assign ControlRegister[4] = EnGPLink;
assign ControlRegister[3] = EnFIFOUnderflow;
assign ControlRegister[2] = EnFIFOOverflow;
assign ControlRegister[1] = CpIRQEn;
assign ControlRegister[0] = EnGPFIFO;

// Weird Clear logic
assign ClearBreakpoint = (CPUWrite & REGISTER_BANK_0 & CPUStrobe[2] & CPUWriteData[17]);
assign ClearFIFOOverflow = (CPUWrite & REGISTER_BANK_1 & CPUStrobe[0] & CPUWriteData[0]);
assign ClearFIFOUnderflow = (CPUWrite & REGISTER_BANK_1 & CPUStrobe[0] & CPUWriteData[1]);

always @ (posedge clk) begin
    if(~resetn) begin
        EnBP <= 0;
        EnGPLink <= 0;
        EnFIFOUnderflow <= 0;
        EnFIFOOverflow <= 0;
        CpIRQEn <= 0;
        EnGPFIFO <= 0;
    end else begin
        if(CPUWrite & REGISTER_BANK_0 & CPUStrobe[0]) begin
            EnBP <= CPUWriteData[5];
            EnGPLink <= CPUWriteData[4];
            EnFIFOUnderflow <= CPUWriteData[3];
            EnFIFOOverflow <= CPUWriteData[2];
            CpIRQEn <= CPUWriteData[1];
            EnGPFIFO <= CPUWriteData[0];
        end
    end
end

wire[15:0] zeros = 0;

// Read Encoder
assign CPUReadData = CPURead ?
                                REGISTER_BANK_0 ? {ControlRegister, StatusRegister} :
                                REGISTER_BANK_1 ? {zeros, zeros} :
                                REGISTER_BANK_3 ? {zeros, FIFOErrors} :
                                REGISTER_BANK_4 ? {BBoxRight, BBoxLeft} :
                                REGISTER_BANK_5 ? {BBoxBottom, BBoxTop} :
                                
                                REGISTER_BANK_7 ? FIFOAXIBase :
                                REGISTER_BANK_8 ? FIFOBase :
                                REGISTER_BANK_9 ? FIFOEnd :
                                REGISTER_BANK_A ? FIFOHighWatermark :
                                REGISTER_BANK_B ? FIFOLowWatermark :
                                REGISTER_BANK_C ? FIFORWDistance :
                                REGISTER_BANK_D ? FIFOWritePointer:
                                REGISTER_BANK_E ? FIFOReadPointer :
                                REGISTER_BANK_F ? FIFOBreakpoint : 0
                                : 0;

// Write Handler Non Ctl/Reset

always @ (posedge clk) begin
    if(CPUWrite) begin
        if(REGISTER_BANK_7) begin
            if(CPUStrobe[0]) FIFOAXIBase[7:0] <= CPUWriteData[7:0];
            if(CPUStrobe[1]) FIFOAXIBase[15:8] <= CPUWriteData[15:8];
            if(CPUStrobe[2]) FIFOAXIBase[23:16] <= CPUWriteData[23:16];
            if(CPUStrobe[3]) FIFOAXIBase[31:24] <= CPUWriteData[31:24];
        end
        
        if(REGISTER_BANK_8) begin
            if(CPUStrobe[0]) FIFOBase[7:0] <= CPUWriteData[7:0];
            if(CPUStrobe[1]) FIFOBase[15:8] <= CPUWriteData[15:8];
            if(CPUStrobe[2]) FIFOBase[23:16] <= CPUWriteData[23:16];
            if(CPUStrobe[3]) FIFOBase[31:24] <= CPUWriteData[31:24];
            FIFONewBase <= 1;
        end else FIFONewBase <= 0;
        
        if(REGISTER_BANK_9) begin
            if(CPUStrobe[0]) FIFOEnd[7:0] <= CPUWriteData[7:0];
            if(CPUStrobe[1]) FIFOEnd[15:8] <= CPUWriteData[15:8];
            if(CPUStrobe[2]) FIFOEnd[23:16] <= CPUWriteData[23:16];
            if(CPUStrobe[3]) FIFOEnd[31:24] <= CPUWriteData[31:24];
        end
        
        if(REGISTER_BANK_A) begin
            if(CPUStrobe[0]) FIFOHighWatermark[7:0] <= CPUWriteData[7:0];
            if(CPUStrobe[1]) FIFOHighWatermark[15:8] <= CPUWriteData[15:8];
            if(CPUStrobe[2]) FIFOHighWatermark[23:16] <= CPUWriteData[23:16];
            if(CPUStrobe[3]) FIFOHighWatermark[31:24] <= CPUWriteData[31:24];
        end
        
        if(REGISTER_BANK_B) begin
            if(CPUStrobe[0]) FIFOLowWatermark[7:0] <= CPUWriteData[7:0];
            if(CPUStrobe[1]) FIFOLowWatermark[15:8] <= CPUWriteData[15:8];
            if(CPUStrobe[2]) FIFOLowWatermark[23:16] <= CPUWriteData[23:16];
            if(CPUStrobe[3]) FIFOLowWatermark[31:24] <= CPUWriteData[31:24];
        end

        if(REGISTER_BANK_F) begin
            if(CPUStrobe[0]) FIFOBreakpoint[7:0] <= CPUWriteData[7:0];
            if(CPUStrobe[1]) FIFOBreakpoint[15:8] <= CPUWriteData[15:8];
            if(CPUStrobe[2]) FIFOBreakpoint[23:16] <= CPUWriteData[23:16];
            if(CPUStrobe[3]) FIFOBreakpoint[31:24] <= CPUWriteData[31:24];
        end
    end else begin
        FIFONewBase <= 0;
    end
end

endmodule
