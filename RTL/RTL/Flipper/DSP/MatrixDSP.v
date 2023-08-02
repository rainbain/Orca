// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * RISCVSoc
 * rainbain
 * 6/26/2023
 * Orca Emulator
 *
 * A highly configurable DSP for matrix operations with custom microcode, swapable actions
 * and up to 4 axis computed simultaniusly.
 * External accumulators enable axis to read data from other axis.
*/

module MatrixDSP #(
	parameter MICROCODEA = "",
	parameter MICROCODEB = "",
	parameter MICROCODEC = "",
	parameter MICROCODED = "",
	
	parameter ENABLE_DSP_A = 0,
	parameter ENABLE_DSP_B = 0,
	parameter ENABLE_DSP_C = 0,
	parameter ENABLE_DSP_D = 0,
	
	parameter MICROCODE_SIZE = 0,
	parameter ACCUMULATOR_SIZE = 0,
	
	parameter MATRIX_DSP_A_ENABLE_EXT_ACC = 0,
	parameter MATRIX_DSP_B_ENABLE_EXT_ACC = 0,
	parameter MATRIX_DSP_C_ENABLE_EXT_ACC = 0,
	parameter MATRIX_DSP_D_ENABLE_EXT_ACC = 0,
	
	parameter MATRIX_DSP_A_NABOR_ADDR = 0,
	parameter MATRIX_DSP_B_NABOR_ADDR = 0,
	parameter MATRIX_DSP_C_NABOR_ADDR = 0,
	parameter MATRIX_DSP_D_NABOR_ADDR = 0
)(
	input wire clk, input wire resetn,
	
	//
	// Controll
	//
	
	input wire cycle, output reg ready, input wire[7:0] nextProgramPosition,
	
	//
	// Vector
	//
	
  input wire[127:0] inputVector,
	
	//
	// Matrix
	//
	
	input wire[127:0] matrixRowA,
	input wire[127:0] matrixRowB,
	input wire[127:0] matrixRowC,
	input wire[127:0] matrixRowD,
	
	//
	// Output
	//
	
	output wire[127:0] outputVector
);

/*
 * State Mashine
*/

localparam [1:0] // 3 states are required for Moore
    STATE_IDLE = 2'b00,
    STATE_DISPATCH = 2'b01, 
    STATE_EXIT = 2'b10,
    STATE_INSTRUCTION_GRAB = 2'b11;
	 
reg[1:0] state;
  reg[1:0] nextState;

always @* begin
	// Defaults
	globalInstructionGrab = 0;
	globalSubmitt = 0;
	globalOutputStore = 0;
	nextState = state;
	ready = 0;

	case(state)
		STATE_IDLE: begin
			ready = ~cycle;
      		globalSubmitt = cycle;
			if(cycle)nextState = STATE_INSTRUCTION_GRAB;
    	end

    	STATE_INSTRUCTION_GRAB: begin
      		globalInstructionGrab = 1;
      		nextState = STATE_DISPATCH;
    	end
			
		STATE_DISPATCH: begin
			if(globalAllReady)begin
				globalSubmitt = 1;
				if(globalProgramAllReady) begin
					nextState = STATE_EXIT;
				end else begin
          			nextState = STATE_INSTRUCTION_GRAB;
				end
			end
    	end
			
		STATE_EXIT: begin
			globalInstructionGrab = 1;
			nextState = STATE_IDLE;
      ready = 1;
    	end
			
	endcase
end

always @ (posedge clk) begin
	if(~resetn) begin
		state <= STATE_EXIT; // Good to prime system
	end else begin
		state <= nextState;
	end

  if(state == STATE_EXIT || (state == STATE_DISPATCH & globalAllReady & globalProgramAllReady)) begin
		programCounter <= nextProgramPosition;
	end else begin
		if(globalSubmitt)programCounter <= programCounter + 1;
  end
end

/*
 * Gloabl State
*/

reg[7:0] programCounter;

reg globalInstructionGrab;
reg globalSubmitt;
reg globalOutputStore;

wire globalAllReady = mdspAReady & mdspBReady & mdspCReady & mdspDReady;
wire globalProgramAllReady = mdspAProgramReady & mdspBProgramReady & mdspCProgramReady & mdspDProgramReady;

/*
 * Put Matrix DSPs Accumulator Data Lines Into Memory Mapped Bus
*/

wire[1:0] matrixDSPDeviceAddresses[0:3];
assign matrixDSPDeviceAddresses[0] = mdspAAccAddrDevice;
assign matrixDSPDeviceAddresses[1] = mdspBAccAddrDevice;
assign matrixDSPDeviceAddresses[2] = mdspCAccAddrDevice;
assign matrixDSPDeviceAddresses[3] = mdspDAccAddrDevice;

wire matrixDSPDeviceEnables[0:3];
assign matrixDSPDeviceEnables[0] = mdspAAccEnDevice;
assign matrixDSPDeviceEnables[1] = mdspBAccEnDevice;
assign matrixDSPDeviceEnables[2] = mdspCAccEnDevice;
assign matrixDSPDeviceEnables[3] = mdspDAccEnDevice;

wire[31:0] matrixDSPDeviceDatas[0:3];
assign matrixDSPDeviceDatas[0] = mdspAAccDataDevice;
assign matrixDSPDeviceDatas[1] = mdspBAccDataDevice;
assign matrixDSPDeviceDatas[2] = mdspCAccDataDevice;
assign matrixDSPDeviceDatas[3] = mdspDAccDataDevice;

/*
 * Map Matrix DSP Hosts onto Bus
*/

  generate if (MATRIX_DSP_A_ENABLE_EXT_ACC == 1) begin
	assign matrixDSPDeviceAddresses[MATRIX_DSP_A_NABOR_ADDR] = mdspAAccAddrHost;
	assign matrixDSPDeviceEnables[MATRIX_DSP_A_NABOR_ADDR] = mdspAAccEnHost;
	assign matrixDSPDeviceDatas[MATRIX_DSP_A_NABOR_ADDR] = mdspAAccDataHost;
    end
endgenerate

    generate if (MATRIX_DSP_B_ENABLE_EXT_ACC == 1) begin
	assign matrixDSPDeviceAddresses[MATRIX_DSP_B_NABOR_ADDR] = mdspBAccAddrHost;
	assign matrixDSPDeviceEnables[MATRIX_DSP_B_NABOR_ADDR] = mdspBAccEnHost;
  assign matrixDSPDeviceDatas[MATRIX_DSP_B_NABOR_ADDR] = mdspBAccDataHost;
      end
endgenerate

      generate if (MATRIX_DSP_C_ENABLE_EXT_ACC == 1) begin
	assign matrixDSPDeviceAddresses[MATRIX_DSP_C_NABOR_ADDR] = mdspCAccAddrHost;
	assign matrixDSPDeviceEnables[MATRIX_DSP_C_NABOR_ADDR] = mdspCAccEnHost;
  assign matrixDSPDeviceDatas[MATRIX_DSP_C_NABOR_ADDR] = mdspCAccDataHost;
        end
endgenerate

        generate if (MATRIX_DSP_D_ENABLE_EXT_ACC == 1) begin
	assign matrixDSPDeviceAddresses[MATRIX_DSP_D_NABOR_ADDR] = mdspDAccAddrHost;
	assign matrixDSPDeviceEnables[MATRIX_DSP_D_NABOR_ADDR] = mdspDAccEnHost;
  assign matrixDSPDeviceDatas[MATRIX_DSP_D_NABOR_ADDR] = mdspDAccDataHost;
          end
endgenerate 


/*
 * Matrix DSP A
*/

wire mdspAReady;
wire mdspAProgramReady;

wire[1:0] mdspAAccAddrDevice;
wire mdspAAccEnDevice;
wire[31:0] mdspAAccDataDevice;

wire[1:0] mdspAAccAddrHost;
wire mdspAAccEnHost;
wire[31:0] mdspAAccDataHost;

wire[1:0] mdspAMatrixSelect;
wire[127:0] mdspAMatrix;

          generate if (ENABLE_DSP_A == 1)  begin
	assign mdspAMatrix = (mdspAMatrixSelect == 0) ? matrixRowA :
									  (mdspAMatrixSelect == 1) ? matrixRowB :
									  (mdspAMatrixSelect == 2) ? matrixRowC :
									  matrixRowD;

	MatrixDSPUnit #(
		.MICROCODE(MICROCODEA),
		.MICROCODE_SIZE(MICROCODE_SIZE),
		.ACCUMULATOR_SIZE(ACCUMULATOR_SIZE),
		.LOCAL_ADDRESS(0),
		.ENABLE_EXT_ACC(MATRIX_DSP_A_ENABLE_EXT_ACC)
	)mdspA (
		.clk(clk), .resetn(resetn),
		
		.programCounter(programCounter), .submit(globalSubmitt),
		.ready(mdspAReady), .instructionGrab(globalInstructionGrab), .programReady(mdspAProgramReady),
		
		.vector(inputVector), .matrix(mdspAMatrix),
		.matrixSelect(mdspAMatrixSelect),
		
		.result(outputVector[127:96]),
		
		.externalAcculatorAddrDevice(mdspAAccAddrDevice), .externalAcccumulatorEnableDevice(mdspAAccEnDevice),
		.externalAccumulatorDataDevice(mdspAAccDataDevice),
		
		.externalAcculatorAddrHost(mdspAAccAddrHost), .externalAcccumulatorEnableHost(mdspAAccEnHost),
		.externalAccumulatorDataHost(mdspAAccDataHost)
	);
            end else begin

	assign mdspAReady = 1;
  assign mdspAProgramReady = 1;

	assign mdspAAccDataDevice = 0;
	assign mdspAAccAddrHost = 0;
	assign mdspAAccEnHost = 0;
	assign mdspAAccDataHost = 0;
  assign outputVector[127:96] = 0;
end
endgenerate


/*
 * Matrix DSP B
*/

wire mdspBReady;
wire mdspBProgramReady;

wire[1:0] mdspBAccAddrDevice;
wire mdspBAccEnDevice;
wire[31:0] mdspBAccDataDevice;

wire[1:0] mdspBAccAddrHost;
wire mdspBAccEnHost;
wire[31:0] mdspBAccDataHost;

wire[1:0] mdspBMatrixSelect;
wire[127:0] mdspBMatrix;

generate if (ENABLE_DSP_B == 1) begin
	assign mdspBMatrix = (mdspBMatrixSelect == 0) ? matrixRowA :
									  (mdspBMatrixSelect == 1) ? matrixRowB :
									  (mdspBMatrixSelect == 2) ? matrixRowC :
									  matrixRowD;

	MatrixDSPUnit #(
		.MICROCODE(MICROCODEB),
		.MICROCODE_SIZE(MICROCODE_SIZE),
		.ACCUMULATOR_SIZE(ACCUMULATOR_SIZE),
		.LOCAL_ADDRESS(1),
		.ENABLE_EXT_ACC(MATRIX_DSP_B_ENABLE_EXT_ACC)
  )mdspB (
		.clk(clk), .resetn(resetn),
		
		.programCounter(programCounter), .submit(globalSubmitt),
		.ready(mdspBReady), .instructionGrab(globalInstructionGrab), .programReady(mdspBProgramReady),
		
		.vector(inputVector), .matrix(mdspBMatrix),
		.matrixSelect(mdspBMatrixSelect),
		
		.result(outputVector[95:64]),
		
		.externalAcculatorAddrDevice(mdspBAccAddrDevice), .externalAcccumulatorEnableDevice(mdspBAccEnDevice),
		.externalAccumulatorDataDevice(mdspBAccDataDevice),
		
		.externalAcculatorAddrHost(mdspBAccAddrHost), .externalAcccumulatorEnableHost(mdspBAccEnHost),
		.externalAccumulatorDataHost(mdspBAccDataHost)
	);
end else begin

	assign mdspBReady = 1;
  assign mdspBProgramReady = 1;

	assign mdspBAccDataDevice = 0;
	assign mdspBAccAddrHost = 0;
	assign mdspBAccEnHost = 0;
	assign mdspBAccDataHost = 0;
  assign outputVector[95:64] = 0;
end
endgenerate


/*
 * Matrix DSP C
*/

wire mdspCReady;
wire mdspCProgramReady;

wire[1:0] mdspCAccAddrDevice;
wire mdspCAccEnDevice;
wire[31:0] mdspCAccDataDevice;

wire[1:0] mdspCAccAddrHost;
wire mdspCAccEnHost;
wire[31:0] mdspCAccDataHost;

wire[1:0] mdspCMatrixSelect;
wire[127:0] mdspCMatrix;

generate if (ENABLE_DSP_C == 1) begin
	
	assign mdspCMatrix = (mdspCMatrixSelect == 0) ? matrixRowA :
									  (mdspCMatrixSelect == 1) ? matrixRowB :
									  (mdspCMatrixSelect == 2) ? matrixRowC :
									  matrixRowD;

	MatrixDSPUnit #(
		.MICROCODE(MICROCODEC),
		.MICROCODE_SIZE(MICROCODE_SIZE),
		.ACCUMULATOR_SIZE(ACCUMULATOR_SIZE),
		.LOCAL_ADDRESS(2),
		.ENABLE_EXT_ACC(MATRIX_DSP_C_ENABLE_EXT_ACC)
	)mdspC (
		.clk(clk), .resetn(resetn),
		
		.programCounter(programCounter), .submit(globalSubmitt),
		.ready(mdspCReady), .instructionGrab(globalInstructionGrab), .programReady(mdspCProgramReady),
		
		.vector(inputVector), .matrix(mdspCMatrix),
		.matrixSelect(mdspCMatrixSelect),
		
		.result(outputVector[63:32]),
		
		.externalAcculatorAddrDevice(mdspCAccAddrDevice), .externalAcccumulatorEnableDevice(mdspCAccEnDevice),
		.externalAccumulatorDataDevice(mdspCAccDataDevice),
		
		.externalAcculatorAddrHost(mdspCAccAddrHost), .externalAcccumulatorEnableHost(mdspCAccEnHost),
		.externalAccumulatorDataHost(mdspCAccDataHost)
	);
end else begin

	assign mdspCReady = 1;
  assign mdspCProgramReady = 1;

	assign mdspCAccDataDevice = 0;
	assign mdspCAccAddrHost = 0;
	assign mdspCAccEnHost = 0;
	assign mdspCAccDataHost = 0;
  assign outputVector[63:32] = 0;
end
endgenerate


/*
 * Matrix DSP D
*/

wire mdspDReady;
wire mdspDProgramReady;

wire[1:0] mdspDAccAddrDevice;
wire mdspDAccEnDevice;
wire[31:0] mdspDAccDataDevice;

wire[1:0] mdspDAccAddrHost;
wire mdspDAccEnHost;
wire[31:0] mdspDAccDataHost;

wire[1:0] mdspDMatrixSelect;
wire[127:0] mdspDMatrix;

  generate if (ENABLE_DSP_D == 1) begin
	assign mdspDMatrix = (mdspDMatrixSelect == 0) ? matrixRowA :
									  (mdspDMatrixSelect == 1) ? matrixRowB :
									  (mdspDMatrixSelect == 2) ? matrixRowC :
									  matrixRowD;

	MatrixDSPUnit #(
		.MICROCODE(MICROCODED),
		.MICROCODE_SIZE(MICROCODE_SIZE),
		.ACCUMULATOR_SIZE(ACCUMULATOR_SIZE),
		.LOCAL_ADDRESS(3),
		.ENABLE_EXT_ACC(MATRIX_DSP_D_ENABLE_EXT_ACC)
	)mdspD (
		.clk(clk), .resetn(resetn),
		
		.programCounter(programCounter), .submit(globalSubmitt),
		.ready(mdspDReady), .instructionGrab(globalInstructionGrab), .programReady(mdspDProgramReady),
		
		.vector(inputVector), .matrix(mdspDMatrix),
		.matrixSelect(mdspDMatrixSelect),
		
		.result(outputVector[31:0]),
		
		.externalAcculatorAddrDevice(mdspDAccAddrDevice), .externalAcccumulatorEnableDevice(mdspDAccEnDevice),
		.externalAccumulatorDataDevice(mdspDAccDataDevice),
		
		.externalAcculatorAddrHost(mdspDAccAddrHost), .externalAcccumulatorEnableHost(mdspDAccEnHost),
		.externalAccumulatorDataHost(mdspDAccDataHost)
	);

end else begin

	assign mdspDReady = 1;
  assign mdspDProgramReady = 1;

	assign mdspDAccDataDevice = 0;
	assign mdspDAccAddrHost = 0;
	assign mdspDAccEnHost = 0;
	assign mdspDAccDataHost = 0;
  assign outputVector[31:0] = 0;
end
endgenerate

endmodule 