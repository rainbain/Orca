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
    input wire[127:0] rdatam_a, output wire rreadym_a, input wire rlastm_a, input wire[1:0] rrespm_a, input wire rvalidm_a,

    //
    // AXI Inerface B (Lite, from host)
    //

    // Read Address Channel
    input wire[31:0] araddr_b, input wire arvalid_b, output wire arready_b,

    // Read Data Channel
    output wire[31:0] rdata_b, output wire[1:0] rresp_b, output wire rvalid_b, input wire rready_b,

    // Write Address Channel
    input wire[31:0] awaddr_b, input wire awvalid_b, output wire awready_b,

    // Write Data Channel
    input wire[31:0] wdata_b, input wire[3:0] wstrb_b,

    // Write Responce Channel
    output wire[1:0] bresp_b, output wire bvalid_b, input wire bready_b
);

/*
 * CPU AXI Lite Interface
*/

// AXI Interface
wire CPURead;
wire CPUWrite;
wire[15:0] CPUFullAddress;
wire[31:0] CPUReadData;
wire[31:0] CPUWriteData;

CPUIFace cpuIFace(
    .clk(clk), .resetn(resetn),

    .araddr_b(araddr_b), .arvalid_b(arvalid_b), .arready_b(arready_b),
    .rdata_b(rdata_b), .rresp_b(rresp_b), .rvalid_b(rvalid_b), .rready_b(rready_b),
    .awaddr_b(awaddr_b), .awvalid_b(awvalid_b), .awready_b(awready_b),
    .wdata_b(wdata_b), .wstrb_b(wstrb_b),
    .bresp_b(bresp_b), .bvalid_b(bvalid_b), .bready_b(bready_b),

    .CPURead(CPURead), .CPUWrite(CPUWrite), .CPUAddress(CPUFullAddress),
    .CPUReadData(CPUReadData), .CPUWriteData(CPUWriteData)
);

// Addressing
wire[11:0] CPUAddress = CPUFullAddress[11:0];
wire[3:0] CPUSelector = CPUFullAddress[15:12];

wire CPU_CP_SELECT = CPUSelector == 0;

// Read Ports
wire[31:0] CPCpuReadData;

assign CPUReadData = CPCpuReadData;


/*
 * CP (Command Processor)
*/

CPTop cp(
    .clk(clk), .resetn(resetn),

    .CPURead(CPURead & CPU_CP_SELECT), .CPUWrite(CPUWrite & CPU_CP_SELECT), .CPUAddress(CPUAddress),
    .CPUReadData(CPCpuReadData), .CPUWriteData(CPUWriteData)
);

endmodule
