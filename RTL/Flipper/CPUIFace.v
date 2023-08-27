// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * CPUIFace
 * rainbain
 * 7/19/2023
 * Orca Emulator
 *
 * Converts AXI Lite to the cpu interface used around the system.
*/

module CPUIFace(
    //
    // Top Level
    //

    input wire clk, input wire resetn,

    //
    // AXI Inerface B (Lite, from host)
    //

    // Read Address Channel
    input wire[31:0] araddr_b, input wire arvalid_b, output reg arready_b,

    // Read Data Channel
    output wire[31:0] rdata_b, output wire[1:0] rresp_b, output reg rvalid_b, input wire rready_b,

    // Write Address Channel
    input wire[31:0] awaddr_b, input wire awvalid_b, output reg awready_b,

    // Write Data Channel
    input wire[31:0] wdata_b, input wire[3:0] wstrb_b, input wire wvalid_b, output reg wready_b,

    // Write Responce Channel
    output wire[1:0] bresp_b, output reg bvalid_b, input wire bready_b,


    //
    // CPU Interface
    //

    output wire CPURead, output reg CPUWrite, output wire[15:0] CPUAddress,
    input wire[31:0] CPUReadData, output wire[31:0] CPUWriteData
);

assign CPUWriteData = wdata_b;
assign CPUAddress = arvalid_b ? araddr_b[15:0] : awaddr_b[15:0];

reg memoryReadResponce;

// awready_b generation
reg aw_en;
always @ (posedge clk) begin
    if(~resetn) begin
        aw_en <= 1;
        awready_b <= 0;
    end else begin
        if(~awready_b && awvalid_b && wvalid_b && aw_en) begin
            awready_b <= 1;
            aw_en <= 0;
        end else if(bready_b & bvalid_b) begin
            awready_b <= 0;
            aw_en <= 1;
        end else begin
            awready_b <= 0;
        end
    end
end

// wready_b gen
always @ (posedge clk) begin
    if(~resetn) begin
        wready_b <= 0;
        CPUWrite <= 0;
    end else begin
        if(~wready_b && wvalid_b & awvalid_b & aw_en & ~CPURead) begin
            wready_b <= 1;
            CPUWrite <= 1;
        end else begin
            wready_b <= 0;
            CPUWrite <= 0;
        end
    end
end

// bresp_b gen 
assign bresp_b = 0;
always @ (posedge clk) begin
    if(~resetn) begin
        bvalid_b <= 0;
    end
    if(awready_b & awvalid_b & ~bvalid_b & wready_b & wvalid_b ) begin
        bvalid_b <= 1;
    end else begin
        bvalid_b <= 0;
    end
end

// arready gen
always @ (posedge clk) begin
    if(~resetn) begin
        arready_b <= 0;
    end else begin
        if(~arready_b && arvalid_b) begin
            arready_b <= 1;
        end else begin
            arready_b <= 0;
        end
    end
end

// rvalid gen
assign rresp_b = 0;
assign CPURead = arready_b && arvalid_b & ~rvalid_b & ~CPUWrite;
assign rdata_b = CPUReadData;

always @ (posedge clk) begin
    if(~resetn) begin
        rvalid_b <= 0;
    end else begin
        if(CPURead) begin
            rvalid_b <= 1;
        end else if(rvalid_b & rready_b) begin
            rvalid_b <= 0;
        end
    end
end

endmodule