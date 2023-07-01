#include "soc.h"

#include "SystemAPI.h"

SystemAPIState systemAPIState;

void main()  {
	// 1000 times per second
	TIMMER_DIVIDER = SYSTEM_CLOCK_SPEED / TIMMER_PRESCALER / 1000 - 1;
	
	// Set Serial
	UART_CLOCK_DIV = UART_CALCULATE_BITRATE(115200);
	
	uint32_t a;
	while(1){
		GPIO_LEDS = 1;
		SystemAPIReadWord(&a);
		NORMAL_DSP_NM00 = a;
		GPIO_LEDS = 2;
		SystemAPIReadWord(&a);
		NORMAL_DSP_NM10 = a;
		GPIO_LEDS = 3;
		SystemAPIReadWord(&a);
		NORMAL_DSP_NM20 = a;
		
		GPIO_LEDS = 4;
		SystemAPIReadWord(&a);
		NORMAL_DSP_NM01 = a;
		GPIO_LEDS = 5;
		SystemAPIReadWord(&a);
		NORMAL_DSP_NM11 = a;
		GPIO_LEDS = 6;
		SystemAPIReadWord(&a);
		NORMAL_DSP_NM21 = a;
		
		GPIO_LEDS = 7;
		SystemAPIReadWord(&a);
		NORMAL_DSP_NM02 = a;
		GPIO_LEDS = 8;
		SystemAPIReadWord(&a);
		NORMAL_DSP_NM12 = a;
		GPIO_LEDS = 9;
		SystemAPIReadWord(&a);
		NORMAL_DSP_NM22 = a;
		
		
		
		GPIO_LEDS = 10;
		SystemAPIReadWord(&a);
		NORMAL_DSP_NNX = a;
		GPIO_LEDS = 11;
		SystemAPIReadWord(&a);
		NORMAL_DSP_NNY = a;
		GPIO_LEDS = 12;
		SystemAPIReadWord(&a);
		NORMAL_DSP_NNZ = a;
		
		GPIO_LEDS = 13;
		SystemAPIReadWord(&a);
		NORMAL_DSP_NTX = a;
		GPIO_LEDS = 14;
		SystemAPIReadWord(&a);
		NORMAL_DSP_NTY = a;
		GPIO_LEDS = 15;
		SystemAPIReadWord(&a);
		NORMAL_DSP_NTZ = a;
		
		GPIO_LEDS = 16;
		SystemAPIReadWord(&a);
		NORMAL_DSP_NBX = a;
		GPIO_LEDS = 17;
		SystemAPIReadWord(&a);
		NORMAL_DSP_NBY = a;
		GPIO_LEDS = 18;
		SystemAPIReadWord(&a);
		NORMAL_DSP_NBZ = a;
		
		
		
		GPIO_LEDS = 19;
		while(!API_RX_READY);
		a = UART_RXTX;
		NORMAL_DSP_OP = a;
		
		a = NORMAL_DSP_ONX;
		SystemAPIWriteWord(&a);
		a = NORMAL_DSP_ONY;
		SystemAPIWriteWord(&a);
		a = NORMAL_DSP_ONZ;
		SystemAPIWriteWord(&a);
		
		a = NORMAL_DSP_OTX;
		SystemAPIWriteWord(&a);
		a = NORMAL_DSP_OTY;
		SystemAPIWriteWord(&a);
		a = NORMAL_DSP_OTZ;
		SystemAPIWriteWord(&a);
		
		a = NORMAL_DSP_OBX;
		SystemAPIWriteWord(&a);
		a = NORMAL_DSP_OBY;
		SystemAPIWriteWord(&a);
		a = NORMAL_DSP_OBZ;
		SystemAPIWriteWord(&a);
	}
	
}
