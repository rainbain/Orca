// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * RISCVSoc
 * rainbain
 * 6/26/2023
 * Orca Emulator
 *
 * A single axis of the Matrix DSP.
*/

module MatrixDSPUnit #(
	parameter MICROCODE = "",
	parameter MICROCODE_SIZE = 0,
	
	parameter ACCUMULATOR_SIZE = 0,
	parameter LOCAL_ADDRESS = 0,
	
	parameter ENABLE_EXT_ACC = 0
)(
	input wire clk, input wire resetn,
	
	//
	// Controll
	//
	
	input wire[7:0] programCounter, input wire submit,
	output wire ready, input wire instructionGrab, output wire programReady,
	
	//
	// IO
	//
	
  input wire[127:0] vector, input wire[127:0] matrix,
	output wire[1:0] matrixSelect,
	
	output wire[31:0] result,
	
	input wire[1:0] externalAcculatorAddrDevice, input wire externalAcccumulatorEnableDevice,
	output wire[31:0] externalAccumulatorDataDevice,
	
	output wire[1:0] externalAcculatorAddrHost, output wire externalAcccumulatorEnableHost,
	input wire[31:0] externalAccumulatorDataHost
);

/*
 * Accumulator
*/

reg[31:0] accumulator[0:ACCUMULATOR_SIZE-1];
reg[1:0] dataOutAddr;

// Port A
wire[1:0] accPortAAddr;
wire[31:0] accPortAData = accumulator[accPortAAddr];

// Port B
wire[1:0] accPortBAddr;
wire[31:0] accPortBData = accumulator[accPortBAddr];

// Result Part
assign result = accumulator[0];


/*
 * Instruction Memory
*/
 
wire[3:0] inputAAddress;
wire inputASelect;
wire[3:0] inputBAddress;
wire[1:0] inputBSelect;
wire[2:0] operation;
wire[1:0] outputAddress;

wire dspMemoryInstructionEnable = instructionGrab;

MatrixDSPMemory #(
	.MICROCODE(MICROCODE),
	
	.MICROCODE_SIZE(MICROCODE_SIZE)
) dspMemory(
  .clk(clk),
	.addr(programCounter), .enable(dspMemoryInstructionEnable),
	
	.inputAAddress(inputAAddress), .inputASelect(inputASelect),
	.inputBAddress(inputBAddress), .inputBSelect(inputBSelect),
	.operation(operation), .outputAddress(outputAddress)
);

/*
 * DSP
*/

// Statup state may be skippable if DSP retains data. Check that

reg[31:0] dspDataA;
reg[31:0] dspDataB;
reg[2:0] dspOperation;
reg dspStart;
wire[31:0] dspResult;
wire dspDone;

FloatDSP fdsp(
  .clk(clk), .clk_en(1'b1), .dataa(fastAccumulatorDataSwitch), .datab(dspDataB), .n(dspOperation),
  .reset(~resetn), .done(dspDone), .result(dspResult), .start(dspStart)
);

/*
 * Ready Signal
*/

reg initalReady;

always @ (posedge clk) begin
  if(~resetn) begin
	 initalReady <= 1;
  end else begin
	if(dspDone) begin
		accumulator[dataOutAddr] <= dspResult;
    initalReady <= 1;
    dataAAccumulatorData <= dspResult;
	end
    if(dspStart)initalReady <= 0;
  end
end

assign ready = dspDone | initalReady;

/*
 * Input Data decoder
*/

wire[31:0] dataA;
wire[31:0] dataB;

// Lookup Vectors
wire lookupVectorA = inputASelect == 0;
wire lookupAccumulatorA = inputASelect == 1;

wire lookupVectorB = inputBSelect == 0;
wire lookupMatrixB = inputBSelect == 1;
wire lookupAccumulatorB = inputBSelect == 2;

wire localAccumulatorSelected;

wire[31:0] portAAccumulatorData;
wire[31:0] portBAccumulatorData;

  generate if (ENABLE_EXT_ACC == 1) begin
	// Input A Accumulator
	assign externalAcculatorAddrHost = inputAAddress[1:0];
	assign localAccumulatorSelected = inputAAddress[3:2] == LOCAL_ADDRESS;

	assign externalAcccumulatorEnableHost = (~localAccumulatorSelected) & lookupAccumulatorA;

	assign accPortAAddr = inputAAddress[1:0];
	assign portAAccumulatorData = externalAcccumulatorEnableHost ? externalAccumulatorDataHost :
                                accPortAData;

	// Input B Accumulator
	assign accPortBAddr = externalAcccumulatorEnableDevice ? externalAcculatorAddrDevice : inputBAddress[1:0];
	assign externalAccumulatorDataDevice = accPortBData;
	assign portBAccumulatorData = accPortBData;
end else begin
	// Input Accumulator
	assign accPortAAddr = inputAAddress[1:0];
	assign accPortBAddr = inputBAddress[1:0];
	
	assign portAAccumulatorData = accPortAData;
	assign portBAccumulatorData = accPortBData;

  assign localAccumulatorSelected = 1;
end
endgenerate

	// Input A Vector Decoder
wire[31:0] portAVectorData = (inputAAddress[1:0] == 0) ? vector[127:96] :
									  (inputAAddress[1:0] == 1) ? vector[95:64] :
                    (inputAAddress[1:0] == 2) ? vector[63:32] :
									  vector[31:0];

// Input B Vector And Matrix Decoder

assign matrixSelect = inputBAddress[3:2];
wire[127:0] portBIncomingData = lookupMatrixB ? matrix : vector;
wire[31:0] portBVectorData = (inputBAddress[1:0] == 0) ? portBIncomingData[127:96] :
									  (inputBAddress[1:0] == 1) ? portBIncomingData[95:64] :
                    (inputBAddress[1:0] == 2) ? portBIncomingData[63:32] :
									  portBIncomingData[31:0];
// Input A Data Mux
assign dataA = lookupVectorA ? portAVectorData : portAAccumulatorData;

// Input B Data Mux
assign dataB = (lookupVectorB|lookupMatrixB) ? portBVectorData : portBAccumulatorData;

// Program Ready Logic
assign programReady = (operation == 7);

/*
 * Fast Accumulator
*/

reg[1:0] lastAccumulatorOutputAddr;

  wire fastAccumulatorEnable = localAccumulatorSelected & (lastAccumulatorOutputAddr == accPortAAddr) & lookupAccumulatorA;

reg[31:0] dataAAccumulatorData;
reg switchEnable;

wire[31:0] fastAccumulatorDataSwitch = switchEnable ? dataAAccumulatorData : dspDataA;


/*
 * State Control
*/

always @ (posedge clk) begin
	if(~resetn) begin
		dspStart <= 0;
		dspOperation <= 0;
    switchEnable <= 0;
    lastAccumulatorOutputAddr <= 0;
    dataOutAddr <= 0;
	end else begin
    if(ready & ~dspStart) begin
			// Enter not ready and submit to dsp.
			if(submit & ~programReady) begin
				dspDataA <= dataA;
				dspDataB <= dataB;
        switchEnable <= fastAccumulatorEnable;
				dspOperation <= operation;
				dspStart <= 1;
				dataOutAddr <= outputAddress;
			end
		end else begin
      dspStart <= 0;
      lastAccumulatorOutputAddr <= dataOutAddr;
		end
	end
end

endmodule 