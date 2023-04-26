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

int main(void)
{
	pwm_init();
	sei();
	//servo_set(0,180);
	int16_t i = 0;
	
    while(1)
    {
		for(i = 0; i <= 50; i++){
			servo_set(i,50);
			//_delay_ms(1);
		}
		for(i = 50; i >= 0; i--){
			servo_set(i,50);
			//_delay_ms(1);

		}
    }
}