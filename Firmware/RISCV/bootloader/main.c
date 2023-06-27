#include "soc.h"

void uartPrint(const char *str){
	while(*str != 0){
		while((UART_FLAGS & UART_FLAG_BIT_TX_READY) == 0);
		UART_RXTX = *str;
		str++;
	}
}

void main()  {
	// 1000 times per second
	TIMMER_DIVIDER = SYSTEM_CLOCK_SPEED / TIMMER_PRESCALER / 1000 - 1;
	
	// Set Serial
	UART_CLOCK_DIV = UART_CALCULATE_BITRATE(115200);
	
	// Send NACKs every second waiting for ACK. LED display to show alive
ON_TRANSMITION_FAIL:
	GPIO_LEDS = 1;
	uint32_t nextNACK = TIMMER_PERIOD + 1000;
	uint8_t ledDirection = 0;
	while(1) {
		if((UART_FLAGS & UART_FLAG_BIT_RX_READY) > 0){
			break;
		}
		if((GPIO_BUTTONS & GPIO_BUTTONS_KEY1) == 0){
			goto EXIT;
		}
		if(TIMMER_PERIOD > nextNACK){
			nextNACK = TIMMER_PERIOD + 1000;
			if(ledDirection)
				GPIO_LEDS >>= 1;
			else
				GPIO_LEDS <<= 1;
			if(GPIO_LEDS == 0x80) ledDirection = 1;
			if(GPIO_LEDS == 0x01) ledDirection = 0;
			
			// I dont expect it to be busy.
			UART_RXTX = 21; // NACK
		}
	}
	
	uint8_t* userMemory = (uint8_t*)0x30000000;
	
	uint8_t header;
	uint8_t packetNumber;
	uint8_t complementNumber;
	uint8_t checksum;
	uint8_t realChecksum;
	
	uint8_t i;
	
	uint8_t expectedPacketNumber = 1;
	
	while(1){
		// SOH
		while((UART_FLAGS & UART_FLAG_BIT_RX_READY) == 0);
		header = UART_RXTX;
		
		// Some Checks
		if(header == 4) break; //End of transmittion
		if(header != 1) {
			UART_RXTX = 21;
			continue;
		}
		
		// Header data
		while((UART_FLAGS & UART_FLAG_BIT_RX_READY) == 0);
		packetNumber = UART_RXTX;
		while((UART_FLAGS & UART_FLAG_BIT_RX_READY) == 0);
		complementNumber = UART_RXTX;
		
		// Data
		for(i = 0; i < 128; i++){
			while((UART_FLAGS & UART_FLAG_BIT_RX_READY) == 0);
			userMemory[i] = UART_RXTX;
		}
		
		// Checksum
		while((UART_FLAGS & UART_FLAG_BIT_RX_READY) == 0);
		checksum = UART_RXTX;
		
		// Compute checksum
		realChecksum = 0;
		for(i = 0; i < 128; i++){
			realChecksum += userMemory[i];
		}
		
		// Do the proper checks
		if((checksum != realChecksum) || (packetNumber != expectedPacketNumber) || ((255-packetNumber) != complementNumber)){
			UART_RXTX = 21;
			continue;
		}
		
		// Iterate counters
		userMemory += 128;
		expectedPacketNumber++;
		
		// ACK (I dont expect it to be busy)
		UART_RXTX = 6;
	}
EXIT:
	((void (*)())0x30000000)();
		GPIO_LEDS = 0xFF;
	
	asm volatile (
        "li sp, 0x10002000\n\t"     // Set stack pointer to 0x10002000
    );
}
