// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * RISCVSoc
 * rainbain
 * 6/26/2023
 * Orca Emulator
 *
 * Simplue UART implmentation.
 * Note: No I/O FIFO, reading must be done fast.
*/

module SystemUART(
	input wire clk, input wire resetn,
	
	input wire[3:0] address,
	input wire read, output wire[31:0] readData,
	input wire write, input wire[31:0] writeData,
	output wire waitrequest,
	
	input wire RX, output wire TX
);

/*
 * Configuration Registers
*/

wire configFlagsSelect = address == 0;
wire configRXTXSelect = address == 8;
wire configClockDivSelect = address == 12;

reg[15:0] clockDivider;

reg[7:0] txData;
reg txWrite;

wire[1:0] uartFlags;
assign uartFlags[0] = txReady & ~txWrite;
assign uartFlags[1] = rxReady;

assign readData = read ? ((configClockDivSelect ? clockDivider : 0) | (configFlagsSelect ? uartFlags : 0) | (configRXTXSelect ? rxDataBuffer : 0)) : 0;

reg txActiveFound;
reg rxReady;

reg[7:0] rxGotten;

always @ (negedge clk) begin
  if(~resetn) begin
    clockDivider <= 0;
    txWrite <= 0;
	 txActiveFound <= 0;
	 rxReady <= 0;
  end else begin
    if(write & configClockDivSelect) clockDivider <= writeData;
	 
	 if(write & configRXTXSelect & uartFlags[0]) begin
	   txData <= writeData;
		 txWrite <= 1;
	 end
	 
	 if(txWrite & ~txReady) begin
	   txWrite <= 0;
	 end
	 
	 if(read & configRXTXSelect & rxReady) rxReady <= 0;
	 
	 if(~rxReady) begin
	   if(txActiveFound) begin
		  if(~rxActive) begin
		    rxReady <= 1;
			 txActiveFound <= 0;
		  end
		end else begin
		  txActiveFound <= rxActive;
		end
	 end
  end
end

assign waitrequest = 0;


/*
 * Clock Gen
*/

reg dividerStage1;
reg dividerStage2;

always @ (negedge clk) begin
  dividerStage1 <= ~dividerStage1;
end

always @ (negedge dividerStage1) begin
  dividerStage2 <= ~dividerStage2;
end


/*
 * TX
*/

reg txBitClock;
reg[15:0] txBitTimmer;

always @ (negedge dividerStage2) begin
  if(txBitTimmer > clockDivider) begin
    txBitTimmer <= 0;
	 txBitClock <= ~txBitClock;
  end else begin
    txBitTimmer <= txBitTimmer + 1;
  end
end

wire txReady;


wire[9:0] outputBitMapper;
assign outputBitMapper[0] = 1; // Default State
assign outputBitMapper[1] = 0; // Start Bit
assign outputBitMapper[2] = txData[0];
assign outputBitMapper[3] = txData[1];
assign outputBitMapper[4] = txData[2];
assign outputBitMapper[5] = txData[3];
assign outputBitMapper[6] = txData[4];
assign outputBitMapper[7] = txData[5];
assign outputBitMapper[8] = txData[6];
assign outputBitMapper[9] = txData[7];

reg[3:0] bitState;
assign TX = outputBitMapper[bitState];

reg active;
assign txReady = ~active;

always @ (negedge txBitClock) begin
  if(~resetn) begin
    bitState <= 0;
	 active <= 0;
  end else begin
    if(active) begin
	   if(bitState >= 9) begin
		  bitState <= 0;
		  active <= 0;
		end else begin
		  bitState <= bitState + 1;
		end
	 end else begin
	   if(txWrite) begin
		  active <= 1;
		  bitState <= 1; // Remove 1 cycle start bit inter latency
		end
	 end
  end
end


/*
 * RX
*/

reg rxActive;
reg[3:0] rxBitState;
reg[15:0] rxBitTimmer;
reg[7:0] rxData;
reg[7:0] rxDataBuffer;

reg clockEdgeDetector;

reg metaStableFix;
reg demetaRX;

always @ (negedge dividerStage2) begin
  if(~resetn) begin
    rxActive <= 0;
  end else begin
    if(rxActive) begin
	   if(rxBitTimmer > clockDivider) begin
		  clockEdgeDetector <= ~clockEdgeDetector;
		  rxBitTimmer <= 0;
		  
		  if(clockEdgeDetector) begin
		    if(rxBitState >= 9) begin
		      rxActive <= 0;
				rxDataBuffer <= rxData;
		    end
		  
		    rxBitState <= rxBitState + 1;
			 
		    
		    if(rxBitState > 0 & rxBitState < 9) begin
		      rxData[rxBitState-1] <= demetaRX;
		    end
		  end
		end else begin
		  rxBitTimmer <= rxBitTimmer + 1;
		end
	 end else begin
	   if(~demetaRX) begin
		  rxActive <= 1;
		  rxBitState <= 0;
		  rxBitTimmer <= clockDivider/2;
		  clockEdgeDetector <= 1;
		end
	 end
  end
  
  metaStableFix <= RX;
  demetaRX <= metaStableFix;
end

endmodule