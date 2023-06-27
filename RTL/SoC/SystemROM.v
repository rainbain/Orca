// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * RISCVSoc
 * rainbain
 * 6/26/2023
 * Orca Emulator
 *
 * ROM implmentation for Altera FPGAs using infered memory.
*/

module SystemROM(
	input wire clk,
	
	input wire[27:0] address,
	output wire[31:0] readData,
	input wire read
);

reg[31:0] memory[0:4096];

initial begin
	$readmemh("firmware/bootloader.mem", memory);
end

reg[31:0] readBuffer;
assign readData = read ? readBuffer : 0;

wire[25:0] addressShortend = address[27:2];

always @ (negedge clk) begin
  if(read) begin
    readBuffer <= memory[addressShortend];
  end
end

endmodule