/*
  Charly 25 RX BPF Board Tester V1.2
  Sends I2C commands to the filter board device on the I2C bus interface of the Arduino board.
  Copyright (C) 2017  Markus Grundner / DG8MG

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <UTFT.h>
#include <UTouch.h>
#include <Wire.h>

// Initialize display
// ------------------
// Set the pins to the correct ones for your development board
// -----------------------------------------------------------
// Standard Arduino Uno/2009 Shield            : <display model>,19,18,17,16
// Standard Arduino Mega/Due shield            : <display model>,38,39,40,41
// CTE TFT LCD/SD Shield for Arduino Due       : <display model>,25,26,27,28
// Teensy 3.x TFT Test Board                   : <display model>,23,22, 3, 4
// ElecHouse TFT LCD/SD Shield for Arduino Due : <display model>,22,23,31,33
//
// Remember to change the model parameter to suit your display module!
UTFT    myGLCD(ITDB32S,38,39,40,41);

// Initialize touchscreen
// ----------------------
// Set the pins to the correct ones for your development board
// -----------------------------------------------------------
// Standard Arduino Uno/2009 Shield            : 15,10,14, 9, 8
// Standard Arduino Mega/Due shield            :  6, 5, 4, 3, 2
// CTE TFT LCD/SD Shield for Arduino Due       :  6, 5, 4, 3, 2
// Teensy 3.x TFT Test Board                   : 26,31,27,28,29
// ElecHouse TFT LCD/SD Shield for Arduino Due : 25,26,27,29,30
//
UTouch  myTouch( 6, 5, 4, 3, 2);

// Declare which font we will be using
extern uint8_t BigFont[];

uint8_t I2CCommandString[3] = {0};  // I2C command string

int i2c_address = 0x21;  // Default I2C address of RX1 BPF board is 0x21
unsigned int x, y;  // x and y coordinates on the display

/*************************
**   Custom functions   **
*************************/

// Draw the buttons on the touch display
void drawButtons()
{
  // Draw the upper row of buttons
  for (x=0; x<6; x++)
  {
    myGLCD.setColor(0, 0, 255);
    myGLCD.fillRoundRect (10+(x*50), 10, 50+(x*50), 60);
    myGLCD.setColor(255, 255, 255);
    myGLCD.drawRoundRect (10+(x*50), 10, 50+(x*50), 60);
  }

  // Draw the center row of buttons
  for (x=0; x<6; x++)
  {
    myGLCD.setColor(0, 0, 255);
    myGLCD.fillRoundRect (10+(x*50), 70, 50+(x*50), 120);
    myGLCD.setColor(255, 255, 255);
    myGLCD.drawRoundRect (10+(x*50), 70, 50+(x*50), 120);
  }

  // Print the bandfilter numbers on the buttons
  myGLCD.print("630", 38, 12, 90);
  myGLCD.print("160", 88, 12, 90);
  myGLCD.print("80", 115, 27);
  myGLCD.print("60", 165, 27);
  myGLCD.print("40", 215, 27);
  myGLCD.print("30", 265, 27);

  myGLCD.print("20", 15, 87);
  myGLCD.print("17", 65, 87);
  myGLCD.print("15", 115, 87);
  myGLCD.print("12", 165, 87);
  myGLCD.print("10", 215, 87);
  myGLCD.print("6", 265, 87);

  myGLCD.setBackColor (0, 0, 0);
}

// Switch the filter band
void switchFilterBand (uint8_t* CommandString)
{
  Wire.beginTransmission(i2c_address);  // Transmit to device #i2c_address
  Wire.write(CommandString, 3);  // Send the I2C command string
  
  if (Wire.endTransmission() == 0)  // Finish transmitting and check the result
  {
    // Print the positive result on the display
    myGLCD.setColor(0, 255, 0);
    myGLCD.print("Sent!    ", LEFT, 208);
  }
  else
  {
    // Print the negative result on the display
    myGLCD.setColor(255, 0, 0);
    myGLCD.print("NOT Sent!", LEFT, 208);
  }
}

// Draw a red frame around the button while it is touched
void waitForIt(int x1, int y1, int x2, int y2)
{
  myGLCD.setColor(255, 0, 0);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
  while (myTouch.dataAvailable())
  myTouch.read();
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
}

// Initial Dance: Let the LED's dance :-)
void initialDance(void)
{
  int8_t index = 0;

  I2CCommandString[0] = 3;  // Command byte 
  I2CCommandString[1] = 0;  // Data byte for register 1
  I2CCommandString[2] = 0;  // Data byte for register 0
    
  for (index = 7; index >= 0; index--)
  {
    I2CCommandString[2] |= 1 << index;
    switchFilterBand(I2CCommandString);
    delay(100);
    I2CCommandString[2] = 0;
    switchFilterBand(I2CCommandString);  
  }
  
  for (index = 7; index > 3; index--)
  {
    I2CCommandString[1] |= 1 << index;
    switchFilterBand(I2CCommandString);
    delay(100);
    I2CCommandString[1] = 0;
    switchFilterBand(I2CCommandString); 
  }

  for (index = 5; index < 8; index++)
  {
    I2CCommandString[1] |= 1 << index;
    switchFilterBand(I2CCommandString);
    delay(100);
    I2CCommandString[1] = 0;
    switchFilterBand(I2CCommandString); 
  }

  for (index = 0; index < 8; index++)
  {
    I2CCommandString[2] |= 1 << index;
    switchFilterBand(I2CCommandString);
    delay(100);
    I2CCommandString[2] = 0;
    switchFilterBand(I2CCommandString);  
  }
   
  for (index = 0; index < 5; index++)
  {
    delay(100);
    I2CCommandString[1] = 80;
    I2CCommandString[2] = 85;
    switchFilterBand(I2CCommandString);
    delay(100);
    I2CCommandString[1] = 160;
    I2CCommandString[2] = 170;
    switchFilterBand(I2CCommandString); 
  }
  delay(100);
  I2CCommandString[1] = 0;
  I2CCommandString[2] = 0;
  switchFilterBand(I2CCommandString);
}

/*************************
**  Required functions  **
*************************/

// Initial setup
void setup()
{
  Wire.begin();  // Join i2c bus (address optional for master)
  myGLCD.InitLCD();  // Initialize the LCD display
  myGLCD.clrScr();  // Clear the screen

  myTouch.InitTouch();  // Initialize the touch display
  myTouch.setPrecision(PREC_MEDIUM);  // Set the precision of the touch display

  myGLCD.setFont(BigFont);  // Set the font
  myGLCD.setBackColor(0, 0, 255);  // Set the background color to blue
  drawButtons();  // Draw the buttons on the display
  myGLCD.print("Band:",LEFT,160);
  myGLCD.print("ADDR:   ",RIGHT,208);  // Print the address label on the display
  myGLCD.printNumI(i2c_address, RIGHT, 208);  // Print the I2C bus address on the display
  
  I2CCommandString[0] = 6;  // Command byte for configuration port 0 
  I2CCommandString[1] = 0;  // Data byte for register 6 -> All I/O ports are set as outputs
  I2CCommandString[2] = 0;  // Data byte for register 7 -> All I/O ports are set as outputs
  switchFilterBand(I2CCommandString);  // Send the initalization for the I/O ports
  
  initialDance(); 
}

// Main program loop
void loop()
{
  // check if the display was touched
  if (myTouch.dataAvailable())
  {
    // Read and store the coordinate of the touched area
    myTouch.read();
    x=myTouch.getX();
    y=myTouch.getY();
	
    I2CCommandString[0] = 3;  // Command byte 
    I2CCommandString[1] = 0;  // Data byte for register 1
    I2CCommandString[2] = 0;  // Data byte for register 0

    // check if a button in the upper row was touched
    if ((y>=10) && (y<=60))  // yes, it's in the upper row
    {  
      if ((x>=10) && (x<=50))  // Button: 630 meter band
      {
        waitForIt(10, 10, 50, 60);
		    myGLCD.print("630 Meter",CENTER,160);
        I2CCommandString[2] |= 1 << 7;
      }

      if ((x>=60) && (x<=100))  // Button: 160 meter band
      {
        waitForIt(60, 10, 100, 60);
        myGLCD.print("160 Meter",CENTER,160);
        I2CCommandString[2] |= 1 << 6;
      }

      if ((x>=110) && (x<=150))  // Button: 80 meter band
      {
        waitForIt(110, 10, 150, 60);
        myGLCD.print(" 80 Meter",CENTER,160);
        I2CCommandString[2] |= 1 << 5;
      }

      if ((x>=160) && (x<=200))  // Button: 60 meter band
      {
        waitForIt(160, 10, 200, 60);
        myGLCD.print(" 60 Meter",CENTER,160);
        I2CCommandString[2] |= 1 << 4;
      }

      if ((x>=210) && (x<=250))  // Button: 40 meter band
      {
        waitForIt(210, 10, 250, 60);
        myGLCD.print(" 40 Meter",CENTER,160);
        I2CCommandString[2] |= 1 << 3;
      }

      if ((x>=260) && (x<=300))  // Button: 30 meter band
      {
        waitForIt(260, 10, 300, 60);
        myGLCD.print(" 30 Meter",CENTER,160);
        I2CCommandString[2] |= 1 << 2;
      }
	  
      switchFilterBand(I2CCommandString);
    }

    // Check if a button in the lower row was touched
    if ((y>=70) && (y<=120))  // yes, it's in the lower row
    {
      if ((x>=10) && (x<=50))  // Button: 20 meter band
      {
        waitForIt(10, 70, 50, 120);
        myGLCD.print(" 20 Meter",CENTER,160);
        I2CCommandString[2] |= 1 << 1;
      }

      if ((x>=60) && (x<=100))  // Button: 17 meter band
      {
        waitForIt(60, 70, 100, 120);
        myGLCD.print(" 17 Meter",CENTER,160);
        I2CCommandString[2] |= 1 << 0;
      }

      if ((x>=110) && (x<=150))  // Button: 15 meter band
      {
        waitForIt(110, 70, 150, 120);
        myGLCD.print(" 15 Meter",CENTER,160);
        I2CCommandString[1] |= 1 << 7;
      }

      if ((x>=160) && (x<=200))  // Button: 12 meter band
      {
        waitForIt(160, 70, 200, 120);
        myGLCD.print(" 12 Meter",CENTER,160);
        I2CCommandString[1] |= 1 << 6;
      }

      if ((x>=210) && (x<=250))  // Button: 10 meter band
      {
        waitForIt(210, 70, 250, 120);
        myGLCD.print(" 10 Meter",CENTER,160);
        I2CCommandString[1] |= 1 << 5;
      }
      
      if ((x>=260) && (x<=300))  // Button: 6 meter band
      {
        waitForIt(260, 70, 300, 120);
        myGLCD.print("  6 Meter",CENTER,160);
        I2CCommandString[1] |= 1 << 4;
      }
	  
      switchFilterBand(I2CCommandString);
    }
  }
}
