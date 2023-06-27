// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * RISCVSoc
 * rainbain
 * 6/26/2023
 * Orca Emulator
 *
 * Reads up to 4 rows of a matrix with ready signals via a 128 bit matrix port.
 * Used to pipeline matrix data in XF operations.
*/

module MatrixPort #(
	parameter ROWS = 3
)(
	input wire clk, input wire resetn,
	                        
	//
	// Matrix Memory Port
	//
	
	output wire[6:0] mpAddr, output reg mpEnable,
	input wire[127:0] mpData, input wire mpValid,   
	
	
	//
	// Matrix
	//
	
	input wire[6:0] mtAddr, output reg mtDone,
	input wire mtCycle,
	
	output wire[127:0] matrixA,
	output wire[127:0] matrixB,
	output wire[127:0] matrixC,
	output wire[127:0] matrixD
);

//
// Selectable Matrix
//

reg[127:0] outputMatrix[0:ROWS];
assign matrixA = outputMatrix[0];
assign matrixB = outputMatrix[1];
assign matrixC = outputMatrix[2];
assign matrixD = outputMatrix[3];

//
// State System
//

reg active;
reg nextActive;

reg[1:0] mtGetterRow;
reg[1:0] mtSetterRow;

assign mpAddr = mtAddr + mtGetterRow;

always @* begin            
	// Defaults
	mpEnable = 1;
	nextActive = active;
	
	case(active)
		0: begin // Idle
			mpEnable = mtCycle;
			if(mtCycle)
				nextActive = 1;				
		end
		1: begin // Active
			if(mtGetterRow >= (ROWS-1))
				nextActive = 0;
		end
	endcase
end

// Getter Logic
always @ (posedge clk) begin
	if(mpEnable) begin
		if(mtGetterRow >= (ROWS-1)) begin
			mtGetterRow <= 0;
			mtDone <= 1;
		end else begin
			mtGetterRow <= mtGetterRow + 1;
			mtDone <= 0;
		end
	end else begin
		mtGetterRow <= 0;
		mtDone <= 0;
	end
end

// Setter Logic
always @ (posedge clk) begin
	if(mpValid) begin
		if(mtSetterRow >= (ROWS-1)) 
			mtSetterRow <= 0;
		end else begin
			mtSetterRow <= mtSetterRow + 1;
		end
		
		outputMatrix[mtSetterRow] <= mpData;
	end else begin
		mtSetterRow <= 0;
	end
end

// State iterator
always @ (posedge clk) begin
	if(~resetn) begin
		active <= 0;
	end else begin
		active <= nextActive;
	end
end

endmodule