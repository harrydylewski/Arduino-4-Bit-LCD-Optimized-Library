#pragma once

#include "avr_gpio_template.h"

#include <stdint.h>

const uint8_t DELAY40US = 40;

const uint8_t SELECT4BIT = 0x30;

//used with calculateCursorPosition to make sure the cursor
//goes the right location
enum LCDLAYOUT :uint8_t {LCD1602,LCD1604,LCD2002,LCD2004};

template<PINS EN, PINS RS, PINS d4, PINS d7, LCDLAYOUT colrow>
class LCD_4BIT_SP
{
	private:
	
	//
	LCDLAYOUT _layout=colrow;
		
	GPIO<EN> _en;
	GPIO<RS> _rs;

	PORT<d4,d7> _port;
	
	/*
	display register gets variable because multiple functions
	(blinking/display on / cursor on)
	are in the same register so it's difficult keep track of 
	what features are turned on and off without being explicit with all of them 
	I.E 
	cmd(DISPLAYON|BLINKON|CURSORFF)
	vs
	_displaycontrol &=~ CURSORON;
	cmd(_displaycontrol)
	*/
	uint8_t _displaycontrol=0x28; //display, blink, cursor are on by default
	
	void config4bit(void)
	{
		_en.on();
		_port.write(3); //write function set register
		_en.off();
		_delay_ms(3);
		
		_en.on();
		_port.write(3); //write function set register
		_en.off();
		_delay_ms(3);
		
		_en.on();
		_port.write(3); //write function set register
		_en.off();
		_delay_ms(3);
		
		_en.on();
		_port.write(2); //enable 4 bit mode
		_en.off();
		_delay_us(DELAY40US);

		_en.on();
		_port.write(2); //enable 4 bit mode
		_en.off();
		_delay_us(DELAY40US);		
		
		_en.on();
		_port.write(8); //enable two lines
		_en.off();
		_delay_us(DELAY40US);
		

		
	}
	
	uint8_t calculateCursorPosition(uint8_t column, uint8_t row)
	{
		uint8_t position=0x80;
		
		if(_layout==LCD1602||_layout==LCD2002)
		{
			if(row>=2)
				position|=0x40;		
					
			if(column>16&&(_layout==LCD1602))
				position|=0x0F;
			else if(column>20&&(_layout==LCD2002))
				position|=0x14;
			else
				position|=column;
		}
					
		else if(_layout==LCD1604)
		{
			if(row==2)
				position|=0x40;
			else if(row==3)
				position|=0x10;
			else if(row==4)
				position|=0x50;	
						
			if(column>16)
				position|=0x0F;
			else
				position|=column;
		}
			
		else
		{
			if(row==2)
				position|=0x40;
			else if(row==3)
				position|=0x14;
			else if(row==4)
				position|=0x54;		
					
			if(column>20)
				position|=20;
			else
				position|=column;
		}
			
			return position;
	}

	void write(uint8_t input)
	{
		_en.on();
		_port.write((input>>4)&0x0F);
		_en.off();
		//_delay_us(DELAY40US);
		
		_en.on();		
		_port.write(input&0x0F);
		_en.off();
		_delay_us(DELAY40US);
	}
	
	public:
		
	enum CMD : uint8_t {
	CLEAR=1,
	RETURN=2,
	//--------------------
	ENTRY_BIT=0x04,
	WRITELEFT=0x02,
	SHIFTON=0x01,
	//--------------------
	CURSOR_BIT   = 0x10,
	CURSORLEFT  = 0x10,
	CURSORRIGHT = 0x14,
	SHIFTLEFT   = 0x18,
	SHIFTRIGHT  = 0x1C,
	//----------------------
	DISPLAY_BIT  = 0x08,
	BLINKON      = 0x01,
	CURSORON     = 0x02,	
	DISPLAYON    = 0x04,
	
	DISPLAY_ALLON = 0x0F,
	//----------------------
	CGDRAM_BIT     = 0x40,
	DDRAM_BIT      = 0x80
	};
	
	LCD_4BIT()
	{
		_port.setOutput();
		_en.setOutput();
		_en.off();
		_rs.setOutput();
		
		config4bit();
		cmd(DISPLAY_ALLON);
	}
	
	enum LEAD : uint8_t {LEADLESS_LEFT,LEADLESS_RIGHT,LEADING_LEFT,LEADING_RIGHT};
	
	void print(int8_t * input)
	{
		_rs.on();
		for(uint8_t x=0 ;input[x]!='\0';x++)
		{
			write(input[x]);		
		}		
	}
	
	void print(const char * input)
	{
		_rs.on();
		for(uint8_t x=0 ;input[x]!='\0';x++)
		{
			write(input[x]);	
		}
	}
	
	void print(int8_t input)
	{	
		uint8_t unsigned_input = input;
		
		_rs.on();	
		
		/*
		if input is negative
		1)perform two's complement on unsigned version
		2)print out a negative sign
		*/
		
		if(input<0)
		{
			unsigned_input=(input^0xFF)+1;
			write('-');			
		}
		
		//print using unsigned vresion
		print(unsigned_input);		
	}
	
	void print(uint8_t input)
	{
		
		_rs.on();
		
		uint8_t output = 255;
		uint8_t totalzeroes = 0;
		
		/*
		For unsigned numbers
		subtract 100
		until MSB is clear
		*/
		
		while(input>=0x80)
		{
			input-=100;
			output++;
		}
		
		/*
		Auto Subtract 100 
		until MSB is clear from underflow
		*/
		
		do
		output++;
		while( !((input -= 100) & 0x80) );
		
		//if 0, do no print anything
		if(!output) 
			totalzeroes++;
			
		else
			write(output+'0');
		
		//---------------------------------						
		/*
		same as above
		now we add 10
		until MSB is set
		*/
		output = 10;	
		
		do
		output--;
		while( (input += 10) & 0x80 );
		
		//if 0, do no print anything
		if((!output)&&(totalzeroes))
			totalzeroes++;
		else
			write(output+'0');
			
		//least digit is always printed
		write(input+'0');	
		
		//print leading zeroes as spaces at the end
		while(totalzeroes--)					
		{
			write(' ');
		}
	}
	
	void print(uint16_t input)
	{
		_rs.on();
		
		uint8_t totalzeroes = 0;
		uint8_t output = 255;
		
		/*
		For unsigned numbers
		subtract 100000
		until MSB is clear
		*/
		
		while(input>=0x8000)
		{
			input-=10000;
			output++;
		}
		
		do
		output++;
		while( !((input -= 10000) & 0x8000) );
		
		if(!output)
			totalzeroes++;
		else
			write(output+'0');
		//1st digit printed
		
		//---------------------------------
		output = 10;	

		do
		output--;
		while( (input += 1000) & 0x8000 );
		if((!output)&&(totalzeroes))
			totalzeroes++;
		else
			write(output+'0');
		//2nd digit printed
		//---------------------------------	
		output = 255;
		
		do
		output ++;
		while( !((input -= 100) & 0x8000) );
		
		if((!output)&&(totalzeroes>1))
			totalzeroes++;
		else
			write(output+'0');
		//3rd digit printed
		//---------------------------------
		output = 10;
		
		do
		output--;
		while( (input += 10) & 0x8000 );
		
		if((!output)&&(totalzeroes>2))
			totalzeroes++;
		else
			write(output+'0');
		//4th digit printed
		//---------------------------------
		
		write(input+'0');
		//5th digit printed
		//---------------------------------
		while(totalzeroes--)
		{
			write(' ');
		}
	}
	
	//leadless expanding right
	
	void print(int16_t input)
	{
		uint16_t unsigned_input = input;
			
		//if the signed number is negative
		//do two's complement
				
		if(input<0)
		{
			unsigned_input  = (input^0xFFFF)+1;
			_rs.on();
			write('-');
		}				
		
		print(unsigned_input);		
	}
	
	void print(uint32_t input)
	{
		_rs.on();
		
		uint8_t totalzeroes=0; 	
		uint8_t output = 255;
		
		while(input>=0x80000000)
		{
			input -=1000000000;
			output++;
		}
			
		do
		output++;		//bill
		while( !((input -= 1000000000) & 0x80000000) );
		
		if(!output)
			totalzeroes++;
		else
			write(output+'0');
			
		//---------------------------------		
		output = 10;
		
		do
		output--;            //100 mill
		while( (input += 100000000) & 0x80000000 );
		
		if((!output)&&(totalzeroes))
			totalzeroes++;
		else
			write(output+'0');		
		//---------------------------------		
		output = 255;
		
		do
		output++;        //10 mill
		while( !((input -= 10000000) & 0x80000000) );
		
		if((!output)&&(totalzeroes>1))
			totalzeroes++;
		else
			write(output+'0');
					
		//---------------------------------		
		output = 10;
		
		do
		output--;            //mil
		while( (input += 1000000) & 0x80000000 );
		
		if((!output)&&(totalzeroes>2))
			totalzeroes++;
		else
			write(output+'0');
		
		//---------------------------------
		output = 255;
		
		do
		output++;        //100 thousand
		while( !((input -= 100000) & 0x80000000) );
		
		if((!output)&&(totalzeroes>3))
			totalzeroes++;
		else
			write(output+'0');
		
		//---------------------------------
		output = 10;
				
		do
		output--;            //10 thousand
		while( (input += 10000) & 0x80000000 );
		
		if((!output)&&(totalzeroes>4))
			totalzeroes++;
		else
			write(output+'0');
		//---------------------------------
		uint16_t casted_input = input;
		
		output = 255;
		
		do
		output++;         // thousand
		while( !((casted_input -= 1000) & 0x8000) );
		
		if((!output)&&(totalzeroes>5))
			totalzeroes++;
		else
			write(output+'0');
		//---------------------------------
		output = 10;
			
		do
		output--;          //hundred
		while( (casted_input += 100) & 0x8000 );
		
		if((!output)&&(totalzeroes>6))
			totalzeroes++;
		else
			write(output+'0');
	
		//---------------------------------		
		output = 255;		
		
		do
		output++;
		while( !((casted_input -= 10) & 0x8000) );
		casted_input+=10; //bug fix, leave it alone
		
		if((!output)&&(totalzeroes>7))
			totalzeroes++;
		else
			write(output+'0');
		
		write(casted_input+'0');
		
		while(totalzeroes--)		
			write(' ');
		
	}
	
	void print(int32_t input)
	{
		uint32_t unsigned_input = input;
			
		//if the signed number is negative
		//do two's complement
					
		if(input<0)
		{
			unsigned_input = (input^0xFFFF)+1;
			write('-');
		}
			
		print(unsigned_input);
	}
	
	//--------------------------------------------
	void cmd(enum CMD input)
	{
		_rs.off();
			
		write(input);	
	}
	
	void cmd(uint8_t input)
	{
		_rs.off();
			
		write(input);	
	}
	
	//---------------------------------------------
	/*
	  void autoscroll();
	  void noAutoscroll();
	  */
	
	void clear() {
		cmd(CLEAR);
		_delay_ms(3);
	}	
	
	void home()	{
		cmd(RETURN);  // set cursor position to zero
		_delay_ms(3);  // this command takes a long time!
	}
	//Display Register Related Functions------------
	
	void noDisplay() {
		_displaycontrol &= ~DISPLAYON;
		cmd(DISPLAY_BIT | _displaycontrol);
	}
	void display() {
		_displaycontrol |= DISPLAYON;
		cmd(DISPLAY_BIT | _displaycontrol);
	}
	
	void noCursor() {		
		_displaycontrol  &=~ CURSORON;
		cmd(DISPLAY_BIT  | _displaycontrol);
	}
	
	void cursor() {
		_displaycontrol  |= CURSORON;
		cmd(DISPLAY_BIT  | _displaycontrol);
	}
		
	void noBlink() {
		 _displaycontrol &= ~BLINKON;
		cmd(DISPLAY_BIT | _displaycontrol);
	}
	
	void blink() {
		_displaycontrol |= BLINKON;
		cmd(DISPLAY_BIT | _displaycontrol);
	}	

	//Cursor or shifting functions
	void scrollDisplayLeft(void) {
		cmd(SHIFTLEFT);
	}
	void scrollDisplayRight(void) {
		cmd(SHIFTRIGHT);
	}
		
	void cursorRight() {
		cmd(CURSORRIGHT);
	}
		
	void cursorLeft() {
		cmd(CURSORRIGHT);
	}
	//----------------------------------------
	
	void leftToRight(void) {
		cmd(ENTRY_BIT | WRITELEFT);
	}

	void rightToLeft(void) {
		cmd(ENTRY_BIT);
	}
	//----------------------------------------
	void setCursor(uint8_t x, uint8_t y)
	{		
		cmd(calculateCursorPosition(x,y));	
	}
	//---------------------------------------------
	void createChar(uint8_t location, uint8_t * charmap) {
		location &= 0x7; // we only have 8 locations 0-7
		cmd(CGDRAM_BIT | (location << 3));
		for (uint8_t i=0; i<8; i++) {
			write(charmap[i]);
		}
	}
		
};
	
	//code for number printing comes from
	//https://www.avrfreaks.net/forum/smallest-and-fastest-binary-bcd-conversion?page=all
	/*
	void bin2bcd( unsigned int val )
	{
		char i;

		i = '0' - 1;
		do
		i++;
		while( !((val -= 10000) & 0x8000) );
		buffer[4] = i;

		i = '0' + 10;
		do
		i--;
		while( (val += 1000) & 0x8000 );
		buffer[3] = i;

		i = '0' - 1;
		do
		i++;
		while( !((val -= 100) & 0x8000) );
		buffer[2] = i;

		i = '0' + 10;
		do
		i--;
		while( (val += 10) & 0x8000 );
		buffer[1] = i;

		buffer[0] = val | '0';
	}
	*/