// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * FlipperTop
 * rainbain
 * 7/4/2023
 * Orca Emulator
 *
 * Top level logic for Flipper, the GC/Wii's GPU.
*/

module FlipperTop(
    //
    // Top Level
    //

    input wire clk, input wire resetn,

    //
    // GX FIFO Data
    //

    output wire GXFIFORead, input wire GXFIFOValid,
    input wire[31:0] GXFIFOData
);

/* 
 * Command System
*/

wire[15:0] CPAddr;
wire[31:0] CPWriteData;

wire CPXFWrite;

CommandProcessor CP(
    .clk(clk), .resetn(resetn),

    .GXFIFORead(GXFIFORead), .GXFIFOValid(GXFIFOValid),
    .GXFIFOData(GXFIFOData),

    .CPAddr(CPAddr), .CPData(CPWriteData),
    .CPXFWrite(CPXFWrite)
);


/*
 * XF ((X) TRANSFORM)
*/

XFTop xf(
    .clk(clk), .resetn(resetn),

    .CPAddr(CPAddr), .CPWrite(CPXFWrite),
    .CPWriteData(CPWriteData)
);

endmodule
