// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * RISCVSoc
 * rainbain
 * 6/26/2023
 * Orca Emulator
 *
 * Infered or distributed ROM, 16 bits wide split up into Matrix DSP axis instructions.
*/

module MatrixDSPMemory #(
parameter MICROCODE = "",
parameter MICROCODE_SIZE = 0
)(
  input wire clk,
	input wire[7:0] addr, input wire enable,
	
	//
	// Instruction
	//
	
	output wire[3:0] inputAAddress, output wire inputASelect,
	output wire[3:0] inputBAddress, output wire[1:0] inputBSelect,
	output wire[2:0] operation, output wire[1:0] outputAddress
);

// Output Hold Register

reg[15:0] outputValue;
assign inputAAddress = outputValue[15:12];
assign inputASelect = outputValue[11];
assign inputBAddress = outputValue[10:7];
assign inputBSelect = outputValue[6:5];
assign operation = outputValue[4:2];
assign outputAddress = outputValue[1:0];       

// Infered block memory on Altra device. Use BRAM generator on Xilinx

reg[15:0] memory[0:MICROCODE_SIZE-1];

initial begin
  $readmemh(MICROCODE, memory);
end

always @ (posedge clk) begin
	if(enable) begin
		outputValue <= memory[addr];
	end
end



endmodule 