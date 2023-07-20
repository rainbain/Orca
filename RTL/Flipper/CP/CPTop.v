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

reg[31:0] masterhand;

assign CPUReadData = CPURead ? (CPUAddress == 1) ? masterhand : 0 : 0;

always @ (posedge clk) begin
    if(CPUWrite & (CPUAddress == 1)) begin
        masterhand <= CPUWriteData;
    end
end

endmodule
