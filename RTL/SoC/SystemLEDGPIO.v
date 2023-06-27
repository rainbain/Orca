// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * RISCVSoc
 * rainbain
 * 6/26/2023
 * Orca Emulator
 *
 * Simple 8 bit LED output.
*/

module SystemLEDGPIO(
  input wire clk, input wire resetn,
  
  input wire write, input wire[7:0] writeData,
  input wire read, output wire[7:0] readData,
  
  output reg[7:0] leds
);

assign readData = read ? leds : 0;

always @ (negedge clk) begin
  if(~resetn) begin
    leds <= 0;
  end else begin
    if(write) begin
	   leds <= writeData;
	 end
  end
end
endmodule 