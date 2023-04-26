/*
 * Timer1.c
 *
 * Created: 4/25/2023 4:45:47 PM
 *  Author: James Ostrowski and Kai Lindholm 
 */ 

#include "Timer1.h"

volatile static uint8_t update_pwm_ready = 0;

ISR(TIMER1_OVF_vect){
	
	update_pwm_ready = 0;
}

ISR(TIMER1_COMPA_vect){
	
}

void pwm_init(void){
	DDRB |= (1 << DDB1);
	
	TIMSK1 = (1 << TOIE1) | (1 << OCIE1A);
	
	ICR1H = (PWM_TOP & 0xFF00) >> 8;
	ICR1L = (PWM_TOP & 0x00FF);
	
	OCR1AH = (SERVO_MIN & 0xFF00) >> 8;
	OCR1AL = (SERVO_MIN & 0x00FF);
	
	TCCR1A = (0b10 << COM1A0) | (0b00 << COM1B0) | (0b10 << WGM10);
	
	TCCR1B = (0b11 << WGM12) | (0b010 << CS10);
}


static void update_pwm(uint16_t i){
	update_pwm_ready = 1;
	while(update_pwm_ready != 0);
	OCR1AH = (i & 0xFF00) >> 8;
	OCR1AL = (i & 0x00FF);
}

void pwm_sweep(void){
	uint16_t i = 0;
	for(i = SERVO_MIN; i <= SERVO_MAX;i=+50){
		update_pwm(i);
		_delay_ms(40);
	}
	for(i = SERVO_MAX; i >= SERVO_MIN;i-=50){
		update_pwm(i);
		_delay_ms(40);
	}
}


void servo_set(uint16_t deg,uint16_t max_deg){
	
	float set = (float)deg / (float)max_deg;
	
	set = (((float)SERVO_MAX-(float)SERVO_MIN)*set) + (float)SERVO_MIN;
	
	uint16_t point = (uint16_t)set;
	
	update_pwm(point);
	
}

