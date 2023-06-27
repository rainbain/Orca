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
		MATRIX_DSP_X = a;
		
		GPIO_LEDS = 2;
		SystemAPIReadWord(&a);
		MATRIX_DSP_Y = a;
		
		GPIO_LEDS = 3;
		SystemAPIReadWord(&a);
		MATRIX_DSP_Z = a;
		
		
		GPIO_LEDS = 4;
		SystemAPIReadWord(&a);
		MATRIX_DSP_M00 = a;
		
		GPIO_LEDS = 5;
		SystemAPIReadWord(&a);
		MATRIX_DSP_M10 = a;
		
		GPIO_LEDS = 6;
		SystemAPIReadWord(&a);
		MATRIX_DSP_M01 = a;
		
		GPIO_LEDS = 7;
		SystemAPIReadWord(&a);
		MATRIX_DSP_M11 = a;
		
		GPIO_LEDS = 8;
		SystemAPIReadWord(&a);
		MATRIX_DSP_M02 = a;
		
		GPIO_LEDS = 9;
		SystemAPIReadWord(&a);
		MATRIX_DSP_M12 = a;
		
		GPIO_LEDS = 10;
		while(!API_RX_READY);
		a = UART_RXTX;
		MATRIX_DSP_OPERATION = a;
		
		a = MATRIX_DSP_OUT_X;
		SystemAPIWriteWord(&a);
		a = MATRIX_DSP_OUT_Y;
		SystemAPIWriteWord(&a);
		a = MATRIX_DSP_OUT_Z;
		SystemAPIWriteWord(&a);
	}
	
}
