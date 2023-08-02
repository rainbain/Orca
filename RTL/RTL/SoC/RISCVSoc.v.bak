// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * RISCVSoc
 * rainbain
 * 6/26/2023
 * Orca Emulator
 *
 * A simple RISC-V SoC built around picorv32.
*/

module RISCVSoc(
  //
  // Top Level
  //
  
  input wire clk, input wire resetn,
  
  //
  // IO
  //
  
  output wire[7:0] leds, input wire[7:0] buttons,
  
  input wire UART_RX, output wire UART_TX
);


/*
 * System Bus
*/

wire[31:0] SystemReadData;
wire[31:0] SystemWriteData;
wire[31:0] SystemAddress;
wire[3:0] SystemByteEnable;

wire SystemRead;
wire SystemWrite;
wire SystemWaitRequest;


/*
 * Address Decoder
*/

wire[27:0] SystemMemoryAddress = SystemAddress[27:0];
wire[3:0] SystemAddressSelector = SystemAddress[31:28];

wire SystemROMEnable = SystemAddressSelector == 0;
wire SystemStackRAMEnable = SystemAddressSelector == 1;
wire SystemHardwareEnable = SystemAddressSelector == 2;
wire SystemUserRAMEnable = SystemAddressSelector == 3;


/*
 * ROM
*/

wire[31:0] SystemROMReadData;

SystemROM rom(
	.clk(clk),
	
	.address(SystemMemoryAddress),
	.readData(SystemROMReadData),
	.read(SystemRead & SystemROMEnable)
);


/*
 * Stack RAM
*/

wire[31:0] SystemStackRAMReadData;

SystemRam #(
  .SIZE(4095)
) stackRam(
	.clk(clk),
	
	.address(SystemMemoryAddress),
	.readData(SystemStackRAMReadData), .writeData(SystemWriteData),
	.read(SystemRead & SystemStackRAMEnable), .write(SystemWrite & SystemStackRAMEnable),
	.byteenable(SystemByteEnable)
);

/*
 * User RAM
*/

wire[31:0] SystemUserRAMReadData;

SystemRam #(
   .SIZE(8191)
) userRam(
	.clk(clk),
	
	.address(SystemMemoryAddress),
	.readData(SystemUserRAMReadData), .writeData(SystemWriteData),
	.read(SystemRead & SystemUserRAMEnable), .write(SystemWrite & SystemUserRAMEnable),
	.byteenable(SystemByteEnable)
);


/*
 * Hardware Address Decoder
*/

wire[19:0] hardwareAddress = SystemMemoryAddress[19:0];
wire[7:0] hardwareAddressSelector = SystemMemoryAddress[27:20];

wire SystemGPIOEnable = hardwareAddressSelector == 0 & SystemHardwareEnable;
wire SystemUARTEnable = hardwareAddressSelector == 1 & SystemHardwareEnable;
wire SystemTimmerEnable = hardwareAddressSelector == 2 & SystemHardwareEnable;
wire SystemMatrixDSPEnable = hardwareAddressSelector == 3 & SystemHardwareEnable;
wire SystemFloatDSPEnable = hardwareAddressSelector == 4 & SystemHardwareEnable;


/*
 * LED GPIO
*/

wire[7:0] SystemLEDGPIOReadData;

wire SystemLEDGPIOEnable = SystemGPIOEnable & hardwareAddress == 0;

SystemLEDGPIO ledgpio(
	.clk(clk), .resetn(resetn),
	
	.write(SystemWrite & SystemLEDGPIOEnable), .writeData(SystemWriteData),
	.read(SystemRead & SystemLEDGPIOEnable), .readData(SystemLEDGPIOReadData),
	.leds(leds)
);

/*
 * Button GPIO
*/

wire[7:0] SystemButtonGPIOReadData;

wire SystemButtonGPIOEnable = SystemGPIOEnable & hardwareAddress == 4;

SystemButtonGPIO buttonGPIO(
	.clk(clk), .resetn(resetn),
	
	.read(SystemRead & SystemButtonGPIOEnable), .readData(SystemButtonGPIOReadData),
	.buttons(buttons)
);


/*
 * UART
*/

wire[31:0] SystemUARTReadData;
wire SystemUARTWaitRequest;

SystemUART uart(
	.clk(clk), .resetn(resetn),
	
	.address(hardwareAddress),
	.read(SystemRead & SystemUARTEnable), .readData(SystemUARTReadData),
	.write(SystemWrite & SystemUARTEnable), .writeData(SystemWriteData),
	.waitrequest(SystemUARTWaitRequest),
	.RX(UART_RX), .TX(UART_TX)
);

/*
 * Timmer
*/

wire[31:0] SystemTimmerReadData;

SystemTimmer timmer(
	.clk(clk), .resetn(resetn),
	
	.write(SystemWrite & SystemTimmerEnable), .read(SystemRead & SystemTimmerEnable),
	.writeData(SystemWriteData), .readData(SystemTimmerReadData),
	.address(hardwareAddress)
);

/*
 * Matrix DSP Test Core SystemMatrixDSPEnable
*/

wire[31:0] SystemMatrixDSPReadData;
wire SystemMatrixDSPWaitRequest;

MatrixDSPCore mdspip(
	.clk(clk), .resetn(resetn),
	
	.address(hardwareAddress), .read(SystemMatrixDSPEnable & SystemRead), .write(SystemMatrixDSPEnable & SystemWrite),
	.readData(SystemMatrixDSPReadData), .writeData(SystemWriteData),
	
	.waitrequest(SystemMatrixDSPWaitRequest)
);

/*
 * Floating Point DSP Test Core SystemFloatDSPEnable
*/

wire[31:0] SystemFloatDSPReadData;

DspCore floatDSP(
	.clk(~clk), .resetn(resetn),
	.read(SystemFloatDSPEnable & SystemRead), .write(SystemFloatDSPEnable & SystemWrite),
	.readdata(SystemFloatDSPReadData), .writedata(SystemWriteData),
	.address(hardwareAddress)
);



/*
 * System Input Combination
*/

assign SystemReadData = SystemROMReadData | SystemStackRAMReadData | SystemLEDGPIOReadData | SystemUARTReadData | SystemUserRAMReadData | SystemTimmerReadData | SystemButtonGPIOReadData | SystemMatrixDSPReadData | SystemFloatDSPReadData;
assign SystemWaitRequest = SystemUARTWaitRequest | SystemMatrixDSPWaitRequest;


/*
 * RISC-V
*/

wire riscVMemValid;
wire riscVMemReady;

assign riscVMemReady = ~SystemWaitRequest;
wire riscVIsReadAction = SystemByteEnable == 0;

assign SystemRead = riscVMemValid & riscVIsReadAction;
assign SystemWrite = riscVMemValid & (~riscVIsReadAction);

picorv32 #(
	.STACKADDR(32'h 1000_2000)
) riscVCore(
	.clk(clk),
	.resetn(resetn),
	.mem_valid(riscVMemValid),
	.mem_ready(riscVMemReady),
	.mem_addr(SystemAddress),
	.mem_wdata(SystemWriteData),
	.mem_wstrb(SystemByteEnable),
	.mem_rdata(SystemReadData)
);

endmodule 