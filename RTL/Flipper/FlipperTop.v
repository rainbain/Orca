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
    // AXI Interface A (Full)
    //

    // Read Address Channel
    output wire[31:0] araddrm_a, output wire[1:0] arburstm_a, output wire[3:0] arlenm_a, input wire arreadym_a,
    output wire[2:0] arsizem_a, output wire arvalidm_a,

    // Read Data Channel
    input wire[127:0] rdatam_a, output wire rreadym_a, input wire rlastm_a, input wire[1:0] rrespm_a, input wire rvalidm_a

    //
    // AXI Inerface B (Lite, from host)
    //

    // Read Address Channel
    input wire[31:0] araddr_b, input wire arvalid_b, output wire arready_b,

    // Read Data Channel
    output wire[31:0] rdata_a, output wire rvalid_b, input wire rready_b,

    // Write Address Channel
    input wire[31:0] awaddr_b, input wire awvalid_b, output wire awread_b,

    // Write Data Channel
    input wire[31:0] wdata_b, input wire[3:0] wstrb_b,

    // Write Responce Channel
    output wire[1:0] bresp_b, output wire bvalid_b, input wire bready_b
);

assign araddrm_a = 0;
assign arburstm_a = 1;
assign arlenm_a = 4;
assign arsizem_a = 3;
assign arvalidm_a = 1;

assign rreadym_a = 1;

always @ (posedge clk) begin
    if(rvalidm_a) begin
        $display("Value: %0h", rdatam_a);
    end
end

/*
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

XFTop xf(
    .clk(clk), .resetn(resetn),

    .CPAddr(CPAddr), .CPWrite(CPXFWrite),
    .CPWriteData(CPWriteData)
);
*/
endmodule
