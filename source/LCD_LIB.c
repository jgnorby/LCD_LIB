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
#define RS_HIGH GPIOA->PDDR|=(1 << 13)	// Register Select HIGH - Data Register
#define RS_LOW 	GPIOA->PDDR&=~(1 << 13)	// Register Select LOW - Instruction Register
//#define EN_HIGH TPM0->CONTROLS[2].CnV=7999	// Enable signal HIGH - Starts the data read & write
//#define EN_LOW 	TPM0->CONTROLS[2].CnV=0		// Enable signal LOW
#define HIGH 1
#define LOW 0

/*
 * Start of Function definitions
 */
/*
 * delay_ms():
 * A delay function in millis
 */
void delay_ms(unsigned int n)
{
	unsigned int i = 0;
	unsigned int j;
	for(i=0; i<n*2000; i++)
	{
		j++;
	}
}

/*
 * EN():
 *  Enables data read & write when high.
 */
void EN() {
	TPM0->CONTROLS[2].CnV=7999;
	delay_ms(50);
	TPM0->CONTROLS[2].CnV=0;
}

/*
 * backLight():
 * 	Controls the back lighting for the LCD.
 */
void backLight(unsigned int state) {
	if(state == 0)
		GPIOD->PDDR	|= (1 << 4);	// OFF
	else
		GPIOD->PDDR	&= ~(1 << 4);	// ON
}

/*
 * setUp():
 * 	initializes the LCD by sending instructions
 * 	0x28 - Sets it to 4 bit operation and selects 2 line display
 *	0x0E - Turns on display and cursor
 *	0x06 - Sets mode to increment address by 1 and shifts cursor to the right
 */
void setUp(){
	unsigned char val = 0;
	unsigned char initLCD[8]={0x03, 0x03, 0x03, 0x28, 0x0E, 0x06};

	while(initLCD[val]){
		cmd(initLCD[val]);
		delay_ms(5);
		val++;
	}
}

/*
 * clear():
 * 	Clears the display
 */
void clear() {
	cmd(0x01);
	delay_ms(50);
}

/*
 * cmd():
 * 	Takes in a value, selects the instruction register by setting RS to low,
 * 	passes value into data to calculate the 2 sets of nibbles.
 */
void cmd(unsigned char val) {
	RS_LOW;	// receive commands

	data(val&0xF0);			// first nibble
	EN();					// a call to the enable function, that starts the data read/write
	data((val<<4)&0xF0);	// second nibble obtained by left shifting
	EN();
}

/*
 * send():
 * 	Takes in a value to send, selects the data register by setting RS to high,
 * 	passes value into data to calculate the 2 sets of nibbles.
 */
void send(unsigned char val) {
	RS_HIGH;

	data(val&0xF0);			// first nibble
	EN();					// a call to the enable function, that starts the data read/write
	data((val<<4)&0xF0);	// second nibble obtained by left shifting
	EN();
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
}

/*
 * msg():
 *	Reads in the characters of a message, sends each character to the send function
 */
void msg(unsigned char *val) {
	while(*val) {
		send(*val);
		val++;
	}
}

/*
 * topWrite():
 *	Writes to the top line.
 */
void topWrite(unsigned char *message) {
	cmd(0x80);	// for top line
	msg(&message[0]);
}

/*
 * btmWrite():
 *
 */
void btmWrite(unsigned char *message) {
	cmd(0xC0);	// for bottom line
	msg(&message[0]);
}

/*
 * lcdInit():
 * Enables clock gating for all ports and initializes all pins for the LCD
 *
 */
void lcd_Init() {
	SIM->SCGC5 |= (1<<9) | (1<<11) | (1<<12);	// enables clock gating: PORTA, PORTC, PORTD
	// Setup PWM
	SIM->SCGC6 |= (1 << 24); // Clock Enable TPM0
	SIM->SOPT2 |= (0x2 << 24); // Set TPMSRC to OSCERCLK

	// Setup Channel 5
	TPM0->CONTROLS[2].CnSC |= (0x1 << 2) | (0x2 << 4);  // Edge PWM
	TPM0->MOD = 7999;  //

	delay_ms(50);
	// LCD EN - D9
    PORTD->PCR[2] &= ~0x400;	// Init clear of port a register 13
    PORTD->PCR[2] |= 0x400;		// Drive pin with TPM0

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

	// LCD K - D10 - Pin connected to the back light. Set to active low
    PORTD->PCR[4] &= ~0x703;	// Init clear of port d register 2
    PORTD->PCR[4] |= 0x703 & ((1 << 8) | 0x3);	// Set MUX bits, enable pullups

	// K - Turns on the backlight
	GPIOD->PDDR	&= ~(1 << 4);	// sets portd pin 4 to output - LCD K
	delay_ms(50);

	TPM0->SC |= 0x01 << 3; // Start the clock!

	// initializing, function set 4 bit operation
    GPIOA->PDDR	&= ~(1 << 13);	// sets porta pin 13 to LOW	 - LCD RS
    TPM0->CONTROLS[2].CnV=0;	// sets enable pin LOW

    setUp();	// initialization of LCD
    GPIOC->PDOR	&= ~(1 << 9);	// sets to portc pin 9 to LOW	 - LCD D7
    GPIOC->PDOR	&= ~(1 << 8);	// sets to portc pin 8 to LOW	 - LCD D6
    GPIOA->PDOR	|= (1 << 5);	// sets to porta pin 5 to HIGH	 - LCD D5
    GPIOA->PDOR	&= ~(1 << 4);	// sets to porta pin 4 to LOW	 - LCD D4
	delay_ms(10);

	cmd(0x0F);	// blink cursor
}

