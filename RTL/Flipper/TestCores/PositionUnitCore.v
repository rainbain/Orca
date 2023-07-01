// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * RISCVSoc
 * rainbain
 * 6/28/2023
 * Orca Emulator
 *
 * Memory mapped Position Unit for testing its operation in real hardware.
*/

module PositionUnitCore(
	input wire clk, input wire resetn,
	
	input wire[7:0] address, input wire read, input wire write,
	output wire[31:0] readData, input wire[31:0] writeData,
	
	output reg waitrequest
);
/* Input Mapping
 * 00: World Matrix 00
 * 01: World Matrix 01
 * 02: World Matrix 02
 * 03: World Matrix 03
 * 04: World Matrix 10
 * 05: World Matrix 11
 * 06: World Matrix 12
 * 07: World Matrix 13
 * 08: World Matrix 20
 * 09: World Matrix 21
 * 0A: World Matrix 22
 * 0B: World Matrix 23
 * 0C: Projection Matrix 00
 * 0D: Projection Matrix 01
 * 0E: Projection Matrix 10
 * 0F: Projection Matrix 11
 * 10: Projection Matrix 20
 * 11: Projection Matrix 21
 * 12: Input Vertex X
 * 13: Input Vertex Y
 * 14: Input Vertex Z
 *
 * 15: Operation
 *
 * Output Mapping
 * 16: World Position X
 * 17: World Position Y
 * 18: World Position Z
 * 19: Projected Position X
 * 1A: Projected Position Y
 * 1B: Projected Position Z
 * 1C: Projected Position W
*/

reg[31:0] inputData[0:20];
wire[31:0] outputData[0:6];

/*
 * Input Register Acess
*/

wire innerRegesterSelect = address[7:2] < 21;
wire operationRegisterSelect = address[7:2] == 21;
wire outerRegisterSelect = address[7:2] > 21;

assign readData = read ? (outerRegisterSelect ? outputData[address[7:2]-22] : 0) : 0;

always @ (negedge clk) begin
	if(write) begin
		if(innerRegesterSelect) begin
			inputData[address[7:2]] <= writeData;
		end
	end
end

/*
 * Write state system
*/

localparam [1:0] // 3 states are required for Moore
    STATE_IDLE = 2'b00,
    STATE_WAIT = 2'b01, 
    STATE_EXIT = 2'b10;

reg[1:0] state;
reg[1:0] nextState;

reg cycle;
wire ready;
wire datavalid;

always @* begin
	waitrequest = 1;
	nextState = state;
	cycle = 0;

	case(state)
		STATE_IDLE: begin
			if(write & operationRegisterSelect) begin
				cycle = 1;
				waitrequest = 1;
				nextState = STATE_WAIT;
			end else begin
				waitrequest = 0;
			end
			
		end
		STATE_WAIT: begin
			if(datavalid) begin
				waitrequest = 0;
				nextState = STATE_IDLE;
			end
		end
	endcase
end

always @ (posedge clk) begin
	if(~resetn) begin
		state <= STATE_IDLE;
	end else begin
		state <= nextState;
	end
end

/*
 * Position Unit
*/

wire[95:0] vinPosition;
assign vinPosition[95:64] = inputData[18];
assign vinPosition[63:32] = inputData[19];
assign vinPosition[31:0] = inputData[20];

wire[95:0] worldPosition;
assign outputData[0] = worldPosition[95:64];
assign outputData[1] = worldPosition[63:32];
assign outputData[2] = worldPosition[31:0];

wire[127:0] projectedPosition;
assign outputData[3] = projectedPosition[127:96];
assign outputData[4] = projectedPosition[95:64];
assign outputData[5] = projectedPosition[63:32];
assign outputData[6] = projectedPosition[31:0];

wire[6:0] posmatAddr;
wire posmatEnable;
reg[127:0] posmatData;
reg posmatValid;

wire[63:0] xfProjectionA;
assign xfProjectionA[63:32] = inputData[12];
assign xfProjectionA[31:0] = inputData[13];

wire[63:0] xfProjectionB;
assign xfProjectionB[63:32] = inputData[14];
assign xfProjectionB[31:0] = inputData[15];

wire[63:0] xfProjectionC;
assign xfProjectionC[63:32] = inputData[16];
assign xfProjectionC[31:0] = inputData[17];

PositionUnit pu(
	.clk(clk), .resetn(resetn),
	.cycle(cycle), .ready(ready), .dataValid(datavalid), 
	
	.vinPositionMatrix(0),  .vinPosition(vinPosition),
	.worldPosition(worldPosition), .projectedPosition(projectedPosition),
	.posmatAddr(posmatAddr), .posmatEnable(posmatEnable),
	.posmatData(posmatData), .posmatValid(posmatValid),
	
	.xfProjectionA(xfProjectionA),
	.xfProjectionB(xfProjectionB),
	.xfProjectionC(xfProjectionC),
	
	.orthoEnable(0)
);

/*
 * Matrix Memory
*/

wire[127:0] matrixMemory[0:2];
assign matrixMemory[0][127:96] = inputData[0];
assign matrixMemory[0][95:64] = inputData[1];
assign matrixMemory[0][63:32] = inputData[2];
assign matrixMemory[0][31:0] = inputData[3];

assign matrixMemory[1][127:96] = inputData[4];
assign matrixMemory[1][95:64] = inputData[5];
assign matrixMemory[1][63:32] = inputData[6];
assign matrixMemory[1][31:0] = inputData[7];

assign matrixMemory[2][127:96] = inputData[8];
assign matrixMemory[2][95:64] = inputData[9];
assign matrixMemory[2][63:32] = inputData[10];
assign matrixMemory[2][31:0] = inputData[11];



always @ (posedge clk) begin
	if(posmatEnable) begin
	   posmatData <= matrixMemory[posmatAddr];
	end
	
	posmatValid <= posmatEnable;
end

endmodule