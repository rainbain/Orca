// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * CommandProcessor
 * rainbain
 * 7/5/2023
 * Orca Emulator
 *
 * Processes commands from the FIFO and feeds them into surounding GC hardware.
*/

module CommandProcessor(
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
 * Command Input System
*/

wire CmdInRead;
wire CmdInValid;
wire[2:0] CmdInBytes;
wire[31:0] CmdInData;

CommandDeserializer commandDeserializer(
    .clk(clk), .resetn(resetn),

    .GXFIFORead(GXFIFORead), .GXFIFOValid(GXFIFOValid),
    .GXFIFOData(GXFIFOData),

    .CPRead(CmdInRead), .CPValid(CmdInValid), .CPBytes(CmdInBytes),
    .CPData(CmdInData)
);

assign CmdInRead = 1;
assign CmdInBytes = 1;

always @ (posedge clk) begin
    if(CmdInValid) $display("FIFO Out: %08h", CmdInData[31:0]);
end

endmodule
