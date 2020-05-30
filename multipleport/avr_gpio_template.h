#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <util/delay.h>
#include <string.h>
#include <avr/pgmspace.h>

//if you're using AVR pin out, comment out this description
//the following should be UNO or MEGA, not both
//#define UNO
#define UNO

//atmega328p only
/*
	enum PINS : uint8_t  {
	D8,D9,D10,D11,D13,
	A0,A1,A2,A3,A4,A5,
	D0,D1,D2,D3,D4,D5,D6,D7}
*/

/*
	Arduino Uno Equivalent
	D8,D9,D10,D11,D12,D13
	A0,A1,A2,A3,A4,A5	
	D0,D1,D2,D3,D4,D5,D6,D7
*/

	enum PINS : uint8_t  {
				
		//Arduino Uno Stuff
		#if defined UNO
		
		D8=0,D9,D10,D11,D13,
		A0=8,A1,A2,A3,A4,A5,
		D0=16,D1,D2,D3,D4,D5,D6,D7
		
		//Arduino Mega
		#elif defined MEGA
		
		//B
		D53=0,D52,D51,D50,D10,D11,D12,D13,
		//C
		D37=8,D36,D35,D34,D33,D32,D31,D30,
		//D
		D21=16,D20,D19,D18,D38=23,
		//A
		D22=24,D23,D24,D25,D26,D27,D28,D29,
		//E
		D0=32,D1,D5=35,D2,D3,
		//F
		A0=40,A1,A2,A3,A4,A5,A6,A7,
		//G
		D41=48,D40,D39,D4=52,
		//below pins are not recommended
		//H
		D17=56,D16,D6=59,D7,D8,D9,
		//J
		D15=64,D14,
		//K
		A8=72,A9,A10,A11,A12,A13,A14,A15,
		//L
		D49=80,D48,D47,D46,D45,D44,D43,D42	
			
		#else
		
		B0=0,B1,B2,B3,B4,B5,   //B6,7 usually used by the crystal, do not use with arduino
		C0=8,C1,C2,C3,C4,C5, //C6,7 are analog input only, only usable when using TQFP-32 packages
		D0=16,D1,D2,D3,D4,D5,D6,D7
		
		#endif		
		/*
		4 bit possible combinations
		D37-D34 , D33-D30
		D53,D50
		D10,D13
		D21,D18
		D22-D25 , D26-D29
		A0-A3 , A4 - A7
		D6,D9
		A8-11,A12-A15
		D49-D46, D45-D42
		
		recommended combinations
		D37,D34
		D33,D30
		D53,D50
		D10,D13
		D21,D18
		D22,D25
		D26,D29
		A0,A3
		A4,A7
	
		*/

		};	

template<PINS Pin_>
class GPIO
{
	
    constexpr static uint8_t portx_{Pin_>>3}; //B=0,C=1,D=2
    constexpr static uint8_t pin_{Pin_ & 7}; //0-7
    constexpr static uint8_t pinbm_{1<<pin_}; //0x01,0x02,0x04...0x80

    constexpr static volatile uint8_t* ddrR(){ 

       return portx_ == 0 ? &DDRB : portx_ == 1 ? &DDRC : &DDRD; 
    }
    constexpr static volatile uint8_t* portR(){ 
        return portx_ == 0 ? &PORTB : portx_ == 1 ? &PORTC : &PORTD;
    }
    constexpr static volatile uint8_t* pinR(){
        return portx_ == 0 ? &PINB : portx_ == 1 ? &PINC : &PIND;
    }

	public:	
	
	static inline void setOutput(){ *ddrR() |= pinbm_; }
	static inline void setInput(){ *ddrR() &=~ pinbm_; }
	
	static inline void setPullup(){ *ddrR() &=~ pinbm_; *portR() |= pinbm_; }
		
	static inline void high(){ *portR() |= pinbm_; }	
	static inline void low(){ *portR() &= ~pinbm_; }
		
	static inline void on(){ *portR() |= pinbm_; }	
	static inline void off(){ *portR() &= ~pinbm_; }
	
	void toggle(){ *pinR() |= pinbm_; }
};

template<PINS lsb, PINS msb>
class PORT
{
	    constexpr static uint8_t _portx{msb>>3}; //B=0,C=1,D=2
		constexpr static uint8_t _shiftbit {0x07&lsb};
	    constexpr static uint8_t _pins{( (1<<(msb-lsb+1))-1 ) << _shiftbit  };
	
	    constexpr static volatile uint8_t* ddrR(){
		    //return _portx == 0 ? &DDRB : _portx == 1 ? &DDRC : &DDRD;
			return _portx == 0 ? &DDRB : _portx == 1 ? &DDRC : _portx == 2 ? &DDRD : &DDRA;
	    }
	    constexpr static volatile uint8_t* portR(){
		    return _portx == 0 ? &PORTB : _portx == 1 ? &PORTC : &PORTD;
	    }
	    constexpr static volatile uint8_t* pinR(){
		    return _portx == 0 ? &PINB : _portx == 1 ? &PINC : &PIND;
	    }
	
	public:
	void setOutput(){ *ddrR() |= _pins; }
	void setInput(){ *ddrR() &=~ _pins; }
		 
	void write(uint8_t input)
	{	
	*portR() = (uint8_t) (*portR()&~ _pins) | ((input<<(_shiftbit))&_pins);	
	}
	
};
