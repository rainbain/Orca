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
    output wire[15:0] CPUReadData, input wire[15:0] CPUWriteData,

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
    output reg CpIRQEn, output reg EnGPFIFO
    
);

// Address Decoder
wire[4:0] shortAlinedAddr = CPUAddress[5:1]; // Remove the last bit as its 16 bit alined
wire SR_SELECT = shortAlinedAddr == 5'h00;
wire CR_SELECT = shortAlinedAddr == 5'h01;
wire CLEAR_SELECT = shortAlinedAddr == 5'h02;
wire TOKEN_SELECT = shortAlinedAddr == 5'h07;
wire BBOX_LEFT_SELECT = shortAlinedAddr == 5'h08;
wire BBOX_RIGHT_SELECT = shortAlinedAddr == 5'h09;
wire BBOX_TOP_SELECT = shortAlinedAddr == 5'h0A;
wire BBOX_BOTTOM_SELECT = shortAlinedAddr == 5'h0B;
wire FIFO_BASE_L_SELECT = shortAlinedAddr == 5'h10;
wire FIFO_BASE_H_SELECT = shortAlinedAddr == 5'h11;
wire FIFO_END_L_SELECT = shortAlinedAddr == 5'h12;
wire FIFO_END_H_SELECT = shortAlinedAddr == 5'h13;
wire FIFO_HWM_L_SELECT = shortAlinedAddr == 5'h14;
wire FIFO_HWM_H_SELECT = shortAlinedAddr == 5'h15;
wire FIFO_LWM_L_SELECT = shortAlinedAddr == 5'h16;
wire FIFO_LWM_H_SELECT = shortAlinedAddr == 5'h17;
wire FIFO_WRD_L_SELECT = shortAlinedAddr == 5'h18;
wire FIFO_WRD_H_SELECT = shortAlinedAddr == 5'h19;
wire FIFO_WRP_L_SELECT = shortAlinedAddr == 5'h1A;
wire FIFO_WRP_H_SELECT = shortAlinedAddr == 5'h1B;
wire FIFO_RDP_L_SELECT = shortAlinedAddr == 5'h1C;
wire FIFO_RDP_H_SELECT = shortAlinedAddr == 5'h1D;
wire FIFO_BP_L_SELECT = shortAlinedAddr == 5'h1E;
wire FIFO_BP_H_SELECT = shortAlinedAddr == 5'h1F;

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
assign ClearBreakpoint = (CPUWrite & CR_SELECT & CPUWriteData[1]);
assign ClearFIFOOverflow = (CPUWrite & CLEAR_SELECT & CPUWriteData[0]);
assign ClearFIFOUnderflow = (CPUWrite & CLEAR_SELECT & CPUWriteData[1]);

always @ (posedge clk) begin
    if(~resetn) begin
        EnBP <= 0;
        EnGPLink <= 0;
        EnFIFOUnderflow <= 0;
        EnFIFOOverflow <= 0;
        CpIRQEn <= 0;
        EnGPFIFO <= 0;
    end else begin
        if(CPUWrite & CR_SELECT) begin
            EnBP <= CPUWriteData[5];
            EnGPLink <= CPUWriteData[4];
            EnFIFOUnderflow <= CPUWriteData[3];
            EnFIFOOverflow <= CPUWriteData[2];
            CpIRQEn <= CPUWriteData[1];
            EnGPFIFO <= CPUWriteData[0];
        end
    end
end

// Read Encoder
assign CPUReadData = CPURead ?
                                SR_SELECT ? StatusRegister :
                                CR_SELECT ? ControlRegister :
                                TOKEN_SELECT ? 0 :
                                BBOX_LEFT_SELECT ? BBoxLeft :
                                BBOX_RIGHT_SELECT ? BBoxRight :
                                BBOX_TOP_SELECT ? BBoxTop :
                                BBOX_BOTTOM_SELECT ? BBoxBottom :
                                FIFO_BASE_L_SELECT ? FIFOBase[15:0] :
                                FIFO_BASE_H_SELECT ? FIFOBase[31:16] :
                                FIFO_END_L_SELECT ? FIFOEnd[15:0] :
                                FIFO_END_H_SELECT ? FIFOEnd[31:16] :
                                FIFO_HWM_L_SELECT ? FIFOHighWatermark[15:0] :
                                FIFO_HWM_H_SELECT ? FIFOHighWatermark[31:16] :
                                FIFO_LWM_L_SELECT ? FIFOLowWatermark[15:0] :
                                FIFO_LWM_H_SELECT ? FIFOLowWatermark[31:16] :
                                FIFO_WRD_L_SELECT ? FIFORWDistance[15:0] :
                                FIFO_WRD_H_SELECT ? FIFORWDistance[31:16] :
                                FIFO_WRP_L_SELECT ? FIFOWritePointer[15:0] :
                                FIFO_WRP_H_SELECT ? FIFOWritePointer[31:16] :
                                FIFO_RDP_L_SELECT ? FIFOReadPointer[15:0] :
                                FIFO_RDP_H_SELECT ? FIFOReadPointer[31:16] :
                                FIFO_BP_L_SELECT ? FIFOBreakpoint[15:0] :
                                FIFO_BP_H_SELECT ? FIFOBreakpoint[31:16] : 0
                                : 0;

// Write Handler Non Ctl/Reset
always @ (posedge clk) begin
    if(CPUWrite) begin
        if(FIFO_BASE_L_SELECT) FIFOBase[15:0] <= CPUWriteData;
        if(FIFO_BASE_H_SELECT) FIFOBase[31:16] <= CPUWriteData;

        if(FIFO_END_L_SELECT) FIFOEnd[15:0] <= CPUWriteData;
        if(FIFO_END_H_SELECT) FIFOEnd[31:16] <= CPUWriteData;

        if(FIFO_HWM_L_SELECT) FIFOHighWatermark[15:0] <= CPUWriteData;
        if(FIFO_HWM_H_SELECT) FIFOHighWatermark[31:16] <= CPUWriteData;

        if(FIFO_LWM_L_SELECT) FIFOLowWatermark[15:0] <= CPUWriteData;
        if(FIFO_LWM_H_SELECT) FIFOLowWatermark[31:16] <= CPUWriteData;

        if(FIFO_BP_L_SELECT) FIFOBreakpoint[15:0] <= CPUWriteData;
        if(FIFO_BP_H_SELECT) FIFOBreakpoint[31:16] <= CPUWriteData;
    end
end

endmodule
