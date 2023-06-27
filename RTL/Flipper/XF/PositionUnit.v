// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * RISCVSoc
 * rainbain
 * 6/26/2023
 * Orca Emulator
 *
 * Part of the XF's TransformationUnit. Transforms the 3D vertexes positions.
*/

module PositionUnit(
	//
	// Top Level
	//          
	
	input wire clk, input wire resetn,
	input wire cycle, output wire ready, output reg dataValid,
	
	
	//
	// Vertex Input
	//             
	
	input wire[6:0] vinPositionMatrix, input wire[95:0] vinPosition,  
	
	
	//
	// Vertex Output
	//
	
	output reg[95:0] worldPosition, output reg[127:0] projectedPosition,
	
	//
	// Position Matrix Port
	//
	
	output wire[6:0] posmatAddr, output reg posmatEnable,
	input wire[127:0] posmatData, input wire posmatValid,
	
	//
	// XF Memory Settings
	//
	
	input wire[63:0] xfProjectionA,
	input wire[63:0] xfProjectionB,
	input wire[63:0] xfProjectionC,
	
	input wire orthoEnable,
);


/*
 * Input Matrix Loading System
*/

reg[95:0] ldsysPosition;
reg[127:0] ldsysMatrix[0:2];
reg ldsysReady;
reg ldsysAck;

assign ready = ~ldsysReady; 

wire matrixPortDone;
wire[127:0] matrixPortA;
wire[127:0] matrixPortB;
wire[127:0] matrixPortC;
MatrixPort mtport(
	.clk(clk), .resetn(resetn),
	
	.mpAddr(posmatAddr), .mpEnable(posmatEnable), .mpData(posmatData), .mpValid(posmatValid),
	
	.mtAddr(vinPositionMatrix), .mtDone(matrixPortDone), .mtCycle(cycle),
	
	.matrixA(matrixPortA), .matrixB(matrixPortB), .matrixC(matrixPortC)
);

always @ (posedge clk) begin
	if(~resetn) begin
		ldsysReady <= 0;
	end else begin
		if(ldsysReady) begin
			ldsysReady <= ~ldsysAck;
		end else begin
			ldsysReady <= matrixPortDone;
		end
		
		if(matrixPortDone) begin
			ldsysPosition <= vinPosition;
			ldsysMatrix[0] <= matrixPortA;
			ldsysMatrix[1] <= matrixPortB;
			ldsysMatrix[2] <= matrixPortC;
		end
	end
end
    
/*
 * Transform System
*/

reg transformSysPositionMatWant;
// State Mashine

localparam [0:0]
    STATE_IDLE = 1'b0,
    STATE_PROJECT = 1'b0

reg[0:0] state;
reg[0:0] nextState;

reg[127:0] lastMatrixResult;

always @* begin
	// Defaults  
	state = nextState;
	dataValid = 0;
	dspCycle = 0;
	ldsysAck = 0;
	nextProgramPosition = 0;
	dspInputVector = lastMatrixResult;
	
	// Feeing projection as default as both projection and ortho use it.
	dspInputMatrix[0][127:64] = xfProjectionA;
	dspInputMatrix[1][127:64] = xfProjectionB;
	dspInputMatrix[2][127:64] = xfProjectionC;
	
	// The diminsions unused by orth/perspective are always position transform
	dspInputMatrix[0][63:0] = ldsysMatrix[0][63:0];
	dspInputMatrix[1][63:0] = ldsysMatrix[1][63:0];
	dspInputMatrix[2][63:0] = ldsysMatrix[2][63:0];

	case(state)
		STATE_IDLE: begin  
			if(ldsysReady) begin
				nextProgramPosition = orthoEnable ? 11 : 7;
				dspCycle = 1;
				dspInputVector = ldsysPosition;
				
				dspInputMatrix[0][127:64] = xfProjectionA;
				dspInputMatrix[1][127:64] = xfProjectionB;
				dspInputMatrix[2][127:64] = xfProjectionC;
			end
			if(dspReady) begin
				nextState = STATE_PROJECT;
				ldsysAck = 1;
			end
		end
		
		STATE_PROJECT: begin
			dspCycle = 1;
			if(dspReady) begin
				nextState = STATE_IDLE;
				dataValid = 1;
			end
		end
	endcase
end

always @ (posedge clk) begin
	if(dspReady) begin
		lastMatrixResult <= dspOutputVector;
	end
	
	if(~resetn) begin
		state <= STATE_IDLE;
	end else begin
		state <= nextState;
	end
end

/*
 * Matrix DSP
*/              

reg[3:0] nextProgramPosition;

wire[7:0] dspNextProgramPosition;
assign dspNextProgramPosition[3:0] = nextProgramPosition; 
assign dspNextProgramPosition[7:4] = 0;

reg dspCycle;
wire dspReady;

reg[127:0] dspInputVector,
reg[127:0] dspInputMatrix[2:0];

wire[127:0] dspOutputVector;

MatrixDSP #(
	.MICROCODEA("firmware/dsp/PositionTransformMicrocodeA.mem"),
	.MICROCODEB("firmware/dsp/PositionTransformMicrocodeB.mem"),
	.MICROCODEC("firmware/dsp/PositionTransformMicrocodeC.mem"),
	
	.ENABLE_DSP_A(1),
	.ENABLE_DSP_B(1),
	.ENABLE_DSP_C(1),
	
	.MICROCODE_SIZE(14),
	.ACCUMULATOR_SIZE(2)
) positionMatrixDsp(  
	.clk(clk), .resetn(resetn),
	
	.cycle(dspCycle), .ready(dspReady), .nextProgramPosition(dspNextProgramPosition),
	
	.inputVector(dspInputVector),
	.matrixRowA(dspInputMatrix[0]),
	.matrixRowB(dspInputMatrix[1]),
	.matrixRowC(dspInputMatrix[2]),
	
	.outputVector(dspOutputVector)
);

endmodule