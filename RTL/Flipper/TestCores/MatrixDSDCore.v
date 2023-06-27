// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * RISCVSoc
 * rainbain
 * 6/26/2023
 * Orca Emulator
 *
 * Memory mapped Matrix DSP for testing its operation in real hardware.
*/

module MatrixDSPCore(
	input wire clk, input wire resetn,
	
	input wire[7:0] address, input wire read, input wire write,
	output wire[31:0] readData, input wire[31:0] writeData,
	
	output reg waitrequest
);

reg[31:0] inputData[0:8];
wire[31:0] outputData[0:2];

/*
 * Input Register Acess
*/

wire innerRegesterSelect = address[7:2] < 9;
wire operationRegisterSelect = address[7:2] == 9;
wire outerRegisterSelect = address[7:2] > 9;

assign readData = read ? (outerRegisterSelect ? outputData[address[7:2]-10] : 0) : 0;

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
			if(ready) begin
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

reg cycle;
wire ready;

wire[127:0] inputVector;
assign inputVector[127:96] = inputData[0];
assign inputVector[95:64] = inputData[1];
assign inputVector[63:32] = inputData[2];
assign inputVector[31:0] = 0;

wire[127:0] matrixA;
assign matrixA[127:96] = inputData[3];
assign matrixA[95:64] = inputData[4];
assign matrixA[63:32] = 0;
assign matrixA[31:0] = 0;

wire[127:0] matrixB;
assign matrixB[127:96] = inputData[5];
assign matrixB[95:64] = inputData[6];
assign matrixB[63:32] = 0;
assign matrixB[31:0] = 0;

wire[127:0] matrixC;
assign matrixC[127:96] = inputData[7];
assign matrixC[95:64] = inputData[8];
assign matrixC[63:32] = 0;
assign matrixC[31:0] = 0;

wire[127:0] outputVector;

assign outputData[0] = outputVector[127:96];
assign outputData[1] = outputVector[95:64];
assign outputData[2] = outputVector[63:32];

MatrixDSP #(
	.MICROCODEA("firmware/dsp/PositionTransformMicrocodeA.mem"),
	.MICROCODEB("firmware/dsp/PositionTransformMicrocodeB.mem"),
	.MICROCODEC("firmware/dsp/PositionTransformMicrocodeC.mem"),
	
	.ENABLE_DSP_A(1),
	.ENABLE_DSP_B(1),
	.ENABLE_DSP_C(1),
	
	.MICROCODE_SIZE(16),
	.ACCUMULATOR_SIZE(2)
) mdsp (
	.clk(clk), .resetn(resetn),
	
	.cycle(cycle), .ready(ready),
	
	.inputVector(inputVector), .matrixRowA(matrixA), .matrixRowB(matrixB), .matrixRowC(matrixC), .matrixRowD(128'h00000000000000000000000000000000),
	
	.outputVector(outputVector)
);

endmodule 