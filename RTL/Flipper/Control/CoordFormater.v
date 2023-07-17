// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * CoordFormater
 * rainbain
 * 7/12/2023
 * Orca Emulator
 *
 * Formats coordnates of a given 3 bit format into floats.
*/

module CoordFormater(
    //
    // Top Level
    //

    input wire clk, input wire resetn,
    input wire start, output wire valid,


    //
    // Data In/Out
    //

    input wire[31:0] dataIn, input wire[2:0] format,
    output wire[31:0] dataOut,

    wire[4:0] shiftIn,
);

//
// Format Decoder and Float Passthrough
//
wire FORMAT_U8    = format == 0;
wire FORMAT_S8    = format == 1;
wire FORMAT_U16   = format == 2;
wire FORMAT_S16   = format == 3;
wire FORMAT_FLOAT = format == 4;

reg convertValid;
wire convertStart;
wire[31:0] convertData;

assign valid = convertValid | (FORMAT_FLOAT & start);
assign convertStart = (~FORMAT_FLOAT) & start;
assign dataOut = FORMAT_FLOAT ? dataIn : convertData;


//
// Output State. Takes an extra cycle.
//
reg cycleCounter;

always @ (posedge clk) begin
    if(~resetn) begin
        cycleCounter <= 0;
        convertValid <= 0;
    end else begin
        if(cycleCounter) begin
            convertValid <= 1;
            cycleCounter <= 0;
        end else begin
            cycleCounter <= convertStart;
            convertValid <= 0;
        end
    end
end


//
// Converter Logic
//

wire[15:0] inputMasked = (FORMAT_S8 | FORMAT_U8) ? dataIn[7:0] : dataIn[15:0];

wire inputSigned = FORMAT_S8 ? dataIn[7] :
                   FORMAT_S16 ? dataIn[15] : 0;

wire[15:0] inputMagnitude = dataIn[15:0] ^ (inputSigned ? 16'hFFFF : 16'h0000);

wire[4:0] bitCount = inputMasked[15] + inputMasked[14] + inputMasked[13] + inputMasked[12] +
                     inputMasked[11] + inputMasked[10] + inputMasked[9] + inputMasked[8] +
                     inputMasked[7] + inputMasked[6] + inputMasked[5] + inputMasked[4] +
                     inputMasked[3] + inputMasked[2] + inputMasked[1] + inputMasked[0];

wire[4:0] bitPower = inputMasked[15] ? 15 : inputMasked[14] ? 14 : inputMasked[13] ? 13 :
                     inputMasked[12] ? 12 : inputMasked[11] ? 11 : inputMasked[10] ? 10 :
                     inputMasked[9] ? 9 : inputMasked[8] ? 8 : inputMasked[7] ? 7 :
                     inputMasked[6] ? 6 : inputMasked[5] ? 5 : inputMasked[4] ? 4 :
                     inputMasked[3] ? 3 : inputMasked[2] ? 2 : inputMasked[1] ? 1 : 0;

wire[7:0] powerLong;
assign powerLong[4:0] = bitPower;
assign powerLong[7:5] = 0;

wire[7:0] exponent = powerLong + 127 - shiftIn;

wire[22:0] mantisa = (inputMagnitude << (23-bitPower)) & 8388607;

assign convertData[31] = inputSigned;
assign convertData[30:23] = exponent;
assign convertData[22:0] = mantisa;

endmodule
