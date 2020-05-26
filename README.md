# Arduino-4-Bit-LCD-Optimized-Library

WIP - Beta

This library optimizes a lot of bit mangling normally used by the arduino library through digital writes. 
However there's still a lot more work to be done.
Usage of this library comes with one big condition, you have to use certain pins for the databus.

Current Features:
  Printing text 
  Printing 8,16,32bit numbers
  setCursor function

Example setup:
         
LCD_4BIT<D0,D1,D4,D7,LCD1602> lcd;

First  - Enable Pin
Second - Register Select
Third  - D4 of the DataBus
Fourth - D7 of the DataBus
Fifth  - Column and row size, use LCD1602, LCD2002, LCD1604, LCD 2004
         Primarily used to make sure that whatever enters the setCursor function is valid

D5 and D6 don't need to be declared, the software handles the rest


For Arduino Uno
    recommended combinations
DataBus4 - DataBus7  
```
       D8-D11
       A0-A3
       D0-D3
       D4-D7
       ```
    
For the Arduino 
		recommended combinations
		```
		D37-D34
		D33-D30
		D53-D50
		D10-D13
		D21-D18
		D22-D25
		D26-D29
		A0-A3
		A4-A7
		```
  
  other possible combinations
  		```
		D37-D34 , D33-D30
		D53-D50
		D10-D13
		D21-D18
		D22-D25 , D26-D29
		A0-A3 , A4 - A7
		D6,D9
		A8-11, A12-A15
		D49-D46, D45-D42
		```
    
    
    
		
