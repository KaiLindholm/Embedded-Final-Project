/*
 * main.c
 *
 * Created: 4/25/2023 4:43:45 PM
 *  Author: James Ostrowski and Kai Lindholm
 */ 

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "Timer1.h"
#define DELAY1 1600
#define DELAY2 8000
#define DELAY3 16000

int main(void)
{
	pwm_init();
	sei();
	
	DDRB &= 0xFE;
	DDRB &= 0xFE;
	PORTC = (1 << PORTC0);
	PORTB = (1 << PORTB0);
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
}