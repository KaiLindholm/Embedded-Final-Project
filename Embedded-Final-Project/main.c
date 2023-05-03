/** 
 * ---------------------------------------------------+ 
 * @desc        Main file
 * ---------------------------------------------------+ 
 * @author      Kai Lindholm & James Ostrowski
 * @file        main.c
 * @version     1.0
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
#define DELAY1 1300
#define DELAY2 6500
#define DELAY3 14400

#include "lcd.h"
#include "mfrc522.h"
#include "hd44780.h"
#include <avr/io.h>
#include <util/delay.h>
#include <lcd.h>
#include <utils.h>
#include <spi.h>
#include <mfrc522.h>
#include "uart.h"
#include <string.h>


uint8_t  cardTag[] = {0xAF, 0x51, 0xDA, 0x02};
uint8_t keyTag[] = {0x40, 0xF8, 0x8C, 0x1E};
	
uint8_t Uids[2][4];

void printByte(uint8_t number);
void selfTest();
void sense_card();
uint8_t read_card();
void build_uid(uint8_t* uid);
uint8_t check_if_uid_in_memory(uint8_t * scannedUid);\
void dispenser();
FILE lcd_str = FDEV_SETUP_STREAM(lcd_putchar, NULL, _FDEV_SETUP_WRITE);

void init(){
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

	// load stored UIDs from memory. 
	memcpy(Uids[0], cardTag, sizeof(cardTag));
	memcpy(Uids[1], keyTag, sizeof(keyTag));
	sei();
	
	servo_set(150,180);
}
void printByte(uint8_t byte) {
	char hex[3];

	sprintf(hex, "%x", byte);
	uart_send_string(hex);
}

void selfTest() {
	const uint8_t MFRC522_firmware_referenceV2_0[] = {
		0x00, 0xEB, 0x66, 0xBA, 0x57, 0xBF, 0x23, 0x95,
		0xD0, 0xE3, 0x0D, 0x3D, 0x27, 0x89, 0x5C, 0xDE,
		0x9D, 0x3B, 0xA7, 0x00, 0x21, 0x5B, 0x89, 0x82,
		0x51, 0x3A, 0xEB, 0x02, 0x0C, 0xA5, 0x00, 0x49,
		0x7C, 0x84, 0x4D, 0xB3, 0xCC, 0xD2, 0x1B, 0x81,
		0x5D, 0x48, 0x76, 0xD5, 0x71, 0x61, 0x21, 0xA9,
		0x86, 0x96, 0x83, 0x38, 0xCF, 0x9D, 0x5B, 0x6D,
		0xDC, 0x15, 0xBA, 0x3E, 0x7D, 0x95, 0x3B, 0x2F
	};
	uint8_t SelfTestBuffer[64];
	// perform a soft reset of the mfrc522
	mfrc522_reset();
	
	// clear the internal buffer by writing 25 0x00 bytes and implement the config command
	for(int i = 0; i < 25; ++i){
		mfrc522_write(FIFODataReg, 0x00);
	}
	
	mfrc522_write(CommandReg,Mem_CMD);
	//enable the self test by writing 0x09 to the auto testreg
	mfrc522_write(AutoTestReg, 0x09);
	//write 0x00 to the FIFO buffer
	mfrc522_write(FIFODataReg, 0x00);
	// start test with the CalcCRC cmd
	mfrc522_write(CommandReg, CalcCRC_CMD);
	_delay_ms(500);
	for(uint8_t byte = 0; byte < 64; ++byte){
		SelfTestBuffer[byte] = mfrc522_read(FIFODataReg);
		if( byte % 8 == 0 && byte != 0){
			uart_send_byte('\n');
		}
		printByte(SelfTestBuffer[byte]);
		uart_send_byte(' ');
		if(SelfTestBuffer[byte] != MFRC522_firmware_referenceV2_0[byte]){
			uart_send_string("Incorrect Version");
		}
	
	}
	uart_send_string("\nMFRC522 is version 2.0");
	
	mfrc522_init();
}

void sense_card(){
	uint8_t byte = mfrc522_read(ComIEnReg);
	mfrc522_write(ComIEnReg,byte | 0x20);
	byte = mfrc522_read(DivIEnReg);
	mfrc522_write(DivIEnReg,byte | 0x80);
		uint8_t str[MAX_LEN];
	while(1){
		byte = mfrc522_request(PICC_REQALL, str);
		printByte(byte);
		uart_send_byte('\n');
		_delay_ms(1000);
	}
}

int main (void) {
	init();
	dispenser();
}
void goToSecondLine(){
	hd44780_wait_ready(true);
	hd44780_outcmd(HD44780_DDADDR(0x40));
}
void dispenser(){
	int16_t count = 0;
	int16_t delay = 0;
	
	fprintf(&lcd_str, "Scan your card.");
	while(!read_card());
	
	count++;
	_delay_ms(1250);
	fprintf(&lcd_str, "\n\n\n");
	fprintf(&lcd_str, "\n\n\n");
				
	fprintf(&lcd_str, "Use the top PB.");
	goToSecondLine();
	fprintf(&lcd_str, "to cycle amounts");
	
	while (1) {
		if((PINC & (1 << PINC0))== 0){
			if(count == 1){
				delay = 1600;
				fprintf(&lcd_str, "\n");
				fprintf(&lcd_str, "\n");
				fprintf(&lcd_str, "Pour 1g Creatine");
				goToSecondLine();
				fprintf(&lcd_str, "Press 2nd button");
				_delay_ms(200);
			} else if(count == 2){
				fprintf(&lcd_str, "\n");
				fprintf(&lcd_str, "\n");
				fprintf(&lcd_str, "Pour 5g Creatine");
				goToSecondLine();
				fprintf(&lcd_str, "Press 2nd button");
				delay = 8000;
				_delay_ms(200);
				}else if(count == 3){
				fprintf(&lcd_str, "\n");
				fprintf(&lcd_str, "\n");
				fprintf(&lcd_str, "Pour 10g Creatine");
				goToSecondLine();
				fprintf(&lcd_str, "Press 2nd button");
				delay = 16000;
				_delay_ms(200);
				count = 0;
			}
			
			count++;
			
		}
		
	
		if( (PINB & (1 << PINB0)) == 0 ) {
			if((count != 0) && (delay != 0)){
				_delay_ms(1000);
				servo_set(105,150);
				if(delay == 1600) {
					_delay_ms(DELAY1);
					} else if(delay == 8000){
					_delay_ms(DELAY2);
					} else if(delay == 16000){
					_delay_ms(DELAY3);
				}
				
				servo_set(150,180);
				fprintf(&lcd_str, "\n\n\n");
				fprintf(&lcd_str, "\n\n\n");
				dispenser();
			}
		}
	}
}

uint8_t read_card(){
	uint8_t byte; 
	uint8_t str[MAX_LEN];
	
	byte = mfrc522_read(ComIEnReg);
	mfrc522_write(ComIEnReg,byte|0x20);
	byte = mfrc522_read(DivIEnReg);
	mfrc522_write(DivIEnReg,byte|0x80);
	
	while(1){
		byte = mfrc522_request(PICC_REQALL, str);
		if(byte == CARD_FOUND) {
			byte = mfrc522_get_card_serial(str);
			if(byte == CARD_FOUND) {
				if(check_if_uid_in_memory(str)){
					goToSecondLine();
					fprintf(&lcd_str, "\n\nAccess Granted");
					return 1; 
				} else {
					fprintf(&lcd_str, "\n\nAccess Denied");
					return 0; 
				}
			} else {
				return 0; 
			}
		}
		
		_delay_ms(1000);
	}
}

void build_uid(uint8_t* uid){
	for(uint8_t byte = 0; byte < 4; ++byte) {
		printByte(uid[byte]);
		uart_send_byte(' ');
	}
	uart_send_byte('\n');
}

uint8_t check_if_uid_in_memory(uint8_t * scannedUid) 
{
	
	for(int i = 0; i < sizeof(Uids); ++i){
		uint8_t * uid = Uids[i];
		if(memcmp(scannedUid, uid, 4) == 0){
			return 1;
		}
	}
	return 0; 
}

