// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * TransformUnit
 * rainbain
 * 7/9/2023
 * Orca Emulator
 *
 * XF's Transform Unit.
*/

module TransformUnit (
    //
	// Top Level
	//
	
	input wire clk, input wire resetn,

    //
    // CP Bus
    //

    input wire[11:0] CPAddr,
    input wire[31:0] CPWriteData,

    input wire CPWriteMatrix
);

/*
 * Matrix Memory Decoder
*/

wire[9:0] matrixAddress = CPAddr[9:0];
wire[1:0] matrixSelector = CPAddr[11:10];

wire CpPosMatrixWrite   = (matrixSelector == 0) & CPWriteMatrix;
wire CpNormMatrixWrite  = (matrixSelector == 1) & CPWriteMatrix;
wire CpPostMatrixWrite  = (matrixSelector == 2) & CPWriteMatrix;
wire CpLightMatrixWrite = (matrixSelector == 3) & CPWriteMatrix;

/*
 * Position Matrix
*/

wire[6:0] posmatAAddr = 0;
wire posmatAEnable = 0;
wire[127:0] posmatAData;
wire posmatAValid;

PositionMatrix positionMatrix(
    .clk(clk), .resetn(resetn),

    .CPAddr(matrixAddress[8:0]), .CPWrite(CpPosMatrixWrite),
    .CPWriteData(CPWriteData),

    .posmatAAddr(posmatAAddr), .posmatAEnable(posmatAEnable),
    .posmatAData(posmatAData), .posmatAValid(posmatAValid)
);

endmodule
