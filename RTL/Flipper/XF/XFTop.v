// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * XF
 * rainbain
 * 7/4/2023
 * Orca Emulator
 *
 * XF's Top Level Logic
*/

module XFTop(
    //
    // Top Level
    //

    input wire clk, input wire resetn,

    //
    // CP Bus
    //

    input wire[15:0] CPAddr, input wire CPWrite,
    input wire[31:0] CPWriteData
);

/*
 * CP Address Decoder
*/

wire[3:0] xfAddrSelect = CPAddr[15:12];
wire[11:0] xfAddr = CPAddr[11:0];

wire CPWriteMatrix = (xfAddrSelect == 0) & CPWrite;
wire CPWriteControl = (xfAddrSelect == 1) & CPWrite;


/*
 * Transform Unit
*/

TransformUnit transformUnit(
    .clk(clk), .resetn(resetn),

    .CPAddr(xfAddr), .CPWriteData(CPWriteData),
    .CPWriteMatrix(CPWriteMatrix)
);

endmodule