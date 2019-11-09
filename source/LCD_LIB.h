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
	void delay_ms(unsigned int n);
	void EN();
	void backLight(unsigned int state);
	void setUp();
	void clear();
	void cmd(unsigned char val);
	void send(unsigned char val);
	void data(unsigned char val);
	void msg(unsigned char *val);
	void topWrite(unsigned char *message);
	void btmWrite(unsigned char *message);
	void lcd_Init();
#endif /* LCD_LIB_H_ */
