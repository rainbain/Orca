#include <stdio.h>
#include <string.h>

#include "PositionTransformMicrocode.h"
#include "NormalTransformMicrocode.h"
#include "TexcoordTransformMicrocode.h"

void DisplayMicrocode(const uint16_t *mc, uint8_t size){
	for(uint8_t i = 0; i < size; i++){
		printf("%04x\n", mc[i] & 0xFFFF);
	}
}

int main(int argc, char *argv[]){
	if(argc != 2){
		printf("Invalid arguments!\n");
	}
	
	const char * cmd = argv[1];
	
	if(strcmp(cmd, "PositionTransformMicrocodeA") == 0){
		DisplayMicrocode(PositionTransformMicrocodeA, sizeof(PositionTransformMicrocodeA) / sizeof(uint16_t));
	}
	if(strcmp(cmd, "PositionTransformMicrocodeB") == 0){
		DisplayMicrocode(PositionTransformMicrocodeB, sizeof(PositionTransformMicrocodeB) / sizeof(uint16_t));
	}
	if(strcmp(cmd, "PositionTransformMicrocodeC") == 0){
		DisplayMicrocode(PositionTransformMicrocodeC, sizeof(PositionTransformMicrocodeC) / sizeof(uint16_t));
	}
	
	if(strcmp(cmd, "NormalTransformMicrocodeA") == 0){
		DisplayMicrocode(NormalTransformMicrocodeA, sizeof(NormalTransformMicrocodeA) / sizeof(uint16_t));
	}
	if(strcmp(cmd, "NormalTransformMicrocodeB") == 0){
		DisplayMicrocode(NormalTransformMicrocodeB, sizeof(NormalTransformMicrocodeB) / sizeof(uint16_t));
	}
	if(strcmp(cmd, "NormalTransformMicrocodeC") == 0){
		DisplayMicrocode(NormalTransformMicrocodeC, sizeof(NormalTransformMicrocodeC) / sizeof(uint16_t));
	}
	
	if(strcmp(cmd, "TexcoordTransformMicrocodeA") == 0){
		DisplayMicrocode(TexcoordTransformMicrocodeA, sizeof(TexcoordTransformMicrocodeA) / sizeof(uint16_t));
	}
	if(strcmp(cmd, "TexcoordTransformMicrocodeB") == 0){
		DisplayMicrocode(TexcoordTransformMicrocodeB, sizeof(TexcoordTransformMicrocodeB) / sizeof(uint16_t));
	}
	if(strcmp(cmd, "TexcoordTransformMicrocodeC") == 0){
		DisplayMicrocode(TexcoordTransformMicrocodeC, sizeof(TexcoordTransformMicrocodeC) / sizeof(uint16_t));
	}
	return 0;
}