// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * NormalUnit
 * rainbain
 * 6/26/2023
 * Orca Emulator
 *
 * Part of the XF's TransformationUnit. Transforms 3D normals.
*/

module NormalUnit(
	//
	// Top Level
	//
	
	input wire clk, input wire resetn,
	input wire cycle, output wire ready, output reg dataValid,
	
	
	//
	// Normal Input
	//
	
	input wire[95:0] normalN, input wire[95:0] normalT, input wire[95:0] normalB,
	input wire[4:0] normalMatrixAddr, input wire enableNTB,
	
	//
	// Output
	//
	
	output reg[95:0] outputNormalN, output reg[95:0] outputNormalT, output reg[95:0] outputNormalB,
	
	//
	// Normal Matrix Port
	//
	
	output wire[4:0] nommatAddr, output wire nommatEnable,
	input wire[95:0] nommatData, input wire nommatValid
);

/*
 * Input Matrix Loading System
*/

reg[95:0] ldsysNormalN;
reg[95:0] ldsysNormalT;
reg[95:0] ldsysNormalB;
reg[95:0] ldsysMatrix[0:2];
reg ldsysEnableNTB;
reg ldsysReady;
reg ldsysAck;

assign ready = ~ldsysReady; 

wire matrixPortDone;
wire[95:0] matrixPortA;
wire[95:0] matrixPortB;
wire[95:0] matrixPortC;
MatrixPort #(
	.WIDTH(96)
) mtport(
	.clk(clk), .resetn(resetn),
	
	.mpAddr(nommatAddr), .mpEnable(nommatEnable), .mpData(nommatData), .mpValid(nommatValid),
	
	.mtAddr(normalMatrixAddr), .mtDone(matrixPortDone), .mtCycle(cycle),
	.rowCount(3),
	
	.matrixA(matrixPortA), .matrixB(matrixPortB), .matrixC(matrixPortC)
);

always @ (posedge clk) begin
	if(~resetn) begin
		ldsysReady <= 0;
	end else begin
		if(ldsysReady) begin
      		if(ldsysAck)ldsysReady <= 0;
		end else begin
			ldsysReady <= matrixPortDone;
		end
		
		if(matrixPortDone) begin
			ldsysNormalN <= normalN;
			ldsysNormalT <= normalT;
			ldsysNormalB <= normalB;
			ldsysMatrix[0] <= matrixPortA;
			ldsysMatrix[1] <= matrixPortB;
			ldsysMatrix[2] <= matrixPortC;
			ldsysEnableNTB <= enableNTB;
		end
	end
end
 
 
/*
 * Transform System
*/

localparam [2:0]
	STATE_IDLE =          3'b000,
    STATE_NORMAL_N_WAIT = 3'b001,
    STATE_NORMAL_T =      3'b010,
    STATE_NORMAL_T_WAIT = 3'b011,
    STATE_NORMAL_B =      3'b100,
    STATE_NORMAL_B_WAIT = 3'b101;

reg[2:0] state;
reg[2:0] nextState;

always @* begin
	// Defaults
	nextState = state;
	dspCycle = 0;
	ldsysAck = 0;
	
	inputVector = ldsysNormalN;

	case(state)
		STATE_IDLE: begin
			if(ldsysReady) begin
				dspCycle = 1;
				nextState = STATE_NORMAL_N_WAIT;
			end
		end
		
		STATE_NORMAL_N_WAIT: begin
			nextState = dspReady ? (ldsysEnableNTB ? STATE_NORMAL_T : STATE_IDLE) : state;
			ldsysAck = dspReady ? (ldsysEnableNTB ? 0 : 1) : 0;
		end
		
		STATE_NORMAL_T: begin
			inputVector = ldsysNormalT;
			dspCycle = 1;
			nextState = STATE_NORMAL_T_WAIT;
		end
		
		STATE_NORMAL_T_WAIT: begin
			inputVector = ldsysNormalT;
			nextState = dspReady ? STATE_NORMAL_B : state;
		end
		
		STATE_NORMAL_B: begin
			inputVector = ldsysNormalB;
			dspCycle = 1;
			nextState = STATE_NORMAL_B_WAIT;
		end
		
		STATE_NORMAL_B_WAIT: begin
			inputVector = ldsysNormalB;
			nextState = dspReady ? STATE_IDLE : state;
			ldsysAck = 1;
		end
	endcase
end

// State itor
always @ (posedge clk) begin
	if(~resetn) begin
		state <= STATE_IDLE;
	end else begin
		state <= nextState;
	end
	                          
	// Save Data
	if(state == STATE_NORMAL_N_WAIT & dspReady) begin
		outputNormalN <= dspOutputVector[127:32];
		if(~ldsysEnableNTB) begin
			dataValid <= 1;
		end
	end
	if(state == STATE_NORMAL_T_WAIT & dspReady)
		outputNormalT <= dspOutputVector[127:32];
	if(state == STATE_NORMAL_B_WAIT & dspReady) begin
		outputNormalB <= dspOutputVector[127:32];
		dataValid <= 1;
	end else begin
		dataValid <= 0;
	end
end

/*
 * Matrix DSP
*/

wire[7:0] dspNextProgramPosition = 0;

reg dspCycle;
wire dspReady;

reg[95:0] inputVector;
wire[128:0] dspInputVector;
assign dspInputVector[127:32] = inputVector;

wire[127:0] dspInputMatrix[2:0];
assign dspInputMatrix[0][127:32] = ldsysMatrix[0];
assign dspInputMatrix[1][127:32] = ldsysMatrix[1];
assign dspInputMatrix[2][127:32] = ldsysMatrix[2];

wire[127:0] dspOutputVector;

MatrixDSP #(
	.MICROCODEA("firmware/dsp/NormalTransformMicrocodeA.mem"),
	.MICROCODEB("firmware/dsp/NormalTransformMicrocodeB.mem"),
	.MICROCODEC("firmware/dsp/NormalTransformMicrocodeC.mem"),
	
	.ENABLE_DSP_A(1),
	.ENABLE_DSP_B(1),
	.ENABLE_DSP_C(1),
	
	.MICROCODE_SIZE(6),
	.ACCUMULATOR_SIZE(2)
) normalMatrixDsp(
	.clk(clk), .resetn(resetn),
	
	.cycle(dspCycle), .ready(dspReady), .nextProgramPosition(dspNextProgramPosition),
	
	.inputVector(dspInputVector),
	.matrixRowA(dspInputMatrix[0]),
	.matrixRowB(dspInputMatrix[1]),
	.matrixRowC(dspInputMatrix[2]),
	
	.outputVector(dspOutputVector)
);                     

endmodule