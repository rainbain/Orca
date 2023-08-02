// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * CP
 * rainbain
 * 7/19/2023
 * Orca Emulator
 *
 * Top level logic for the command processor.
*/

module CPTop (
    //
    // Top Level
    //

    input wire clk, input wire resetn,


    //
    // CPU Interface
    //

    input wire CPURead, input wire CPUWrite, input wire[11:0] CPUAddress,
    output wire[31:0] CPUReadData, input wire[31:0] CPUWriteData
);

/*
 * CP CPU Mapped Registers
*/

wire[15:0] BBoxLeft = 0;
wire[15:0] BBoxRight = 0;
wire[15:0] BBoxTop = 0;
wire[15:0] BBoxBottom = 0;

wire[31:0] FIFOBase;
wire[31:0] FIFOEnd;
wire[31:0] FIFOHighWatermark;
wire[31:0] FIFOLowWatermark;
wire[31:0] FIFORWDistance = 0;
wire[31:0] FIFOWritePointer = 0;
wire[31:0] FIFOReadPointer = 0;
wire[31:0] FIFOBreakpoint;

wire IntBP = 0;
wire IntFIFOverflow = 0;
wire IntFIFOUnderflow = 0;
wire StatGPIdle = 0;
wire StatGPReadIdle = 0;

wire EnBP;
wire EnGPLink;
wire EnFIFOUnderflow;
wire EnFIFOOverflow;
wire CpIRQEn;
wire EnGPFIFO;

assign CPUReadData[31:16] = 0;

CPRegisters CPURegisters(
    .clk(clk), .resetn(resetn),

    .CPURead(CPURead), .CPUWrite(CPUWrite), .CPUAddress(CPUAddress[5:0]),
    .CPUReadData(CPUReadData[15:0]), .CPUWriteData(CPUWriteData[15:0]),

    .BBoxLeft(BBoxLeft), .BBoxRight(BBoxRight),
    .BBoxTop(BBoxTop), .BBoxBottom(BBoxBottom),

    .FIFOBase(FIFOBase), .FIFOEnd(FIFOEnd),
    .FIFOHighWatermark(FIFOHighWatermark), .FIFOLowWatermark(FIFOLowWatermark),
    .FIFORWDistance(FIFORWDistance), .FIFOWritePointer(FIFOWritePointer), .FIFOReadPointer(FIFOReadPointer),
    .FIFOBreakpoint(FIFOBreakpoint),
    
    .IntBP(IntBP), .IntFIFOverflow(IntFIFOverflow), .IntFIFOUnderflow(IntFIFOUnderflow),
    .StatGPIdle(StatGPIdle), .StatGPReadIdle(StatGPReadIdle),

    .EnBP(EnBP), .EnGPLink(EnGPLink), .EnFIFOUnderflow(EnFIFOUnderflow), .EnFIFOOverflow(EnFIFOOverflow),
    .CpIRQEn(CpIRQEn), .EnGPFIFO(EnGPFIFO)
);

endmodule
