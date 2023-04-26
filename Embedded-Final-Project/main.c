/** 
 * ---------------------------------------------------+ 
 * @desc        Main file
 * ---------------------------------------------------+ 
 * @copyright   Copyright (C) 2020 Marian Hrinko.
 * @author      Marian Hrinko
 * @email       mato.hrinko@gmail.com
 * @datum       10.11.2020
 * @update      15.04.2021
 * @file        main.c
 * @version     1.0
 * @tested      AVR Atmega16a
 * ---------------------------------------------------+
 */

// include libraries
#define F_CPU 16000000UL
#include "defines.h"

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include <util/delay.h>

#include "lcd.h"
#include "mfrc522.h"

/**
 * @desc    Main function
 *
 * @param   void
 * @return  int
 */

FILE lcd_str = FDEV_SETUP_STREAM(lcd_putchar, NULL, _FDEV_SETUP_WRITE);
static void init(){
	lcd_init();
	spi_init();
	mfrc522_init();
	
}
int main (void) {
	uint8_t byte;
	
	
	init();
	fprintf(&lcd_str, "RFID Reader");
	_delay_ms(1000);
	fprintf(&lcd_str, "");
	byte = mfrc522_read(VersionReg);
	if(byte == 0x92){
		fprintf(&lcd_str, "MIFARE RC522");
		fprintf(&lcd_str, "Detected");
	} else if(byte == 0x91 || byte == 0x90){
		fprintf(&lcd_str, "MIFARE RC522");
		fprintf(&lcd_str, "Detected");
	} else {
		fprintf(&lcd_str, "No reader found");

	}
	while(1);
	return 0;
}