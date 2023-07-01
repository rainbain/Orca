// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * RISCVSoc
 * rainbain
 * 6/29/2023
 * Orca Emulator
 *
 * Memory mapped Normal Unit for testing its operation in real hardware.
*/

`default_nettype none

module NormalUnitCore(
	input wire clk, input wire resetn,
	
	input wire[7:0] address, input wire read, input wire write,
	output wire[31:0] readData, input wire[31:0] writeData,
	
	output reg waitrequest
);

/* Input Mapping
 * 00: Normal Matrix 00
 * 01: Normal Matrix 10
 * 02: Normal Matrix 20
 * 03: Normal Matrix 01
 * 04: Normal Matrix 11
 * 05: Normal Matrix 21
 * 06: Normal Matrix 02
 * 07: Normal Matrix 12
 * 08: Normal Matrix 22
 * 09: Normal X
 * 0A: Normal Y
 * 0B: Normal Z
 * 0C: Tangent X
 * 0D: Tangent Y
 * 0E: Tangent Z
 * 0F: Bisector X
 * 10: Bisector Y
 * 11: Bisector Z
 *
 * 12: Operation
 * 
 * Output Mapping
 * 13: Output Normal X
 * 14: Output Normal Y
 * 15: Output Normal Z
 * 16: Output Tangent X
 * 17: Output Tangent Y
 * 18: Output Tangent Z
 * 19: Output Bisector X
 * 1A: Output Bisector Y
 * 1B: Output Bisector Z 
*/

reg[31:0] inputData[0:17];
wire[31:0] outputData[0:8];

/*
 * Input Register Acess
*/

wire innerRegesterSelect = address[7:2] < 18;
wire operationRegisterSelect = address[7:2] == 18;
wire outerRegisterSelect = address[7:2] > 18;

assign readData = read ? (outerRegisterSelect ? outputData[address[7:2]-19] : 0) : 0;

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
 * Matrix Unit
*/

wire[95:0] vinNormalN;
assign vinNormalN[95:64] = inputData[9];
assign vinNormalN[63:32] = inputData[10];
assign vinNormalN[31:0] = inputData[11];

wire[95:0] vinNormalT;
assign vinNormalT[95:64] = inputData[12];
assign vinNormalT[63:32] = inputData[13];
assign vinNormalT[31:0] = inputData[14];

wire[95:0] vinNormalB;
assign vinNormalB[95:64] = inputData[15];
assign vinNormalB[63:32] = inputData[16];
assign vinNormalB[31:0] = inputData[17];

wire[95:0] outNormalN;
assign outputData[0] = outNormalN[95:64];
assign outputData[1] = outNormalN[63:32];
assign outputData[2] = outNormalN[31:0];

wire[95:0] outNormalT;
assign outputData[3] = outNormalT[95:64];
assign outputData[4] = outNormalT[63:32];
assign outputData[5] = outNormalT[31:0];

wire[95:0] outNormalB;
assign outputData[6] = outNormalB[95:64];
assign outputData[7] = outNormalB[63:32];
assign outputData[8] = outNormalB[31:0];

wire[5:0] nommatAddr;
wire nommatEnable;
reg[95:0] nommatData;
reg nommatValid;

NormalUnit nu(
	.clk(clk), .resetn(resetn),
	.cycle(cycle), .ready(ready), .dataValid(datavalid),
	
	.normalN(vinNormalN), .normalT(vinNormalT), .normalB(vinNormalB),
	.normalMatrixAddr(0), .enableNTB(1),
	
	.outputNormalN(outNormalN), .outputNormalT(outNormalT), .outputNormalB(outNormalB),
	
	.nommatAddr(nommatAddr), .nommatEnable(nommatEnable),
	.nommatData(nommatData), .nommatValid(nommatValid)
);

/*
 * Matrix Memory
*/

wire[95:0] matrixMemory[0:2];
assign matrixMemory[0][95:64] = inputData[0];
assign matrixMemory[0][63:32] = inputData[1];
assign matrixMemory[0][31:0] = inputData[2];

assign matrixMemory[1][95:64] = inputData[3];
assign matrixMemory[1][63:32] = inputData[4];
assign matrixMemory[1][31:0] = inputData[5];

assign matrixMemory[2][95:64] = inputData[6];
assign matrixMemory[2][63:32] = inputData[7];
assign matrixMemory[2][31:0] = inputData[8];

always @ (posedge clk) begin
	if(nommatEnable) begin
		nommatData <= matrixMemory[nommatAddr];
	end
	
	nommatValid <= nommatEnable;
end

endmodule