// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * CommandDeserializer
 * rainbain
 * 7/4/2023
 * Orca Emulator
 *
 * Breaks up 32 bit words from the fifo into 8, 16, 24, 32 bytes.
 * To syncronize with the real gamecube it always operates within 2 cycles.
 * To do this 1 cycle commits data the other refreshes the read window.
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
 * Deserializer
*/

reg[31:0] deserializerWindow;
wire[31:0] nextWindow = (deserializerWindow >> (CPBytes << 3)) | (GXFIFOData << ((4-CPBytes) << 3));

reg[3:0] dataValidPipeline;

reg[1:0] byteCounter;
wire[2:0] byteAdder = byteCounter + CPBytes;
wire[1:0] nextByteCounter = byteAdder[1:0];
wire byteCounterNextWord = byteAdder[2];

reg deserializerLoad;
always @ (posedge clk) begin
    if(~resetn) begin
        byteCounter <= 0;
    end else begin
        if(deserializerLoad) begin
            deserializerWindow <= nextWindow;
            byteCounter <= nextByteCounter;
        end

        if(deserializerLoad) begin
            dataValidPipeline[0] <= dataValidPipeline[1];
            dataValidPipeline[1] <= dataValidPipeline[2];
            dataValidPipeline[2] <= dataValidPipeline[3];
            dataValidPipeline[3] <= 0;
        end else begin
            if(GXFIFOValid)dataValidPipeline <= 4'b1111;
        end
    end
end

/*
 * Output System
*/

reg outputReadEnable;

wire[3:0] pipelineMask;
assign pipelineMask[0] = CPBytes > 0;
assign pipelineMask[1] = CPBytes > 1;
assign pipelineMask[2] = CPBytes > 2;
assign pipelineMask[3] = CPBytes > 3;

wire readDataValid = outputReadEnable & ((pipelineMask & dataValidPipeline) == pipelineMask);

always @ (posedge clk) begin
    CPValid <= outputReadEnable & readDataValid;
    if(outputReadEnable) CPData <= deserializerWindow;
end

/*
 * Control System
*/

localparam
    STATE_IDLE = 0,
    STATE_NEXT = 1;

reg state;
reg nextState;

reg refreshRequested;

always @* begin
    // Defaults
    nextState = state;
    deserializerLoad = 0;
    outputReadEnable = 0;
    GXFIFORead = 0;

    case(state)
        STATE_IDLE: begin
            deserializerLoad = CPRead;
            outputReadEnable = CPRead;
            GXFIFORead = CPRead & byteCounterNextWord;

            if(CPRead) begin
                nextState = STATE_NEXT;
            end
        end
        STATE_NEXT: begin
            if(refreshRequested) begin
                if(CPValid)nextState = STATE_IDLE;
            end else begin
                nextState = STATE_IDLE;
            end
        end
    endcase
end

always @ (posedge clk) begin
    if(~resetn) begin
        refreshRequested <= 0;
    end else begin
        if(refreshRequested) begin
            refreshRequested <= ~CPValid;
        end else begin
            refreshRequested <= GXFIFORead;
        end
    end

    if(~resetn) begin
        state <= STATE_IDLE;
    end else begin
        state <= nextState;
    end
end

endmodule
