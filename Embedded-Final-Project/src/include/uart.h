/*
 * uart.h
 *
 * Created: 4/11/2023 9:20:27 PM
 *  Author:  Kai Lindholm & James Ostrowski
 */ 


#ifndef MY_UART_H_
#define MY_UART_H_

#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "config.h"
#define BUFFER_SIZE 128
#define MAX_COMMAND_STR_SIZE 60

typedef struct UARTBuffer {
	uint8_t buffer[BUFFER_SIZE];  // buffer of size BUFFER_SIZE initially all zeros
	uint16_t size;      // the amount of data in the buffer.
	uint16_t head;		// head is the front of the buffer queue. wraps around to
	uint16_t tail;		// tail is incremented and where data is read from.
 } UARTBuffer;

extern UARTBuffer uart_buffer;

void uart_init(uint32_t baud);
void uart_send_byte(char byte);
void uart_send_string(char *arr);

char * uart_get_string();
uint8_t uart_string_complete();

uint16_t uart_get_buffer_size(void);
uint8_t uart_read_buffer(void);

char * uart_get_command(void);

#endif /* MY_UART_H_ */