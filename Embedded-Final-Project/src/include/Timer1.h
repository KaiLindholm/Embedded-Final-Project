/*
 * Timer1.h
 *
 * Created: 4/25/2023 4:45:29 PM
 *  Author: James Ostrowski and Kai Lindholm 
 */ 


#ifndef TIMER1_H_
#define TIMER1_H_
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define PWM_TOP (39999U)
#define SERVO_MIN (1999U)
#define SERVO_MAX (4999U)

void pwm_init(void);
void pwm_sweep(void);
void servo_set(uint16_t deg,uint16_t max_deg);



#endif /* TIMER1_H_ */