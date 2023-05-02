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

#include "config.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "Timer1.h"
#define DELAY1 1600
#define DELAY2 8000
#define DELAY3 16000

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
	/* initialize peripherals */
	lcd_init();
	spi_init();
	mfrc522_init();
	pwm_init();
	/* Set up pins the PWM Servo Motor */
	DDRB &= 0xFE;
	DDRB &= 0xFE;
	PORTC = (1 << PORTC0);
	PORTB = (1 << PORTB0);
}
int main (void) {
	uint8_t byte;
	init();
	sei();
	servo_set(150,180);
	int16_t count = 0;
	int16_t delay = 0;

	while(1)
	{
		if(count == 0){
			//Welcome Display
			count++;
			_delay_ms(1000);
		}
		if((PINC & (1 << PINC0))== 0){
			if(count == 1){
				delay = 1600;
				_delay_ms(1000);
				//1 Gram 
			}else if(count == 2){
				//5 Grams
				delay = 8000;;
				_delay_ms(1000);
			}else if(count == 3){
				//10 grams
				delay = 16000;
				_delay_ms(1000);
				count = 0;
			}
			count++;
		}
		if((PINB & (1 << PINB0)) == 0){
			if((count == 0) && (delay == 0)){
				;
			}else{
				_delay_ms(1000);
				servo_set(105,150);
				if(delay == 1600){
					_delay_ms(DELAY1);
				}else if(delay == 8000){
					_delay_ms(DELAY2);
				}else if(delay == 16000){
					_delay_ms(DELAY3);
				}
				servo_set(150,180);
			}
			
		}
	}

	return 0;
}
