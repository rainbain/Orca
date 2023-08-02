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
    output reg[31:0] rdata_b, output wire[1:0] rresp_b, output reg rvalid_b, input wire rready_b,

    // Write Address Channel
    input wire[31:0] awaddr_b, input wire awvalid_b, output reg awready_b,

    // Write Data Channel
    input wire[31:0] wdata_b, input wire[3:0] wstrb_b,

    // Write Responce Channel
    output wire[1:0] bresp_b, output reg bvalid_b, input wire bready_b,


    //
    // CPU Interface
    //

    output wire CPURead, output wire CPUWrite, output wire[15:0] CPUAddress,
    input wire[31:0] CPUReadData, output wire[31:0] CPUWriteData
);

parameter [1:0]
    AXI_STATE_IDLE = 0,
    AXI_STATE_READ_REPLY = 1,
    AXI_STATE_WRITE_REPLY = 2;

reg[1:0] state;
reg[1:0] nextState;

assign CPUWriteData = wdata_b;
assign CPUAddress = arvalid_b ? araddr_b[15:0] : awaddr_b[15:0];

// Handshakes
assign CPURead = arvalid_b & arready_b;
assign CPUWrite = awvalid_b & awready_b;

// Reads and Writes never fail
assign rresp_b = 0;
assign bresp_b = 0;

always @ (*) begin
    // Defaults
    nextState = state;
    arready_b = 0;
    awready_b = 0;

    bvalid_b = 0;
    rvalid_b = 0;

    case(state)
        AXI_STATE_IDLE: begin
            // Address Writes Ready
            arready_b = 1;
            awready_b = ~arvalid_b; // Only write ready when not reading

            // Accept Read
            if(arvalid_b) begin
                nextState = AXI_STATE_READ_REPLY;
            end else if(awvalid_b) begin // Accept Write
                nextState = AXI_STATE_WRITE_REPLY;
            end
        end

        AXI_STATE_READ_REPLY: begin
            rvalid_b = 1;
            if(rready_b)
                nextState = AXI_STATE_IDLE;
        end

        AXI_STATE_WRITE_REPLY: begin
            bvalid_b = 1;
            if(bready_b)
                nextState = AXI_STATE_IDLE;
        end
        default: begin
        end
    endcase
end

// State Register
always @ (posedge clk) begin
    if(~resetn) begin
        state <= AXI_STATE_IDLE;
    end else begin
        state <= nextState;
    end
end

// Read Data Loader
always @ (posedge clk) begin
    if(CPURead) begin
        rdata_b <= CPUReadData;
    end
end

endmodule