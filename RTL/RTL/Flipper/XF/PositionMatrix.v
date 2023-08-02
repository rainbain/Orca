// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * PositionMatrix
 * rainbain
 * 7/9/2023
 * Orca Emulator
 *
 * Gamecube's Position Matrix Memory
*/

module PositionMatrix (
    //
    // Top Level
    //

    input wire clk, input wire resetn,

    //
    // CP Bus
    //

    input wire[8:0] CPAddr, input wire CPWrite,
    input wire[31:0] CPWriteData,

    //
    // PortA
    //

    input wire[6:0] posmatAAddr, input wire posmatAEnable,
    output wire[127:0] posmatAData, output reg posmatAValid
);

/*
 * The Memorys
*/

reg[31:0] posmatMemoryA[0:64];
reg[31:0] posmatMemoryB[0:64];
reg[31:0] posmatMemoryC[0:64];
reg[31:0] posmatMemoryD[0:64];

wire[6:0] CPPosmatAddr = CPAddr[8:2];
wire[1:0] CPPosmatSelect = CPAddr[1:0];


always @ (posedge clk) begin
    if(CPWrite) begin
        if(CPPosmatSelect == 0)posmatMemoryA[CPPosmatAddr] <= CPWriteData;
        if(CPPosmatSelect == 1)posmatMemoryB[CPPosmatAddr] <= CPWriteData;
        if(CPPosmatSelect == 2)posmatMemoryC[CPPosmatAddr] <= CPWriteData;
        if(CPPosmatSelect == 3)posmatMemoryD[CPPosmatAddr] <= CPWriteData;
        $display("GX -> XF -> Position Matrix: Addr: %03h, Write Data: %08h",CPAddr, CPWriteData);
    end else begin
        if(posmatAEnable) begin
            posmatAData[127:96] <= posmatMemoryA[posmatAAddr];
            posmatAData[95:64] <= posmatMemoryB[posmatAAddr];
            posmatAData[63:32] <= posmatMemoryC[posmatAAddr];
            posmatAData[31:0] <= posmatMemoryD[posmatAAddr];
        end
    end

    posmatAValid <= (~CPWrite & posmatAEnable);
end

endmodule