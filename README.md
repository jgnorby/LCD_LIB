# LCD_LIB
LCD interfacing in 4 bit mode on a FRDM-KL46Z microcontroller.
This is part of an ongoing project for the CSE325 Embedded Systems course.

The print function is used as follows:
 
 		void print(unsigned char *val, int pos)
 
Reads in the characters of a message and takes in the cursor position.
To write on the top line, write a 1. For the bottom, write a 2.
 
Example:	
			print("hello",1);
 			print("world",2);
Output:		
			hello
 			world
 				
The compiler might yell at you if you just write a sentence in quotes.
I suggest creating an unsigned char * variable and pass in the variable like so:

		unsigned char *message = "write message here";
		print(message,1);