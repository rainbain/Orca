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
    
    output wire irq,


    //
    // CPU Interface
    //

    input wire CPURead, input wire CPUWrite, input wire[11:0] CPUAddress, input wire[3:0] CPUStrobe,
    output wire[31:0] CPUReadData, input wire[31:0] CPUWriteData, output wire CPUWaitRequest,

    input wire CPRegistersSelect, input wire WPARSelect,

    //
    // AXI Host IFace
    //

    // Address Write Port
    output wire[48:0] awaddr_a, output wire[7:0] awlen_a,output wire[2:0] awsize_a,
    output wire[1:0] awburst_a, output wire awvalid_a, input wire awready_a,

    // Write Data Channel
    output wire[127:0] wdata_a, output wire[15:0] wstrb_a, output wire wlast_a,
    output wire wvalid_a, input wire wready_a,

    // Write Responce Channel
    input wire[1:0] bresp_a, input wire bvalid_a, output wire bready_a,

    // Read Address Channel
    output wire[48:0] araddr_a, output wire[7:0] arlen_a, output wire[2:0] arsize_a,
    output wire[1:0] arburst_a, output wire arvalid_a, input wire arready_a,

    // Read Data Channel
    input wire[127:0] rdata_a, input wire[1:0] rresp_a, input wire rlast_a,
    input wire rvalid_a, output wire rready_a
);

/*
 * CP CPU Mapped Registers
*/

wire[15:0] BBoxLeft = 0;
wire[15:0] BBoxRight = 0;
wire[15:0] BBoxTop = 0;
wire[15:0] BBoxBottom = 0;

wire[31:0] FIFOAXIBase;
wire[31:0] FIFOBase;
wire[31:0] FIFOEnd;
wire[31:0] FIFOHighWatermark;
wire[31:0] FIFOLowWatermark;
wire[31:0] FIFORWDistance = 0;
wire[31:0] FIFOWritePointer;
wire[31:0] FIFOReadPointer = 0;
wire[31:0] FIFOBreakpoint;
wire[15:0] FIFOErrors;

wire IntBP;
wire IntFIFOverflow;
wire IntFIFOUnderflow;
wire StatGPIdle = 0;
wire StatGPReadIdle = 0;
wire FIFONewBase;

wire EnBP;
wire EnGPLink;
wire EnFIFOUnderflow;
wire EnFIFOOverflow;
wire CpIRQEn;
wire EnGPFIFO;

CPRegisters CPURegisters(
    .clk(clk), .resetn(resetn), .irq(irq),

    .CPURead(CPURead & CPRegistersSelect), .CPUWrite(CPUWrite & CPRegistersSelect), .CPUAddress(CPUAddress[5:0]),
    .CPUReadData(CPUReadData), .CPUWriteData(CPUWriteData), .CPUStrobe(CPUStrobe),

    .BBoxLeft(BBoxLeft), .BBoxRight(BBoxRight),
    .BBoxTop(BBoxTop), .BBoxBottom(BBoxBottom),

    .FIFOBase(FIFOBase), .FIFOEnd(FIFOEnd),
    .FIFOHighWatermark(FIFOHighWatermark), .FIFOLowWatermark(FIFOLowWatermark),
    .FIFORWDistance(FIFORWDistance), .FIFOWritePointer(FIFOWritePointer), .FIFOReadPointer(FIFOReadPointer),
    .FIFOBreakpoint(FIFOBreakpoint),
    
    .IntBP(IntBP), .IntFIFOverflow(IntFIFOverflow), .IntFIFOUnderflow(IntFIFOUnderflow),
    .StatGPIdle(StatGPIdle), .StatGPReadIdle(StatGPReadIdle),
    .FIFONewBase(FIFONewBase),

    .EnBP(EnBP), .EnGPLink(EnGPLink), .EnFIFOUnderflow(EnFIFOUnderflow), .EnFIFOOverflow(EnFIFOOverflow),
    .CpIRQEn(CpIRQEn), .EnGPFIFO(EnGPFIFO),
    
    
    .FIFOAXIBase(FIFOAXIBase), .FIFOErrors(FIFOErrors)
);

/*
 * GX WPAR
*/

GXWPAR wpar(
    .clk(clk), .resetn(resetn),

    .CPUWrite(CPUWrite & WPARSelect), .CPUWriteData(CPUWriteData),
    .CPUStrobe(CPUStrobe), .CPUWaitRequest(CPUWaitRequest),

    .awaddr_a(awaddr_a), .awlen_a(awlen_a), .awsize_a(awsize_a), .awburst_a(awburst_a),
    .awvalid_a(awvalid_a), .awready_a(awready_a), .wdata_a(wdata_a), .wstrb_a(wstrb_a),
    .wlast_a(wlast_a), .wvalid_a(wvalid_a), .wready_a(wready_a), .bresp_a(bresp_a),
    .bvalid_a(bvalid_a), .bready_a(bready_a),

    .FIFOBase(FIFOBase), .FIFOEnd(FIFOEnd),
    .FIFOHighWatermark(FIFOHighWatermark), .FIFOLowWatermark(FIFOLowWatermark), .FIFOBreakpoint(FIFOBreakpoint),
    .FIFOWritePointer(FIFOWritePointer), .FIFOAXIBase(FIFOAXIBase), .FIFOErrors(FIFOErrors), .FIFONewBase(FIFONewBase),
    
    .IntBP(IntBP), .IntFIFOverflow(IntFIFOverflow), .IntFIFOUnderflow(IntFIFOUnderflow)
);

endmodule
