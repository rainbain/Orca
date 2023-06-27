// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * RISCVSoc
 * rainbain
 * 6/26/2023
 * Orca Emulator
 *
 * RAM implmentation for Altera FPGAs using infered memory.
*/

module SystemRam #(
	parameter [31:0] SIZE = 32'h 0000_300
) (
	input wire clk,
	
	input wire[27:0] address,
	output wire[31:0] readData, input wire[31:0] writeData,
	input wire read, input wire write,
	input wire[3:0] byteenable
	
);

reg[7:0] memoryA[0:SIZE];
reg[7:0] memoryB[0:SIZE];
reg[7:0] memoryC[0:SIZE];
reg[7:0] memoryD[0:SIZE];

reg[31:0] readBuffer;
assign readData = read ? readBuffer : 0;

wire[25:0] addressShortend = address[27:2];

always @ (negedge clk) begin
  if(read) begin
    readBuffer <= (memoryA[addressShortend] << 24) | (memoryB[addressShortend] << 16) | (memoryC[addressShortend] << 8) | (memoryD[addressShortend]);
  end else if(write) begin
    if(byteenable[3]) memoryA[addressShortend] <= writeData[31:24];
	 if(byteenable[2]) memoryB[addressShortend] <= writeData[23:16];
	 if(byteenable[1]) memoryC[addressShortend] <= writeData[15:8];
	 if(byteenable[0]) memoryD[addressShortend] <= writeData[7:0];
  end
end

endmodule