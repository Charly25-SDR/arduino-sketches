/*
  Charly 25LC Board Tester V1.2
  Sends I2C commands to the Charly 25LC board on the I2C bus interface of the Arduino board.
  Copyright (C) 2016  Markus Grundner / DG8MG

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

/*
  Charly 25LC pin assignment for PCA9555 port expander IC:
  Attenuator 12dB: I/O 0.0 -> Initial value = 0
  Attenuator 24dB: I/O 0.1 -> Initial value = 0
  VV 1 (preamp 1): I/O 0.2 -> Initial value = 0
  VV 2 (preamp 2): I/O 0.3 -> Initial value = 0
  unused: I/O 0.4 -> Initial value = 0
  unused: I/O 0.5 -> Initial value = 0
  unused: I/O 0.6 -> Initial value = 0
  unused: I/O 0.7 -> Initial value = 0
  
  10m: I/O 1.0 -> Initial value = 1
  20m: I/O 1.1 -> Initial value = 0
  40m: I/O 1.2 -> Initial value = 0
  80m: I/O 1.3 -> Initial value = 0
  Antenna relais: I/O 1.4 -> Initial value = 0
  PA switch: I/O 1.5 -> Initial value = 0
  unused: I/O 1.6 -> Initial value = 0
  unused: I/O 1.7 -> Initial value = 0
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
uint8_t IO_PortStatus[2] = {0};  // I/O port status string

int i2c_address = 0x20;  // Default I2C address 0x20
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
    myGLCD.setColor(255, 255, 255);  // White
    myGLCD.drawRoundRect (10+(x*50), 10, 50+(x*50), 60);
  }

  // Draw the lower row of buttons
  for (x=0; x<6; x++)
  {
    myGLCD.setColor(255, 255, 255);  // White
    myGLCD.drawRoundRect (10+(x*50), 70, 50+(x*50), 120);
  }

  printLabels();  // Print the labels on the buttons
}

// Print the labels on the buttons
void printLabels()
{  
  myGLCD.setColor(255, 255, 255);  // White
  
  myGLCD.print("10", 15, 27);
  myGLCD.print("20", 65, 27);
  myGLCD.print("40", 115, 27);
  myGLCD.print("80", 165, 27);
  myGLCD.print("ANT", 238, 13, 90);
  myGLCD.print("PA", 265, 27);

  myGLCD.print("IO5", 38, 73, 90);
  myGLCD.print("IO4", 88, 73, 90);
  myGLCD.print("VV2", 138, 73, 90);
  myGLCD.print("VV1", 188, 73, 90);
  myGLCD.print("A24", 238, 73, 90);
  myGLCD.print("A12", 288, 73, 90);
}

// Update a label on a button
void updateLabel(unsigned int Index, bool Status)
{
  if (Status)
  {
    myGLCD.setColor(0, 255, 0);  // Green  
  }
  else
  {
    myGLCD.setColor(255, 255, 255);  // White
  }
  
  switch (Index)
  {
    case 0:
    myGLCD.print("10", 15, 27);
    break;
    case 1:
    myGLCD.print("20", 65, 27);
    break;
    case 2:
    myGLCD.print("40", 115, 27);
    break;
    case 3:
    myGLCD.print("80", 165, 27);
    break;
    case 4:
    myGLCD.print("ANT", 238, 13, 90);
    break;
    case 5:
    myGLCD.print("PA", 265, 27);
    break;
  
    case 6:
    myGLCD.print("IO5", 38, 73, 90);
    break;
    case 7:
    myGLCD.print("IO4", 88, 73, 90);
    break;
    case 8:
    myGLCD.print("VV2", 138, 73, 90);
    break;
    case 9:
    myGLCD.print("VV1", 188, 73, 90);
    break;
    case 10:
    myGLCD.print("A24", 238, 73, 90);
    break;
    case 11:
    myGLCD.print("A12", 288, 73, 90);
    break;
  }
}

// Read the I/O's
bool readIO (uint8_t* PortStatus)
{ 
  PortStatus[0] = 0;  // Clear all bits in the first status byte
  PortStatus[1] = 0;  // Clear all bits in the second status byte
  
  Wire.beginTransmission(i2c_address);  // Transmit to device #i2c_address
  Wire.write(0);  // Set requested register to 0 (= Input port 0)

  if (Wire.endTransmission() == 0)  // Finish transmitting and check the result
  {
    // Print the positive result on the display
    myGLCD.setColor(0, 255, 0);
    myGLCD.print("Sent!    ", LEFT, 208);
    
    Wire.requestFrom(i2c_address, 1);  // Request one byte (Input port 0) from I2C bus 
    PortStatus[0] = Wire.read();  // Read and save first byte (Input port 0) from I2C bus 
    Wire.requestFrom(i2c_address, 1);  // Request one byte (Input port 0) from I2C bus 
    PortStatus[1] = Wire.read();  // Read and save second byte (Input port 1) from I2C bus 
    return true;
  }
  else
  {
    // Print the negative result on the display
    myGLCD.setColor(255, 0, 0);
    myGLCD.print("NOT Sent!", LEFT, 208);
    return false;
  }
}

// Switch the I/O's
bool switchIO (uint8_t* CommandString)
{
  Wire.beginTransmission(i2c_address);  // Transmit to device #i2c_address
  Wire.write(CommandString, 3);  // Send the I2C command string
  
  if (Wire.endTransmission() == 0)  // Finish transmitting and check the result
  {
    // Print the positive result on the display
    myGLCD.setColor(0, 255, 0);  // Green
    myGLCD.print("Sent!    ", LEFT, 208);
    return true;
  } 
  else
  {
    // Print the negative result on the display
    myGLCD.setColor(255, 0, 0);  // Red
    myGLCD.print("NOT Sent!", LEFT, 208);
    return false;
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

/*************************
**  Required functions  **
*************************/

// Initial setup
void setup()
{
// DEBUG
//  Serial.begin(9600);
//  Serial.print("*** INIT ***");
//  Serial.println();
  
  Wire.begin();  // Join i2c bus (address optional for master)
  myGLCD.InitLCD();  // Initialize the LCD display
  myGLCD.clrScr();  // Clear the screen

  myTouch.InitTouch();  // Initialize the touch display
  myTouch.setPrecision(PREC_MEDIUM);  // Set the precision of the touch display

  myGLCD.setFont(BigFont);  // Set the font
  myGLCD.setBackColor(0, 0, 0);  // Set the background color to black
  drawButtons();  // Draw the buttons on the display
  myGLCD.print("Band:",LEFT,160);
  myGLCD.print("Relais:",LEFT,176);
  myGLCD.print("ADDR:   ",RIGHT,208);  // Print the address label on the display
  myGLCD.printNumI(i2c_address, RIGHT, 208);  // Print the I2C bus address on the display
  
  I2CCommandString[0] = 6;  // Command byte for configuration port 0 
  I2CCommandString[1] = 0;  // Data byte for register 6 -> All I/O ports are set as outputs
  I2CCommandString[2] = 0;  // Data byte for register 7 -> All I/O ports are set as outputs
  switchIO(I2CCommandString);  // Send the initalization for the I/O ports

  I2CCommandString[0] = 3;  // Command byte, first write into I/O register 1 and then into I/O register 0
  I2CCommandString[1] = 0b00000001;  // Copy the initial I/O status byte for register 1
  I2CCommandString[2] = 0b00000000;  // Copy the initial I/O status byte for register 0
  switchIO(I2CCommandString);  // Send the initalization for the I/O ports
  updateLabel(0, 1); // Update the 10 meter band label to the initial status
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
	
    readIO(IO_PortStatus);  // Read the current status of the I/O ports
    I2CCommandString[0] = 3;  // Command byte, first write into I/O register 1 and then into I/O register 0
    I2CCommandString[1] = IO_PortStatus[1];  // Copy the current I/O status byte for register 1
    I2CCommandString[2] = IO_PortStatus[0];  // Copy the current I/O status byte for register 0

// DEBUG
//    Serial.print ("Old I/O Status Port 0:");
//    Serial.print (IO_PortStatus[0],BIN);
//    Serial.print ("\n");
//    Serial.print ("Old I/O Status Port 1:");
//    Serial.print (IO_PortStatus[1],BIN);
//    Serial.print ("\n");

    // check if a button in the upper row was touched
    if ((y>=10) && (y<=60))  // yes, it's in the upper row
    {
      // check if a band switching button was touched
      if ((x>=10) && (x<=200))  // yes, it's a band switching button
      {    
        updateLabel((log(I2CCommandString[1] & B00001111) / log (2)), 0);  // Clear the active band button on the display
        I2CCommandString[1] = I2CCommandString[1] & B11110000;  // Deactivate all band filters, leave the other I/O's unchanged
        
        if ((x>=10) && (x<=50))  // Button: 10 meter band
        {
          waitForIt(10, 10, 50, 60);
      		myGLCD.print(" 10 Meter ",CENTER,160);          
          I2CCommandString[1] = I2CCommandString[1] | B00000001;  // Activate the new band filter
          updateLabel(0, 1);
// DEBUG        
//        Serial.print("Buttonstatus: ");
//        Serial.print(bitRead(I2CCommandString[1], 0));
//        Serial.print("\n");        
        }
  
        if ((x>=60) && (x<=100))  // Button: 20 meter band
        {
          waitForIt(60, 10, 100, 60);
          myGLCD.print(" 20 Meter ",CENTER,160);
          I2CCommandString[1] = I2CCommandString[1] | B00000010;  // Activate the new band filter
          updateLabel(1, 1);
// DEBUG        
//        Serial.print("Buttonstatus: ");
//        Serial.print(bitRead(I2CCommandString[1], 1));
//        Serial.print("\n");               
        }
  
        if ((x>=110) && (x<=150))  // Button: 40 meter band
        {
          waitForIt(110, 10, 150, 60);
          myGLCD.print(" 40 Meter ",CENTER,160);
          I2CCommandString[1] = I2CCommandString[1] | B00000100;  // Activate the new band filter
          updateLabel(2, 1);
// DEBUG        
//        Serial.print("Buttonstatus: ");
//        Serial.print(bitRead(I2CCommandString[1], 2));
//        Serial.print("\n");               
        }
  
        if ((x>=160) && (x<=200))  // Button: 80 meter band
        {
          waitForIt(160, 10, 200, 60);
          myGLCD.print(" 80 Meter ",CENTER,160);
          I2CCommandString[1] = I2CCommandString[1] | B00001000;  // Activate the new band filter
          updateLabel(3, 1);
// DEBUG        
//        Serial.print("Buttonstatus: ");
//        Serial.print(bitRead(I2CCommandString[1], 3));
//        Serial.print("\n");        
        }
      }
      else  // no band switching button touched
      {
        if ((x>=210) && (x<=250))  // Button: Antenna relais
        {
          waitForIt(210, 10, 250, 60);       
          myGLCD.print("  ANT  ",CENTER,176);
          I2CCommandString[1] = I2CCommandString[1] ^ B00010000;
          updateLabel(4, bitRead(I2CCommandString[1], 4));
// DEBUG        
//        Serial.print("Buttonstatus: ");
//        Serial.print(bitRead(I2CCommandString[1], 4));
//        Serial.print("\n");        
        }
  
        if ((x>=260) && (x<=300))  // Button: PA on
        {
          waitForIt(260, 10, 300, 60);
          myGLCD.print("  PA   ",CENTER,176);
          I2CCommandString[1] = I2CCommandString[1] ^ B00100000;
          updateLabel(5, (bitRead(I2CCommandString[1], 5)));
// DEBUG        
//        Serial.print("Buttonstatus: ");
//        Serial.print(bitRead(I2CCommandString[1], 5));
//        Serial.print("\n");
        }
      }
      
      switchIO(I2CCommandString);
    }

    // Check if a button in the lower row was touched
    if ((y>=70) && (y<=120))  // yes, it's in the lower row
    {     
      if ((x>=10) && (x<=50))  // Button: I/O 0.5 (unused)
      {
        waitForIt(10, 70, 50, 120);
        myGLCD.print("I/O 0.5",CENTER,176);
        I2CCommandString[2] = I2CCommandString[2] ^ B00100000;
        updateLabel(6, bitRead(I2CCommandString[2], 5));
// DEBUG        
//        Serial.print("Buttonstatus: ");
//        Serial.print(bitRead(I2CCommandString[2], 5));
//        Serial.print("\n");
      }

      if ((x>=60) && (x<=100))  // Button: I/O 0.4 (unused)
      {
        waitForIt(60, 70, 100, 120);
        myGLCD.print("I/O 0.4",CENTER,176);
        I2CCommandString[2] = I2CCommandString[2] ^ B00010000;
        updateLabel(7, bitRead(I2CCommandString[2], 4));
// DEBUG        
//        Serial.print("Buttonstatus: ");
//        Serial.print(bitRead(I2CCommandString[2], 4));
//        Serial.print("\n");
      }

      if ((x>=110) && (x<=150))  // Button: VV 2
      {
        waitForIt(110, 70, 150, 120);
        myGLCD.print(" VV 2  ",CENTER,176);
        I2CCommandString[2] = I2CCommandString[2] ^ B00001000;
        updateLabel(8, bitRead(I2CCommandString[2], 3));
// DEBUG        
//        Serial.print("Buttonstatus: ");
//        Serial.print(bitRead(I2CCommandString[2], 3));
//        Serial.print("\n");        
      }

      if ((x>=160) && (x<=200))  // Button: VV 1
      {
        waitForIt(160, 70, 200, 120);
        myGLCD.print(" VV 1  ",CENTER,176);
        I2CCommandString[2] = I2CCommandString[2] ^ B00000100;
        updateLabel(9, bitRead(I2CCommandString[2], 2));
// DEBUG        
//        Serial.print("Buttonstatus: ");
//        Serial.print(bitRead(I2CCommandString[2], 2));
//        Serial.print("\n");
      }

      if ((x>=210) && (x<=250))  // Button: Attenuator 24dB
      {
        waitForIt(210, 70, 250, 120);
        myGLCD.print("A 24dB ",CENTER,176);
        I2CCommandString[2] = I2CCommandString[2] ^ B00000010;
        updateLabel(10, bitRead(I2CCommandString[2], 1));
// DEBUG        
//        Serial.print("Buttonstatus: ");
//        Serial.print(bitRead(I2CCommandString[2], 1));
//        Serial.print("\n");
      }
      
      if ((x>=260) && (x<=300))  // Button: Attenuator 12dB
      {
        waitForIt(260, 70, 300, 120);
        myGLCD.print("A 12dB ",CENTER,176);
        I2CCommandString[2] = I2CCommandString[2] ^ B00000001;
        updateLabel(11, bitRead(I2CCommandString[2], 0));
// DEBUG        
//        Serial.print("Buttonstatus: ");
//        Serial.print(bitRead(I2CCommandString[2], 0));
//        Serial.print("\n");
      }
	  
      switchIO(I2CCommandString);
    }

// DEBUG    
//    readIO(IO_PortStatus);  // Read the status of the I/O ports
//    Serial.print ("New I/O Status Port 0:");
//    Serial.print (IO_PortStatus[0],BIN);
//    Serial.print ("\n");
//    Serial.print ("New I/O Status Port 1:");
//    Serial.print (IO_PortStatus[1],BIN);
//    Serial.print ("\n");
  }
}
