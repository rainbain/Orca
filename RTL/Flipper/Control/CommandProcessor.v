// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * CommandProcessor
 * rainbain
 * 7/5/2023
 * Orca Emulator
 *
 * Processes commands from the FIFO and feeds them into surounding GC hardware.
*/

module CommandProcessor(
    //
    // Top Level
    //

    input wire clk, input wire resetn,

    //
    // GX FIFO Data
    //

    output wire GXFIFORead, input wire GXFIFOValid,
    input wire[31:0] GXFIFOData,

    //
    // CP Control Bus
    //

    output wire[15:0] CPAddr, output wire[31:0] CPData,
    output reg CPXFWrite
);

/*
 * Command Input System
*/

reg CmdInRead;
wire CmdInValid;
reg[2:0] CmdInBytes;
wire[31:0] CmdInData;

CommandDeserializer commandDeserializer(
    .clk(clk), .resetn(resetn),

    .GXFIFORead(GXFIFORead), .GXFIFOValid(GXFIFOValid),
    .GXFIFOData(GXFIFOData),

    .CPRead(CmdInRead), .CPValid(CmdInValid), .CPBytes(CmdInBytes),
    .CPData(CmdInData)
);

wire[31:0] CmdInAs32 = CmdInData;
wire[15:0] CmdInAs16 = CmdInData[31:16];
wire[7:0] CmdInAs8 = CmdInData[31:24];

/*
 * XF Transaction Counter
*/

reg[15:0] cpStartAddress;

reg clearTransactionCounter;
reg transactionCounterNext;

reg[3:0] transactionSize;
reg[3:0] transactionItor;

wire[15:0] longTransactionItor;
assign longTransactionItor[15:4] = 0; 
assign longTransactionItor[3:0] = transactionItor;

assign CPAddr = cpStartAddress + longTransactionItor;

wire transactionCounterTerminate = transactionItor >= transactionSize;

always @ (posedge clk) begin
    if(clearTransactionCounter) begin
        transactionItor <= 0;
    end else begin
        if(transactionCounterNext)transactionItor <= transactionItor + 1;
    end
end

/*
 * Loaders
*/

assign CPData = CmdInAs32;

reg loadTransactionSize;
reg loadCPStartAddress;

always @ (posedge clk) begin
    if(loadTransactionSize) transactionSize <= CmdInAs16[3:0];
    if(loadCPStartAddress)cpStartAddress <= CmdInAs16;
end

/*
 * OPCODE Decoder
*/

wire OPCODE_XF_LOAD = CmdInAs8 == 8'h10;


/*
 * Command State System
*/

parameter CP_STATE_BITS = 4;

parameter [CP_STATE_BITS-1:0]
    CP_INSTRUCT_GET    = 0,
    CP_INSTRUCT_DECODE = 1,
    CP_XF_LOAD_SZ      = 2,
    CP_XF_LOAD_ADDR    = 3,
    CP_XF_WRITE        = 4;

reg[CP_STATE_BITS-1:0] state;
reg[CP_STATE_BITS-1:0] nextState;

always @* begin
    // Defaults
    nextState = state;

    CPXFWrite = 0;
    CmdInRead = 0;
    CmdInBytes = 0;

    loadTransactionSize = 0;
    loadCPStartAddress = 0;

    clearTransactionCounter = 0;
    transactionCounterNext = 0;

    case(state)
        CP_INSTRUCT_GET: begin
            if(CmdInValid) begin
                if(OPCODE_XF_LOAD) begin
                     nextState = CP_XF_LOAD_SZ;
                end
            end else begin
                CmdInBytes = 1;
                CmdInRead = 1;
            end
        end

        CP_XF_LOAD_SZ: begin
            if(CmdInValid) begin
                loadTransactionSize = 1;
                nextState = CP_XF_LOAD_ADDR;
            end else begin
                CmdInBytes = 2;
                CmdInRead = 1;
            end
        end

        CP_XF_LOAD_ADDR: begin
            if(CmdInValid) begin
                loadCPStartAddress = 1;
                nextState = CP_XF_WRITE;
                clearTransactionCounter = 0;
            end else begin
                CmdInBytes = 2;
                CmdInRead = 1;
            end
        end

        CP_XF_WRITE: begin
            if(CmdInValid) begin
                transactionCounterNext = 1;
                CPXFWrite = 1;
                if(transactionCounterTerminate)nextState = CP_INSTRUCT_GET;
            end else begin
                CmdInBytes = 4;
                CmdInRead = 1;
            end
        end

        default:begin
        end
    endcase
end

always @ (posedge clk) begin
    if(~resetn) begin
        state <= CP_INSTRUCT_GET;
    end else begin
        state <= nextState;
    end
end

endmodule
