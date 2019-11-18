/**
 * LCD_LIB.c
 *
 * @author Jennifer Norby
 * @version 10/28/2019
 * Project 4 - Library Development
 * LCD interfacing in 4 bit mode
 */

#include <stdio.h>
#include "LCD_LIB.h"
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL46Z4.h"
#include "fsl_debug_console.h"

/*
 * Constant definitions
 */
#define LCD_SET 0x30	// manually set instructions to change modes
#define LCD_4BIT 0x20	// 4 bit mode select
#define LCD_2LINE 0x28	// 4 bit mode again, 2 line display
#define LCD_DISP 0x0F	// display on, cursor on, blinking on
#define LCD_CLEAR 0x01			// display clear
#define LCD_INCR 0x06			// entry mode set to increment
#define RET 0x02		// return home

/*
 * Start of Function definitions
 */
/*
 * delay_ms():
 * A delay function in millis
 */
void delay_ms(unsigned int n){
	unsigned int i = 0;
	unsigned int j;
	for(i=0; i<n*2000; i++){
		j++;
	}
}

/*
 * EN():
 *  Enables data read & write when high.
 */
void EN() {
    GPIOD->PDOR &= ~(1 << 2); 	// off
    delay_ms(1);
    GPIOD->PDOR |= (1 << 2); 	// on
    delay_ms(1);
    GPIOD->PDOR &= ~(1 << 2); 	// off
    delay_ms(1);
}

/*
 * backLight():
 * 	Controls the back lighting for the LCD.
 */
void backLight(unsigned int state) {
	if(state == 0)
		GPIOD->PDOR	|= (1 << 4);	// OFF
	else
		GPIOD->PDOR	&= ~(1 << 4);	// ON
}

/*
 * setUp():
 * 	initializes the LCD by sending instructions
 * 	0x30 - Repeat 3 times to reset for 4 bit mode, passed into data to only send a nibble
 * 	0x20 - Sets it to 4 bit mode, passed in as a nibble.
 * 	0x28 - Sets it to 4 bit mode and selects 2 line display
 *	0x0C - Turns on display. Can also turn on the cursor and make it blink with 0x0F.
 *	0x01 - Clears the display
 *	0x02 - Returns home
 */
void setup(){
	data(0x30);
	delay_ms(5);
	data(0x30);
	delay_ms(5);
	data(0x30);
	delay_ms(5);

	data(0x20);
	cmd(0x28);
	delay_ms(1);
	cmd(0x0C);
	delay_ms(1);
	cmd(0x01);
	delay_ms(3);
	cmd(0x02);
	delay_ms(3);
}

/*
 * clear():
 * 	Clears the display
 */
void clear() {
	cmd(0x01);
	delay_ms(10);
}

/*
 * cmd():
 * 	Takes in a value, selects the instruction register by setting RS to low,
 * 	passes value into data to calculate the 2 sets of nibbles.
 */
void cmd(unsigned char val) {
	GPIOA->PDOR &= ~(1 << 13);	//rs low

	data(val&0xF0);			// first nibble
	data((val<<4)&0xF0);	// second nibble obtained by left shifting
}

/*
 * send():
 * 	Takes in a value to send, selects the data register by setting RS to high,
 * 	passes value into data to calculate the 2 sets of nibbles.
 */
void send(unsigned char val) {
	GPIOA->PDOR |= (1 << 13);
	data(val&0xF0);			// first nibble
	data((val<<4)&0xF0);	// second nibble obtained by left shifting
	GPIOA->PDOR &= ~(1 << 13);	//rs low
}

/*
 * data():
 * 	Reads in a byte and compares it to each bit.
 */
void data(unsigned char val) {

	// Bit 7
	if(val&0x80)
	    GPIOC->PDOR	|= (1 << 9);	// ON
	else
	    GPIOC->PDOR	&= ~(1 << 9);	// OFF

	// Bit 6
	if(val&0x40)
	    GPIOC->PDOR	|= (1 << 8);	// ON
	else
	    GPIOC->PDOR	&= ~(1 << 8);	// OFF

	// Bit 5
	if(val&0x20)
	    GPIOA->PDOR	|= (1 << 5);	// ON
	else
	    GPIOA->PDOR	&= ~(1 << 5);	// OFF

	// Bit 4
	if(val&0x10)
	    GPIOA->PDOR	|= (1 << 4);	// ON
	else
	    GPIOA->PDOR	&= ~(1 << 4);	// OFF
	EN();
}

/*
 * print():
 *	Reads in the characters of a message and takes in the cursor position.
 *	For the top line, write a 1. For the bottom, write a 2.
 *	Example:	print("hello",1);
 *				print("world",2);
 *	Output:		hello
 *				world
 */
void print(unsigned char *val, int pos) {
	if(pos==1) {
		cmd(0x80);	// for top line
	}else if(pos==2){
		cmd(0xC0);	// for bottom line
	}

	delay_ms(1000);

    unsigned int length = strlen((const char*)val);	// length of the char string

	for (int i = 0; i < length; i++){
		unsigned char singleChar = val[i];
		send(singleChar);
	}
	delay_ms(1000);
}

/*
 * lcdInit():
 * Enables clock gating for all ports and initializes all pins for the LCD
 *
 */
void lcd_Init() {
	SIM->SCGC5 |= (1<<9) | (1<<11) | (1<<12);	// enables clock gating: PORTA, PORTC, PORTD

	// LCD EN - D9
    PORTD->PCR[2] &= ~0x700;	// Init clear of port a register 13
    PORTD->PCR[2] |= 0x700 & (1 << 8);		// Drive pin with TPM0

	// LCD RS - D8
    PORTA->PCR[13] &= ~0x700;	// Init clear of port a register 13
    PORTA->PCR[13] |= 0x700 & (1 << 8);	// Set MUX bits

	// LCD D4
    PORTA->PCR[4] &= ~0x700;	// Init clear of port a register  4
    PORTA->PCR[4] |= 0x700 & (1 << 8);	// Set MUX bits

	// LCD D5
    PORTA->PCR[5] &= ~0x700;	// Init clear of port a register  5
    PORTA->PCR[5] |= 0x700 & (1 << 8);	// Set MUX bits

	// LCD D6
    PORTC->PCR[8] &= ~0x700;	// Init clear of port c register  8
    PORTC->PCR[8] |= 0x700 & (1 << 8);	// Set MUX bits

	// LCD D7
    PORTC->PCR[9] &= ~0x700;	// Init clear of port c register  9
    PORTC->PCR[9] |= 0x700 & (1 << 8);	// Set MUX bits

	// LCD K - D10 - Pin connected to the back light.
    PORTD->PCR[4] &= ~0x700;	// Init clear of port d register 2
    PORTD->PCR[4] |= 0x700 & (1 << 8);	// Set MUX bits, enable pullups

	// K - Turns on the backlight
	GPIOD->PDDR	|= (1 << 4);	// sets portd pin 4 to output - LCD K
	GPIOD->PDOR |= (1 << 4);	// sets state

	// initializing, function set 4 bit operation
    GPIOA->PDDR |= (1 << 13);	// sets porta pin 13 to LOW	 - LCD RS
    GPIOD->PDDR |= (1 << 2); 	// enable

    GPIOC->PDDR	|= (1 << 9);	// sets to portc pin 9 to LOW	 - LCD D7
    GPIOC->PDDR	|= (1 << 8);	// sets to portc pin 8 to LOW	 - LCD D6
    GPIOA->PDDR	|= (1 << 5);	// sets to porta pin 5 to LOW	 - LCD D5
    GPIOA->PDDR	|= (1 << 4);	// sets to porta pin 4 to LOW	 - LCD D4

	delay_ms(50);

    GPIOA->PDOR &= ~(1 << 13);	// sets porta pin 13 to LOW	 - LCD RS
    GPIOD->PDOR &= ~(1 << 2); 	// enable
}
