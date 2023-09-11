// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * GXWPARFIFO
 * rainbain
 * 8/30/2023
 * Orca Emulator
 *
 * A FPGA logic implmented fifo due to its small size.
 * Not really that small, its got weird wiring
 * Not really a FIFO too.
*/

module GXWPARFIFO(
    // Top level
    input wire clk, input wire resetn,

    // Write
    input wire write, input wire[7:0] writeData,

    // Read
    input wire read, output reg[127:0] readData,

    // Control
    output reg overflow, output wire[4:0] test

);
reg[4:0] writeHead;
reg readHead;

assign test = writeHead;

wire[4:0] readHeadBig = {readHead, 4'd0};

wire[4:0] readAddress1 =  {readHead, 4'b0000};
wire[4:0] readAddress2 =  {readHead, 4'b0001};
wire[4:0] readAddress3 =  {readHead, 4'b0010};
wire[4:0] readAddress4 =  {readHead, 4'b0011};
wire[4:0] readAddress5 =  {readHead, 4'b0100};
wire[4:0] readAddress6 =  {readHead, 4'b0101};
wire[4:0] readAddress7 =  {readHead, 4'b0110};
wire[4:0] readAddress8 =  {readHead, 4'b0111};
wire[4:0] readAddress9 =  {readHead, 4'b1000};
wire[4:0] readAddress10 = {readHead, 4'b1001};
wire[4:0] readAddress11 = {readHead, 4'b1010};
wire[4:0] readAddress12 = {readHead, 4'b1011};
wire[4:0] readAddress13 = {readHead, 4'b1100};
wire[4:0] readAddress14 = {readHead, 4'b1101};
wire[4:0] readAddress15 = {readHead, 4'b1110};
wire[4:0] readAddress16 = {readHead, 4'b1111};

reg[7:0] fifo[0:31];

always @ (posedge clk) begin
    if(~resetn) begin
        writeHead <= 0;
        readHead <= 0;
        overflow <= 0;
    end else begin
        // We dont need simultainiouse read write, why support it
        if(read & overflow) begin
            readHead <= ~readHead;
            readData[7:0]     <= fifo[readAddress1];
            readData[15:8]    <= fifo[readAddress2];
            readData[23:16]   <= fifo[readAddress3];
            readData[31:24]   <= fifo[readAddress4];
            readData[39:32]   <= fifo[readAddress5];
            readData[47:40]   <= fifo[readAddress6];
            readData[55:48]   <= fifo[readAddress7];
            readData[63:56]   <= fifo[readAddress8];
            readData[71:64]   <= fifo[readAddress9];
            readData[79:72]   <= fifo[readAddress10];
            readData[87:80]   <= fifo[readAddress11];
            readData[95:88]   <= fifo[readAddress12];
            readData[103:96]  <= fifo[readAddress13];
            readData[111:104] <= fifo[readAddress14];
            readData[119:112] <= fifo[readAddress15];
            readData[127:120] <= fifo[readAddress16];

            if(readHead == 1)overflow <= 0; // Only cleared once the whole contence is read.
        end else if(write & ~overflow) begin
            writeHead <= writeHead + 1;
            fifo[writeHead] <= writeData;
            
            if(writeHead == 31)overflow <= 1;
        end
    end
end


endmodule