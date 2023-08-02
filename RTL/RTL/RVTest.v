// Copyright 2023 Orca Hardware Emulator
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * RVTest
 * rainbain
 * 6/26/2023
 * Orca Emulator
 *
 * Top level logic to the real time RISC-V controlled test system.
*/

module RVTest(
	input wire clk50Mhz, input wire[1:0] buttons,
	
	output wire[7:0] leds,
	
   /*
	 *
	 * Orca Super Speed Board
	 *
	*/
	
	//
	// FT601Q 32b FIFO
	//
	
	output wire fifoWakeup, output wire fifoReset, output wire fifoSIWU, output wire[1:0] fifoIOSelect,
	output wire fifoOE, output wire fifoRD, output wire fifoWR, input wire fifoCLK, input wire fifoRXF, input wire fifoTXE,
	inout wire[31:0] fifoData, output wire[3:0] fifoByteEnable,
	
	//
	// IS66WVH8M8 HyperRAM
	//
	
	output wire hyperReset, output wire hyperRWDS, output wire hyperCS, output wire hyperCK, output wire hyperCKN,
	inout wire[7:0] hyperData,
	
	//
	// UART
	//
	
	input wire UART_RX, output wire UART_TX
);

/*
 * Basic Connections
*/

wire systemClock = clk50Mhz;
wire resetn = buttons[0];


/*
 * RISC-V SoC
*/

wire[7:0] socButtons;
assign socButtons[0] = buttons[1];

RISCVSoc riscvsoc(
  .clk(systemClock), .resetn(resetn),
  
  .leds(leds), .buttons(socButtons),
  
  .UART_RX(UART_RX), .UART_TX(UART_TX)
);

endmodule 