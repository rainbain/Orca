// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * TexCoordRegularUnit
 * rainbain
 * 6/26/2023
 * Orca Emulator
 *
 * Part of the XF's TextureCoord Transform Unit.
 * Transforms regular texture coords.
*/

module TexCoordRegularUnit(
	//
	// Top Level
	//
	
	input wire clk, input wire resetn,
	input wire cycle, input reg ready, input reg dataValid,
	
	//
	// Texcoord Input
	//
	
	input wire[95:0] inputTexcoord,
	input wire[5:0] inputTexMatrix,
	input wire useAB11, input wire useST, input wire useNormalze,
	input wire useDualTrans,
	
	input wire[5:0] postTexMatrix,
	
	//
	// Texcoord Output
	//
	
	output reg[95:0] outputTexcoord,
	
	//
	// Position Matrix Port
	//
	
	output wire[5:0] posmatAddr, output wire posmatEnable,
	input wire[127:0] posmatData, input wire posmatValid
);

/*
 * Input Matrix Loading System
*/

reg[95:0] ldsysTexcoord;
reg[127:0] ldsysMatrix[0:2];
reg ldsysUseAB11;
reg ldsysUseST;
reg ldsysUseNormalize;
reg ldsysUseDualTrans;

reg ldsysReady;
reg ldsysAck;

wire matrixPortDone;
wire[127:0] matrixPortA;
wire[127:0] matrixPortB;
wire[127:0] matrixPortC;

reg ldsysCycle;

reg[5:0] ldsysLoadAddr;

MatrixPort mtport(
	.clk(clk), .resetn(resetn),
	
	.mpAddr(posmatAddr), .mpEnable(posmatEnable), .mpData(posmatData), .mpValid(posmatValid),
	
	.mtAddr(ldsysLoadAddr), .mtDone(matrixPortDone), .mtCycle(ldsysCycle),
	
	.matrixA(matrixPortA), .matrixB(matrixPortB), .matrixC(matrixPortC)
);

reg saveInitalData;
reg saveMatrixData;

always @ (posedge clk) begin
	if(~resetn) begin
		ldsysReady <= 0;
	end else begin
		if(ldsysReady) begin
			if(ldsysAck) ldsysReady <= 0;
		end else begin
			ldsysReady <= matrixPortDone;
		end
		
		if(saveInitalData) begin
			ldsysTexcoord <= inputTexcoord;
			ldsysMatrix[0] <= matrixPortA;
			ldsysMatrix[1] <= matrixPortB;
			ldsysMatrix[2] <= matrixPortC;
			
			ldsysUseAB11 <= useAB11;
			ldsysUseST <= useST;
			ldsysUseNormalize <= useNormalze;
			ldsysUseDualTrans <= useDualTrans;
		end
	end
end
  
  
/*
 * Extended Load System Control for post matrix
*/

localparam [1:0]
	LDSYS_IDLE =        3'b000,
	LDSYS_GET  =        3'b001,
	LDSYS_GET_POST =    3'b010,
	LDSYS_WAIT_POST =   3'b011
	LDSYS_WAIT_ACK =    3'b100;

reg[2:0] ldsysState;
reg[2:0] ldsysNextState;

always @* begin
	// Defaults
	ldsysNextState = ldsysState;
	ready = 0;
	ldsysCycle = 0;
	saveInitalData = 0;
	ldsysLoadAddr = inputTexMatrix;

	case(ldsysState)
		LDSYS_IDLE: begin
			ldsysCycle = cycle;
			if(cycle) begin
				ldsysNextState = LDSYS_GET;
			end
			ready = 1;
		end
		LDSYS_GET: begin
			if(matrixPortDone) begin
				saveInitalData = 1;
				saveMatrixData = 1;
				
				if(useDualTrans) begin
					ldsysNextState = LDSYS_GET_POST;
				end else begin
					ldsysNextState = LDSYS_WAIT_ACK;
				end
			end
		end
		LDSYS_GET_POST: begin
			if(ldsysAck) begin
				ldsysLoadAddr = postTexMatrix;
				ldsysCycle = 1;
			end
		end
		LDSYS_WAIT_POST: begin
			if(matrixPortDone) begin
				saveMatrixData = 1;
				ldsysNextState = LDSYS_WAIT_ACK;
			end
		end
		LDSYS_WAIT_ACK: begin
			if(ldsysAck) begin
				ldsysNextState = LDSYS_IDLE;
				ready = 1;
			end
		end
	endcase
end

always @ (posedge clk) begin
	if(~resetn) begin
		ldsysState <= LDSYS_IDLE;
	end else begin
		ldsysState <= ldsysNextState;
	end
end

/*
 * Transform System
*/

localparam [2:0]
	STATE_IDLE =        3'b000,
	STATE_MATRIX_WAIT = 3'b001,
	STATE_DUAL =        3'b010,
	STATE_DUAL_WAIT =   3'b011;

reg[2:0] state;
reg[2:0] nextState;
reg enableZIdintityInput;
reg enableZIdintityOutput;

reg[95:0] savedTexcoord;

always @* begin
	// Defaults
	nextState = state;
	dspCycle = 0;
	ldsysAck = 0;
	enableZIdintityInput = 0;
	enableZIdintityOutput = 0;
	
	inputVector = ldsysTexcoord;
	savedTexcoord = outputVector;
	
	case(state)
		STATE_IDLE: begin
			if(ldsysReady) begin
				dspCycle = 1;
				nextState = STATE_MATRIX_WAIT;
				
				// In AB11 supplement input z for 1
				if(ldsysUseAB11) begin
					enableZIdintityInput = 1;
				end
				
				// In ST supplement output z for 1
				if(ldsysUseST) begin
					enableZIdintityOutput = 1;	
				end
			end
		end
		STATE_MATRIX_WAIT: begin
			nextState = dspReady ? (ldsysUseDualTrans ? STATE_DUAL : STATE_IDLE) : state;
			ldsysAck = dspReady;
			
			// Hold data
			if(ldsysUseAB11) begin
				enableZIdintityInput = 1;
			end
			if(ldsysUseST) begin
				enableZIdintityOutput = 1;	
			end
		end
		STATE_DUAL: begin
		end
		STATE_DUAL_WAIT: begin
		end
	endcase
end

// State Itor
always @ (posedge clk) begin
	if(~resetn) begin
		state <= STATE_IDLE;
	end else begin          
		state <= nextState;
	end
	
	// Save Data
	if(state == STATE_MATRIX_WAIT & dspReady) begin
		outputTexcoord <= savedTexcoord;
		if(~useDualTrans) begin
			dataValid <= 1;
		end
	end
	if(state == STATE_DUAL_WAIT & dspReady) begin
		outputTexcoord <= savedTexcoord;
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
assign dspInputVector[127:64] = inputVector[95:32];
assign dspInputVector[63:32] = enableZIdintityInput ? 32'h3f800000 : inputVector[31:0];

wire[127:0] dspOutputVector;
wire[95:0] outputVector = dspOutputVector[127:32];
assign outputVector[95:32] = dspOutputVector[127:64];
assign outputVector[31:0] = enableZIdintityOutput ? 32'h3f800000 : inputVector[31:0];

MatrixDSP #(
	.MICROCODEA("firmware/dsp/TexcoordTransformMicrocodeA.mem"),
	.MICROCODEB("firmware/dsp/TexcoordTransformMicrocodeB.mem"),
	.MICROCODEC("firmware/dsp/TexcoordTransformMicrocodeC.mem"),
	
	.ENABLE_DSP_A(1),
	.ENABLE_DSP_B(1),
	.ENABLE_DSP_C(1),
	
	.MICROCODE_SIZE(6),
	.ACCUMULATOR_SIZE(2)
) texcoordMatrixDSP(
	.clk(clk), .resetn(resetn),
	
	.cycle(dspCycle), .ready(dspReady), .nextProgramPosition(dspNextProgramPosition),
	
	.inputVector(dspInputVector),
	.matrixRowA(ldsysMatrix[0]),
	.matrixRowB(ldsysMatrix[1]),
	.matrixRowC(ldsysMatrix[2]),
	
	.outputVector(dspOutputVector)
);



endmodule