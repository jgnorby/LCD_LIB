/**
 * LCD_LIB.c
 *
 * @author Jennifer Norby
 * @version 11/12/2019
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
 * Start of Function definitions
 */
/*
 * delay():
 * A delay function in millis
 */
void delay(unsigned int n){
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
void EN(){
    GPIOD->PDOR &= ~(1 << 2); 	// off
    delay(1);
    GPIOD->PDOR |= (1 << 2); 	// on
    delay(1);
    GPIOD->PDOR &= ~(1 << 2); 	// off
    delay(1);
}

/*
 * setup():
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
	delay(5);
	data(0x30);
	delay(5);
	data(0x30);
	delay(5);

	data(0x20);
	cmd(0x28);
	delay(1);
	cmd(0x0C);
	delay(1);
	cmd(0x01);
	delay(3);
	cmd(0x02);
	delay(3);
}

/*
 * clear():
 * 	Clears the entire display
 */
void clear(){
	cmd(0x01);
	delay(10);
}

/*
 * cmd():
 * 	Takes in a value, selects the instruction register by setting RS to low,
 * 	passes value into data to calculate the 2 nibbles.
 * 	Example:	cmd(0x01); will clear the display
 */
void cmd(unsigned char val){
	GPIOA->PDOR &= ~(1 << 13);	//rs low

	data(val&0xF0);			// first nibble
	data((val<<4)&0xF0);	// second nibble obtained by left shifting
}

/*
 * send():
 * 	Takes in a value to send, selects the data register by setting RS to high,
 * 	passes value into data to calculate the 2 nibbles, then sets RS low.
 * 	Example:	send('H'); will print the letter H to the display
 */
void send(unsigned char val){
	GPIOA->PDOR |= (1 << 13);
	data(val&0xF0);			// first nibble
	data((val<<4)&0xF0);	// second nibble obtained by left shifting
	GPIOA->PDOR &= ~(1 << 13);	//rs low
}

/*
 * data():
 * 	Reads in a byte, but only compares the left nibble to each bit.
 * 	Example:	data(0xF0); will drive all 4 pins HIGH
 * 				data(0x0F);	will drive all 4 pins LOW
 */
void data(unsigned char val){
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
void print(unsigned char *val, int pos){
	if(pos==1){
		cmd(0x80);		// for top line
	}else if(pos==2){
		cmd(0xC0);		// for bottom line
	}
	delay(50);			// give it time to register the command before writing to the screen

    unsigned int length = strlen((const char*)val);	// length of the char string

	for (int i = 0; i < length; i++){
		unsigned char singleChar = val[i];
		send(singleChar);
	}
	delay(1000);
}

/*
 * dtostrf
 * A function that converts double to string
 */
char *dtostrf (double val, signed char width, unsigned char prec, char *sout) {
  uint32_t leftDec = (uint32_t)val;
  uint32_t rightDec = (uint32_t)((val - (double)leftDec) * pow(10, prec));

  sprintf(sout, "%d.%d", leftDec, rightDec);
  return sout;
}

/*
 * lcdInit():
 * Enables clock gating for all ports and initializes all pins for the LCD
 *
 */
void lcd_Init() {
	SIM->SCGC5 |= (1<<9) | (1<<10) | (1<<11) | (1<<12);	// enables clock gating: PORTA, PORTC, PORTD

	// LCD EN - D9
    PORTD->PCR[2] &= ~0x700;	// Init clear of port a register 13
    PORTD->PCR[2] |= 0x700 & (1 << 8);	// Set MUX bits

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
    PORTD->PCR[4] &= ~0x700;	// Init clear of port d register 4
    PORTD->PCR[4] |= 0x700 & (1 << 8);	// Set MUX bits, enable pullups

	// K - Turns on the backlight
	GPIOD->PDDR	|= (1 << 4);	// sets portd pin 4 to output - LCD K
	GPIOD->PDOR |= (1 << 4);	// sets state

	// initializes all pins to output
    GPIOA->PDDR |= (1 << 13);	// sets porta pin 13 to output	 - LCD RS
    GPIOD->PDDR |= (1 << 2); 	// enable

    GPIOC->PDDR	|= (1 << 9);	// sets portc pin 9 to output	 - LCD D7
    GPIOC->PDDR	|= (1 << 8);	// sets portc pin 8 to output	 - LCD D6
    GPIOA->PDDR	|= (1 << 5);	// sets porta pin 5 to output	 - LCD D5
    GPIOA->PDDR	|= (1 << 4);	// sets porta pin 4 to output	 - LCD D4

	delay(50);

    GPIOA->PDOR &= ~(1 << 13);	// sets porta pin 13 to LOW	 - LCD RS
    GPIOD->PDOR &= ~(1 << 2); 	// sets enable to LOW
}
