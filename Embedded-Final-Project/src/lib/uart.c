/*
 * uart.c
 *
 * Created: 4/11/2023 9:19:17 PM
 *  Author:  Kai Lindholm & James Ostrowski
 */ 
#include "uart.h"
#define MAX_BUFFER_SIZE 128
#define MAX_COMMAND_STR_SIZE 60

volatile static uint8_t uart_tx_busy = 1;
volatile static uint8_t uart_fetch_complete = 0;		// when the buffer has reached a '\n' char flag set to 1. once the string has been read from flag set to 0. 
UARTBuffer uart_buffer; 

ISR(USART_RX_vect){
	uart_buffer.buffer[uart_buffer.head] = UDR0;			 // read in the UDR0 register
	uart_buffer.head++;
	uart_buffer.size++;
	
	if (uart_buffer.head >= MAX_BUFFER_SIZE){		// if the rx position overruns the rx max buffer size. return rx pos to 0. "Circular FIFO data structure as stated in the datasheet"
		uart_buffer.head = 0;
	}
	
}

ISR(USART_TX_vect){
	uart_tx_busy = 1;
}

uint8_t uart_string_complete(){
	return uart_fetch_complete;
}

void uart_init(uint32_t baud){
	/*Set baud rate*/
	baud = F_CPU/(16*baud) - 1;
	UBRR0H = (baud & 0x0F00) >> 8;	// mask out the upper 4 bits
	UBRR0L = (baud & 0x00FF);		// mask out the lower 8 bits
	
	/* Enable Rx and Tx as well as Rx and Rx complete interrupts*/
	UCSR0B |= (1<< RXEN0) | (1<<TXEN0) | (1<< RXCIE0) | (1 << TXCIE0);
	
	/* Initialize Objects */
	uart_buffer = (UARTBuffer){ .buffer = {0}, .head = 0, .tail = 0};		
}


void uart_send_byte(char data){
	while(uart_tx_busy == 0); // while UART is not transmitting
	
	uart_tx_busy = 0; 
	UDR0 = data;				// load byte passed into the function to the uart buffer
}

void uart_send_string(char *arr) {
	uint16_t i = 0; 
	
	do {
		uart_send_byte(arr[i]);
		i++;
	} while(arr[i] != '\0');
}

uint16_t uart_get_buffer_size(void){
	return uart_buffer.size;
}

uint8_t uart_read_buffer(void){
	uint8_t data = uart_buffer.buffer[uart_buffer.tail];		// pull the data out of the buffer at the tail.
	uart_buffer.tail++;								// increment the position of the tail
	uart_buffer.size--; 							// decrement the size of the queue.

	if(uart_buffer.tail >= MAX_BUFFER_SIZE){		// reading has gone past the buffers size wrap the tail to the start.
		uart_buffer.tail = 0;
	}
	
	return data;
}