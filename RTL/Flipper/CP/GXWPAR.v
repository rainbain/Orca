// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * GXWPAR
 * rainbain
 * 8/30/2023
 * Orca Emulator
 *
 * Write gather pipeline in the gamecube.
 * Write data of any size, and it will be stored back in memory at the FIFO.
*/

module GXWPAR(
    //
    // Top Level
    //

    input wire clk, input wire resetn,

    //
    // CPU Bus
    //

    input wire CPUWrite,
    input wire[31:0] CPUWriteData,
    input wire[3:0] CPUStrobe, output wire CPUWaitRequest,

    //
    // AXI Host Interface
    //

    // Address Write Port
    output wire[48:0] awaddr_a, output wire[7:0] awlen_a,output wire[2:0] awsize_a,
    output wire[1:0] awburst_a, output wire awvalid_a, input wire awready_a,

    // Write Data Channel
    output wire[127:0] wdata_a, output wire[15:0] wstrb_a, output wire wlast_a,
    output wire wvalid_a, input wire wready_a,

    // Write Responce Channel
    input wire[1:0] bresp_a, input wire bvalid_a, output wire bready_a,

    //
    // CP Registers
    //

    input wire[31:0] FIFOBase, input wire[31:0] FIFOEnd,
    input wire[31:0] FIFOHighWatermark, output wire[31:0] FIFOWritePointer, input wire[31:0] FIFOAXIBase,
    output wire[15:0] FIFOErrors, input wire FIFONewBase
);

// Resets whenever the device is reset or the fifo base moves.
wire fifoBaseResetN = resetn & (~FIFONewBase);

/*
 * Physical FIFO
*/

wire fifoWrite;
wire[7:0] fifoWriteData;
wire fifoRead;
wire[127:0] fifoReadData;

wire fifoOverflow;

wire[4:0] test;

GXWPARFIFO fifo(
    .clk(clk), .resetn(fifoBaseResetN),

    .write(fifoWrite), .writeData(fifoWriteData),

    .read(fifoRead), .readData(fifoReadData),

    .overflow(fifoOverflow), .test(test)
);

/*
 * FIFO Packer
*/
wire       packerWrite;
wire[31:0] packerWriteData;
wire[3:0]  packerWriteStrobe;
wire packerNotReady = packerStrobe > 0;

reg[31:0] packerData;
reg[3:0]  packerStrobe;

always @ (posedge clk) begin
    if(~resetn) begin
        packerStrobe <= 0;
    end else begin
        if(packerNotReady) begin
            if(~fifoOverflow) begin
                packerStrobe <= packerStrobe >> 1;
                packerData <= packerData >> 8;
            end
        end else if(packerWrite) begin
            packerData <= packerWriteData;
            packerStrobe <= packerWriteStrobe;
        end
    end
end

assign fifoWrite = packerNotReady;
assign fifoWriteData = packerData[7:0];

assign packerWrite = CPUWrite;
assign packerWriteData = CPUWriteData;
assign packerWriteStrobe = CPUStrobe;
assign CPUWaitRequest = CPUWrite & packerNotReady;


/*
 * AXI Master
*/
wire axiWrite;
wire axiNext;
wire[127:0] axiData;
wire[48:0] axiAddr;
wire axiRequestSize;
wire axiReady;
wire[1:0] axiBerror;

GXWPARMaster axiMaster(
    .clk(clk), .resetn(resetn),
    
    .awaddr_a(awaddr_a), .awlen_a(awlen_a), .awsize_a(awsize_a),
    .awburst_a(awburst_a), .awvalid_a(awvalid_a), .awready_a(awready_a),
    .wdata_a(wdata_a), .wstrb_a(wstrb_a), .wlast_a(wlast_a),
    .wvalid_a(wvalid_a), .wready_a(wready_a),
    .bresp_a(bresp_a), .bvalid_a(bvalid_a), .bready_a(bready_a),
    
    .write(axiWrite), .next(axiNext), .data(axiData), .addr(axiAddr), .requestSize(axiRequestSize),
    .ready(axiReady), .berror(axiBerror)
);

/*
 * AXI FIFO Connection
*/

assign axiWrite = fifoOverflow;
assign fifoRead = axiNext;
assign axiData = fifoReadData;
assign axiRequestSize = ~alinedBarrier;

assign FIFOErrors = {14'd0, axiBerror};

/*
 * Addressing
*/

reg[27:0] FIFOCounter;
wire[27:0] FIFOEndAddr = {FIFOEnd[31:5], 1'd1};
wire[27:0] FIFOStartAddr = FIFOBase[31:4];
assign FIFOWritePointer = {FIFOCounter, 4'd0};

assign axiAddr = {FIFOAXIBase[16:0], FIFOCounter, 4'd0};

wire alinedBarrier = FIFOCounter[1] & FIFOCounter[2] & FIFOCounter[3] & FIFOCounter[4] & FIFOCounter[6] & FIFOCounter[7]; 

wire fifoRollover = FIFOCounter >= FIFOEndAddr;

always @ (posedge clk) begin
    if(~fifoBaseResetN) begin
        FIFOCounter <= FIFOStartAddr;
    end else begin
        if(axiWrite & axiReady) begin
            if(fifoRollover) begin
                FIFOCounter <= FIFOStartAddr;
            end else begin
                FIFOCounter <= FIFOCounter + (axiRequestSize ? 2 : 1);
            end
        end
    end
end

endmodule