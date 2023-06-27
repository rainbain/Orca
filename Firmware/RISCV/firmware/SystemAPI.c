#include "SystemAPI.h"

void SystemAPIReadWord(uint32_t *word){
	while(!API_RX_READY);
	((uint8_t*)word)[3] = UART_RXTX;
	while(!API_RX_READY);
	((uint8_t*)word)[2] = UART_RXTX;
	while(!API_RX_READY);
	((uint8_t*)word)[1] = UART_RXTX;
	while(!API_RX_READY);
	((uint8_t*)word)[0] = UART_RXTX;
}

// Writes a 4 byte word with crc
void SystemAPIWriteWord(uint32_t *word){
	while(!API_TX_READY);
	UART_RXTX = ((uint8_t*)word)[3];
	while(!API_TX_READY);
	UART_RXTX = ((uint8_t*)word)[2];
	while(!API_TX_READY);
	UART_RXTX = ((uint8_t*)word)[1];
	while(!API_TX_READY);
	UART_RXTX = ((uint8_t*)word)[0];
}