// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * GXWPARMaster
 * rainbain
 * 9/10/2023
 * Orca Emulator
 *
 * A strange peace of tech.
 * Supports a fixed 128 bit AXI Master Write with 1-2 beats.
*/

module GXWPARMaster(
    //
    // Top Level
    //
    
    input wire clk, input wire resetn,

    //
    // AXI Host Interface
    //

    // Address Write Port
    output reg[48:0] awaddr_a, output wire[7:0] awlen_a,output wire[2:0] awsize_a,
    output wire[1:0] awburst_a, output reg awvalid_a, input wire awready_a,

    // Write Data Channel
    output wire[127:0] wdata_a, output wire[15:0] wstrb_a, output reg wlast_a,
    output reg wvalid_a, input wire wready_a,

    // Write Responce Channel
    input wire[1:0] bresp_a, input wire bvalid_a, output reg bready_a,
    
    
    //
    // Input
    //
    
    input wire write, output wire next, input wire[127:0] data, input wire[48:0] addr, input wire requestSize,
    output wire ready, output reg[1:0] berror
);

/* AXI Is Tuff. A high level explanation.
 * For each of the 3 axi channels there is 3 always blocks.
 * First is address write. Once write is on and ready is active,
 * a address will be written. It then goes back to idle.
 *
 * Second is write data. Once a address is accepted, it begins a write transaction.
 * At the same time next is high. After that, it waits till its accepted.
 * If it is accepted and request counter is at the request size, it exits back to idle.
 *
 * Last the back responce reciver begins its wait once a address is accepted. It will
 * then wait till the back responce from the device is valid. There it will save
 * the back responce if it is != OK.
 *
 * The ready signal is only high once a address has been written, the write system is idle,
 * and the back responce is idle.
*/

assign wstrb_a = 16'hFFFF;
assign awsize_a = 4; // 16 bytes per word
assign awburst_a = 1; // Incramenting burst

reg len;
assign awlen_a[7:1] = 0;
assign awlen_a[0] = len;

// Write Address Generation
reg awaitAddressWrite;
always @ (posedge clk) begin
    if(~resetn) begin
        awvalid_a <= 0;
        awaitAddressWrite <= 0;
    end else begin
        if(write & ready) begin
            awaddr_a <= addr;
            awvalid_a <= 1;
            len <= requestSize;
            awaitAddressWrite <= 1;
        end else begin
            if(awvalid_a & awready_a) begin
                awvalid_a <= 0;
                awaitAddressWrite <= 0;
            end
        end
    end
end

// Write Data Generation
reg inTransaction;
reg requestCounter;

// Load next word when a address is accepted or when the next write is needed.
assign next = (~inTransaction) ? (awvalid_a & awready_a) : (wvalid_a & wready_a & (requestCounter != len));
assign wdata_a = data;

always @ (posedge clk) begin
    if(~resetn) begin
        inTransaction <= 0;
        wvalid_a <= 0;
    end else begin
        if(~inTransaction) begin
            requestCounter <= 0;
            // When a address is accepted begin write
            if(awvalid_a & awready_a) begin
                inTransaction <= 1;
                wvalid_a <= 1;
                
                if(len == 0) wlast_a <= 1;
                else wlast_a <= 0;
            end
        end else begin
            if(wvalid_a & wready_a) begin
                if(requestCounter == len) begin
                    wvalid_a <= 0;
                    inTransaction <= 0;
                end else begin
                    requestCounter <= 1;
                    wlast_a <= 1;
                end
            end
        end
    end
end

// Back Responce Reciver
reg awaitResponce;
always @ (posedge clk) begin
    if(~resetn) begin
        awaitResponce <= 0;
        bready_a <= 0;
        berror <= 0;
    end else begin
        if(awvalid_a & awready_a) begin
            awaitResponce <= 1;
            bready_a <= 1;
        end else begin
            if(bready_a & bvalid_a) begin
                bready_a <= 0;
                if(bresp_a != 0) begin
                    berror <= bresp_a;
                end
                awaitResponce <= 0;
            end
        end
    end
end

assign ready = (~awaitResponce) & (~inTransaction) & (~awaitAddressWrite);

endmodule