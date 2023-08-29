// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * PositionUnit
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
	
	input wire[5:0] vinPositionMatrix, input wire[95:0] vinPosition,  
	
	
	//
	// Vertex Output
	//
	
	output reg[95:0] worldPosition, output reg[127:0] projectedPosition,
	
	//
	// Position Matrix Port
	//
	
  output wire[5:0] posmatAddr, output wire posmatEnable,
	input wire[127:0] posmatData, input wire posmatValid,
	
	//
	// XF Memory Settings
	//
	
	input wire[63:0] xfProjectionA,
	input wire[63:0] xfProjectionB,
	input wire[63:0] xfProjectionC,
	
	input wire orthoEnable
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

// State Mashine

  localparam [1:0]
    STATE_IDLE = 2'b00,
    STATE_MDSP_WAIT_A = 2'b01,
    STATE_PROJECT = 2'b10,
    STATE_MDSP_WAIT_B = 2'b11;

reg[1:0] state;
reg[1:0] nextState;

always @* begin
	// Defaults  
	nextState = state;
	dspCycle = 0;
	ldsysAck = 0;
	nextProgramPosition = 0;
  	dspInputVector[127:32] = worldPosition;
	
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
				dspInputVector[127:32] = ldsysPosition;
				
				dspInputMatrix[0][127:64] = ldsysMatrix[0][127:64];
				dspInputMatrix[1][127:64] = ldsysMatrix[1][127:64];
				dspInputMatrix[2][127:64] = ldsysMatrix[2][127:64];

				nextState = STATE_MDSP_WAIT_A;
		  end
		end

    	STATE_MDSP_WAIT_A: begin
			nextProgramPosition = orthoEnable ? 11 : 7;
      		dspInputMatrix[0][127:64] = ldsysMatrix[0][127:64];
      		dspInputMatrix[1][127:64] = ldsysMatrix[1][127:64];
      		dspInputMatrix[2][127:64] = ldsysMatrix[2][127:64];
      		dspInputVector[127:32] = ldsysPosition;
      		dspCycle = 0;
      		nextState = dspReady ? STATE_PROJECT : state;
      		ldsysAck = dspReady;
    	end
		
		STATE_PROJECT: begin
			dspCycle = 1;
      		nextState = STATE_MDSP_WAIT_B;
		end
    	STATE_MDSP_WAIT_B: begin
      		nextState = dspReady ? STATE_IDLE : STATE_MDSP_WAIT_B;
    	end
	endcase
end

always @ (posedge clk) begin
	if(~resetn) begin
		state <= STATE_IDLE;
	end else begin
		state <= nextState;
	end

  if(state == STATE_MDSP_WAIT_A & dspReady) begin
    worldPosition <= dspOutputVector[127:32];
  end

  if(state == STATE_MDSP_WAIT_B & dspReady) begin
    projectedPosition <= dspOutputVector;
    dataValid <= 1;
  end else begin
    dataValid <= 0;
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

reg[127:0] dspInputVector;
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