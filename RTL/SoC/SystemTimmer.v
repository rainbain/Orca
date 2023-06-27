// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * RISCVSoc
 * rainbain
 * 6/26/2023
 * Orca Emulator
 *
 * Simple low speed 32 bit timmer with 21 bit period.
*/

module SystemTimmer(
	input wire clk, input wire resetn,
	
	input wire write, input wire read,
	input wire[31:0] writeData, output wire[31:0] readData,
	input wire[2:0] address
);

wire CONFIG_TIMMER = address == 0;
wire CONFIG_DIVIDER = address == 4;

reg[20:0] dividerTimmer;
reg[20:0] dividerTimmerPeriod;

reg[31:0] timmer;

assign readData = read ? ((CONFIG_TIMMER ? timmer : 0) | (CONFIG_DIVIDER ? dividerTimmerPeriod : 0)) : 0;

reg[5:0] prescaler;

always @ (negedge clk) begin
  prescaler[0] <= ~prescaler[0];
  
  if(write & CONFIG_DIVIDER) dividerTimmerPeriod <= writeData;
end

always @ (negedge prescaler[0]) prescaler[1] <= ~prescaler[1];
always @ (negedge prescaler[1]) prescaler[2] <= ~prescaler[2];
always @ (negedge prescaler[2]) prescaler[3] <= ~prescaler[3];
always @ (negedge prescaler[3]) prescaler[4] <= ~prescaler[4];
always @ (negedge prescaler[4]) prescaler[5] <= ~prescaler[5];

always @ (negedge prescaler[5]) begin
  if(dividerTimmer > dividerTimmerPeriod) begin
    dividerTimmer <= 0;
	 timmer <= timmer + 1;
  end else begin
    dividerTimmer <= dividerTimmer + 1;
  end
end

endmodule 