// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * RISCVSoc
 * rainbain
 * 6/26/2023
 * Orca Emulator
 *
 * Memory mapped floating point DSP for testing its operation in real hardware.
*/

module DspCore(
	input wire clk, input wire resetn,
	
	input wire read, input wire write,
	output wire[31:0] readdata, input wire[31:0] writedata,
	
	input wire[4:0] address
);

reg[31:0] inputValues[0:1];
wire[31:0] outputValue;

reg dspClock;
reg start;
reg reset;
reg clocken;
reg[2:0] n;
wire done;

wire[31:0] configValue;
assign configValue[0] = dspClock;
assign configValue[1] = start;
assign configValue[2] = reset;
assign configValue[3] = clocken;
assign configValue[4] = n[0];
assign configValue[5] = n[1];
assign configValue[6] = n[2];
assign configValue[7] = done;

assign readdata = read ? (address[4:2] < 2 ? inputValues[address[4:2]] : address[4:2] == 2 ? configValue : resultHold) : 0;

always @ (posedge clk) begin
	if(~resetn) begin
		start <= 0;
		reset <= 0;
		dspClock <= 0;
	end else begin
		if(write & ~start) begin
		if(address[4:2] == 2) begin
			dspClock <= writedata[0];
			start <= writedata[1];
			reset <= writedata[2];
			clocken <= writedata[3];
			n <= writedata[6:4];
		end else begin
			inputValues[address[4:2]] <= writedata;
		end
		end else start <= 0;
	end
end

reg[31:0] resultHold;

always @ (posedge clk) begin
	if(done)resultHold <= outputValue;
end

FloatDSP fdsp(
	.clk(clk),
	.clk_en(1),
	.dataa(inputValues[0]),
	.datab(inputValues[1]),
	.n(n),
	.reset(~resetn),
	.reset_req(~resetn),
	.start(start),
	.done(done),
	.result(outputValue)
);

/*
module FloatDSP (
		input  wire        clk,       // s1.clk
		input  wire        clk_en,    //   .clk_en
		input  wire [31:0] dataa,     //   .dataa
		input  wire [31:0] datab,     //   .datab
		input  wire [2:0]  n,         //   .n
		input  wire        reset,     //   .reset
		input  wire        reset_req, //   .reset_req
		input  wire        start,     //   .start
		output wire        done,      //   .done
		output wire [31:0] result     //   .result
	);
*/

endmodule