/**
 * LCD_LIB.h
 *
 * @author Jennifer Norby
 * @version 10/28/2019
 * Project 4 - Library Development
 * LCD interfacing in 4 bit mode
 */

#ifndef LCD_LIB_H_
#define LCD_LIB_H_
	void delay(unsigned int n);
	void EN();
	void setup();
	void clear();
	void cmd(unsigned char val);
	void send(unsigned char val);
	void data(unsigned char val);
	void setCursor(int pos, int loc);
	void print(unsigned char *val);
	char *dtostrf (double val, signed char width, unsigned char prec, char *sout);
	void lcd_Init();
#endif /* LCD_LIB_H_ */
