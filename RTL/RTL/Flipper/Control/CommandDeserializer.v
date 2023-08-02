// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * CommandDeserializer
 * rainbain
 * 7/4/2023
 * Orca Emulator
 *
 * Breaks up 32 bit words from the fifo into 8, 16, 24, 32 bytes.
 * This one took a lot of work to get right.
*/

module CommandDeserializer(
    //
    // Top Level
    //

    input wire clk, input wire resetn,

    
    //
    // GX FIFO
    //

    output reg GXFIFORead, input wire GXFIFOValid,
    input wire[31:0] GXFIFOData,


    //
    // CP Controll
    //

    input wire CPRead, output reg CPValid, input wire[2:0] CPBytes,
    output reg[31:0] CPData
);

/*
 * Input Output FIFO
*/

reg[2:0] readHead;
reg[2:0] writeHead;

reg[7:0] fifoData[0:7];

reg readNext;
wire[31:0] readData;
assign readData[31:24] = fifoData[readHead+0];
assign readData[23:16] = fifoData[readHead+1];
assign readData[15:8]  = fifoData[readHead+2];
assign readData[7:0]   = fifoData[readHead+3];

wire[2:0] fifoSize = writeHead - readHead;

wire fifoNotFull = fifoSize < 5; // Must have at least 5 bytes of room. 4 for the byte, 1 to prevent read head write head interception
wire fifoNotEmpty = fifoSize >= CPBytes; // Size must match or excede requested size.

always @ (posedge clk) begin
    if(~resetn) begin
        readHead <= 0;
        writeHead <= 0;
    end else begin
        if(readNext) begin
            readHead <= readHead + CPBytes;
        end

        if(GXFIFOValid) begin
            writeHead <= writeHead + 4;
            fifoData[writeHead] <= GXFIFOData[31:24];
            fifoData[writeHead+1] <= GXFIFOData[23:16];
            fifoData[writeHead+2] <= GXFIFOData[15:8];
            fifoData[writeHead+3] <= GXFIFOData[7:0];
        end
    end
end

/*
 * Output Buffer
*/
always @ (posedge clk) begin
    if(~resetn) begin
        CPValid <= 0;
    end else begin
        CPValid <= readNext;
        if(readNext) begin
            CPData <= readData;
        end
    end
end

/*
 * Input Request System
*/

reg dataInputRequested;
always @ (posedge clk) begin
    if(~resetn) begin
        dataInputRequested <= 0;
    end else begin
        if(dataInputRequested) begin
            dataInputRequested <= ~GXFIFOValid;
        end else begin
            dataInputRequested <= GXFIFORead;
        end
    end
end


/*
 * State Control
*/

reg state;
reg nextState;

parameter 
    STATE_OUTPUT = 1'b0,
    STATE_INPUT = 1'b1;

always @* begin
    //Defaults
    nextState = state;
    readNext = 0;
    GXFIFORead = 0;

    case(state)
        STATE_OUTPUT: begin
            readNext = CPRead & fifoNotEmpty;
            GXFIFORead = fifoNotFull;
            if(readNext | GXFIFORead) nextState = STATE_INPUT;
        end

        STATE_INPUT: begin
            if(dataInputRequested) begin
                if(GXFIFOValid)nextState = STATE_OUTPUT;
            end else begin
                nextState = STATE_OUTPUT;
            end
        end
    endcase
end

always @ (posedge clk) begin
    if(~resetn) begin
        state <= STATE_OUTPUT;
    end else begin
        state <= nextState;
    end
end

endmodule
