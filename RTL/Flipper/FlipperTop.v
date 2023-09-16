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
    // AXI Interface A (Full, is host)
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
    input wire rvalid_a, output wire rready_a,

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
    input wire[31:0] wdata_b, input wire[3:0] wstrb_b, input wire wvalid_b, output wire wready_b,

    // Write Responce Channel
    output wire[1:0] bresp_b, output wire bvalid_b, input wire bready_b,
    
    //
    // Interrupts
    //
    
    output wire irq_a
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
wire CPUWaitRequest;
wire[3:0] CPUStrobe;

CPUIFace cpuIFace(
    .clk(clk), .resetn(resetn),

    .araddr_b(araddr_b), .arvalid_b(arvalid_b), .arready_b(arready_b),
    .rdata_b(rdata_b), .rresp_b(rresp_b), .rvalid_b(rvalid_b), .rready_b(rready_b),
    .awaddr_b(awaddr_b), .awvalid_b(awvalid_b), .awready_b(awready_b),
    .wdata_b(wdata_b), .wstrb_b(wstrb_b), .wvalid_b(wvalid_b), .wready_b(wready_b),
    .bresp_b(bresp_b), .bvalid_b(bvalid_b), .bready_b(bready_b),

    .CPURead(CPURead), .CPUWrite(CPUWrite), .CPUAddress(CPUFullAddress),
    .CPUReadData(CPUReadData), .CPUWriteData(CPUWriteData), .CPUWaitRequest(CPUWaitRequest), .CPUStrobe(CPUStrobe)
);

// Addressing
wire[11:0] CPUAddress = CPUFullAddress[11:0];
wire[3:0] CPUSelector = CPUFullAddress[15:12];

wire CPU_CP_SELECT = CPUSelector == 0;
wire CPU_WPAR_SELECT = CPUSelector == 8;

// Read Ports
wire[31:0] CPCpuReadData;

assign CPUReadData = CPCpuReadData;

// Wait Request Ports
wire CPWaitRequest;

assign CPUWaitRequest = CPWaitRequest;

// IRQ Ports
wire cpIrq;

assign irq_a = cpIrq;


/*
 * CP (Command Processor)
*/

CPTop cp(
    .clk(clk), .resetn(resetn), .irq(cpIrq),

    .CPURead(CPURead), .CPUWrite(CPUWrite), .CPUAddress(CPUAddress), .CPUStrobe(CPUStrobe),
    .CPUReadData(CPCpuReadData), .CPUWriteData(CPUWriteData), .CPUWaitRequest(CPWaitRequest),
    .CPRegistersSelect(CPU_CP_SELECT), .WPARSelect(CPU_WPAR_SELECT),

    .awaddr_a(awaddr_a), .awlen_a(awlen_a), .awsize_a(awsize_a),
    .awburst_a(awburst_a), .awvalid_a(awvalid_a), .awready_a(awready_a),
    .wdata_a(wdata_a), .wstrb_a(wstrb_a), .wlast_a(wlast_a),
    .wvalid_a(wvalid_a), .wready_a(wready_a),
    .bresp_a(bresp_a), .bvalid_a(bvalid_a), .bready_a(bready_a),
    .araddr_a(araddr_a), .arlen_a(arlen_a), .arsize_a(arsize_a),
    .arburst_a(arburst_a), .arvalid_a(arvalid_a), .arready_a(arready_a),
    .rdata_a(rdata_a), .rresp_a(rresp_a), .rlast_a(rlast_a),
    .rvalid_a(rvalid_a), .rready_a(rready_a)
);

endmodule
