// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * RISCVSoc
 * rainbain
 * 6/26/2023
 * Orca Emulator
 *
 * Simple Button input GPIO.
 * Note: No input filtering for the metastate.
*/

module SystemButtonGPIO(
  input wire clk, input wire resetn,
  
  input wire read, output wire[7:0] readData,
  
  input wire[7:0] buttons
);

assign readData = read ? buttons : 0;

endmodule