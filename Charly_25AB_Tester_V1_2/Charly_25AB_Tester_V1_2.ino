/*
  Charly 25 Allband Tester V1.2 / 16.03.2017 (C) Markus Grundner / DG8MG
  Sends I2C commands to the Charly 25AB device on the I2C bus interface of the Arduino board
*/

/*
  Charly 25AB pin assignment for PCA9555 port expander IC:
  
  6m:               I/O 0.0 -> Initial value = 1
  10/12m:           I/O 0.1 -> Initial value = 0
  15/17m:           I/O 0.2 -> Initial value = 0
  20m:              I/O 0.3 -> Initial value = 0
  30/40m:           I/O 0.4 -> Initial value = 0
  60/80m:           I/O 0.5 -> Initial value = 0
  160m:             I/O 0.6 -> Initial value = 0
  unused:           I/O 0.7 -> Initial value = 0
  
  Attenuator 12dB:  I/O 1.0 -> Initial value = 0
  Attenuator 24dB:  I/O 1.1 -> Initial value = 0
  VV 1 (preamp 1):  I/O 1.2 -> Initial value = 0
  VV 2 (preamp 2):  I/O 1.3 -> Initial value = 0
  RX/TX relais:     I/O 1.4 -> Initial value = 0
  PA enable:        I/O 1.5 -> Initial value = 0
  ANT select:       I/O 1.6 -> Initial value = 0
  unused:           I/O 1.7 -> Initial value = 0
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
  for (x=0; x<8; x++)
  {
    myGLCD.setColor(255, 255, 255);  // White
    myGLCD.drawRoundRect (3+(x*40), 10, 38+(x*40), 95);
  }

  // Draw the lower row of buttons
  for (x=0; x<8; x++)
  {
    myGLCD.setColor(255, 255, 255);  // White
    myGLCD.drawRoundRect (3+(x*40), 105, 38+(x*40), 190);
  }

  printLabels();  // Print the labels on the buttons
}

// Print the labels on the buttons
void printLabels()
{  
  myGLCD.setColor(255, 255, 255);  // White
  
  myGLCD.print("   6", 28, 13, 90);
  myGLCD.print("10/12", 68, 13, 90);
  myGLCD.print("15/17", 108, 13, 90);
  myGLCD.print("  20", 148, 13, 90);
  myGLCD.print("30/40", 188, 13, 90);
  myGLCD.print("60/80", 228, 13, 90);
  myGLCD.print(" 160", 268, 13, 90);
  myGLCD.print("IO1.7", 308, 13, 90);

  myGLCD.print("IO0.7", 28, 108, 90);
  myGLCD.print("ANT S", 68, 108, 90);
  myGLCD.print("EN PA", 108, 108, 90);
  myGLCD.print("RX/TX", 148, 108, 90);
  myGLCD.print(" VV2", 188, 108, 90);
  myGLCD.print(" VV1", 228, 108, 90);
  myGLCD.print(" A24", 268, 108, 90);
  myGLCD.print(" A12", 308, 108, 90);
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
    myGLCD.print("   6", 28, 13, 90);
    break;
    case 1:
    myGLCD.print("10/12", 68, 13, 90);
    break;
    case 2:
    myGLCD.print("15/17", 108, 13, 90);
    break;
    case 3:
    myGLCD.print("  20", 148, 13, 90);
    break;
    case 4:
    myGLCD.print("30/40", 188, 13, 90);
    break;
    case 5:
    myGLCD.print("60/80", 228, 13, 90);
    break;
    case 6:
    myGLCD.print(" 160", 268, 13, 90);
    break;
    case 7:
    myGLCD.print("IO1.7", 308, 13, 90);
    break;
    
    case 8:
    myGLCD.print("IO0.7", 28, 108, 90);
    break;
    case 9:
    myGLCD.print("ANT S", 68, 108, 90);
    break;
    case 10:
    myGLCD.print("EN PA", 108, 108, 90);
    break;
    case 11:
    myGLCD.print("RX/TX", 148, 108, 90);
    break;
    case 12:
    myGLCD.print(" VV2", 188, 108, 90);
    break;
    case 13:
    myGLCD.print(" VV1", 228, 108, 90);
    break;
    case 14:
    myGLCD.print(" A24", 268, 108, 90);
    break;
    case 15:
    myGLCD.print(" A12", 308, 108, 90);
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
    myGLCD.print("Sent!    ", LEFT, 224);
    
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
    myGLCD.print("NOT Sent!", LEFT, 224);
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
    myGLCD.print("Sent!    ", LEFT, 224);
    return true;
  } 
  else
  {
    // Print the negative result on the display
    myGLCD.setColor(255, 0, 0);  // Red
    myGLCD.print("NOT Sent!", LEFT, 224);
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
  myGLCD.print("Band:",LEFT,192);
  myGLCD.print("Relais:",LEFT,208);
  myGLCD.print("ADDR:   ",RIGHT,224);  // Print the address label on the display
  myGLCD.printNumI(i2c_address, RIGHT, 224);  // Print the I2C bus address on the display
  
  I2CCommandString[0] = 6;  // Command byte for configuration port 0 
  I2CCommandString[1] = 0;  // Data byte for register 6 -> All I/O ports are set as outputs
  I2CCommandString[2] = 0;  // Data byte for register 7 -> All I/O ports are set as outputs
  switchIO(I2CCommandString);  // Send the initalization for the I/O ports

  I2CCommandString[0] = 3;  // Command byte, first write into I/O register 1 and then into I/O register 0
  I2CCommandString[1] = 0b00000001;  // Copy the initial I/O status byte for register 1
  I2CCommandString[2] = 0b00000000;  // Copy the initial I/O status byte for register 0
  switchIO(I2CCommandString);  // Send the initalization for the I/O ports
  updateLabel(0, 1); // Update the 6 meter band label to the initial status
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
    if ((y>=10) && (y<=95))  // yes, it's in the upper row
    {
      // check if a band switching button was touched
      if ((x>=3) && (x<=278))  // yes, it's a band switching button
      {    
        updateLabel((log(I2CCommandString[1] & B01111111) / log (2)), 0);  // Clear the active band button on the display
        I2CCommandString[1] = I2CCommandString[1] & B10000000;  // Deactivate all band filters, leave the other I/O's unchanged
        
        if ((x>=3) && (x<=38))  // Button: 6 meter band
        {
          waitForIt(3, 10, 38, 95);
          myGLCD.print("  6 Meter  ",CENTER,192);          
          I2CCommandString[1] = I2CCommandString[1] | B00000001;  // Activate the new band filter
          updateLabel(0, 1);
// DEBUG        
//        Serial.print("Buttonstatus: ");
//        Serial.print(bitRead(I2CCommandString[1], 0));
//        Serial.print("\n");        
        }
  
        if ((x>=43) && (x<=78))  // Button: 10/12 meter band
        {
          waitForIt(43, 10, 78, 95);
          myGLCD.print("10/12 Meter",CENTER,192);
          I2CCommandString[1] = I2CCommandString[1] | B00000010;  // Activate the new band filter
          updateLabel(1, 1);
// DEBUG        
//        Serial.print("Buttonstatus: ");
//        Serial.print(bitRead(I2CCommandString[1], 1));
//        Serial.print("\n");               
        }
  
        if ((x>=83) && (x<=118))  // Button: 15/17 meter band
        {
          waitForIt(83, 10, 118, 95);
          myGLCD.print("15/17 Meter",CENTER,192);
          I2CCommandString[1] = I2CCommandString[1] | B00000100;  // Activate the new band filter
          updateLabel(2, 1);
// DEBUG        
//        Serial.print("Buttonstatus: ");
//        Serial.print(bitRead(I2CCommandString[1], 2));
//        Serial.print("\n");               
        }
  
        if ((x>=123) && (x<=158))  // Button: 20 meter band
        {
          waitForIt(123, 10, 158, 95);
          myGLCD.print(" 20 Meter  ",CENTER,192);
          I2CCommandString[1] = I2CCommandString[1] | B00001000;  // Activate the new band filter
          updateLabel(3, 1);
// DEBUG        
//        Serial.print("Buttonstatus: ");
//        Serial.print(bitRead(I2CCommandString[1], 3));
//        Serial.print("\n");        
        }
        if ((x>=163) && (x<=198))  // Button: 30/40 meter band
        {
          waitForIt(163, 10, 198, 95);
          myGLCD.print("30/40 Meter",CENTER,192);
          I2CCommandString[1] = I2CCommandString[1] | B00010000;  // Activate the new band filter
          updateLabel(4, 1);
// DEBUG        
//        Serial.print("Buttonstatus: ");
//        Serial.print(bitRead(I2CCommandString[1], 4));
//        Serial.print("\n");        
        }
        if ((x>=203) && (x<=238))  // Button: 60/80 meter band
        {
          waitForIt(203, 10, 238, 95);
          myGLCD.print("60/80 Meter",CENTER,192);
          I2CCommandString[1] = I2CCommandString[1] | B00100000;  // Activate the new band filter
          updateLabel(5, 1);
// DEBUG        
//        Serial.print("Buttonstatus: ");
//        Serial.print(bitRead(I2CCommandString[1], 5));
//        Serial.print("\n");        
        }
        if ((x>=243) && (x<=278))  // Button: 160 meter band
        {
          waitForIt(243, 10, 278, 95);
          myGLCD.print(" 160 Meter ",CENTER,192);
          I2CCommandString[1] = I2CCommandString[1] | B01000000;  // Activate the new band filter
          updateLabel(6, 1);
// DEBUG        
//        Serial.print("Buttonstatus: ");
//        Serial.print(bitRead(I2CCommandString[1], 6));
//        Serial.print("\n");        
        }     
      }
      else  // no band switching button touched
      {
        if ((x>=283) && (x<=318))  // Button: I/O 1.7
        {
          waitForIt(283, 10, 318, 95);       
          myGLCD.print("I/O 1.7",CENTER,208);
          I2CCommandString[1] = I2CCommandString[1] ^ B10000000;
          updateLabel(7, bitRead(I2CCommandString[1], 7));
// DEBUG        
//        Serial.print("Buttonstatus: ");
//        Serial.print(bitRead(I2CCommandString[1], 7));
//        Serial.print("\n");        
        }
      }
      
      switchIO(I2CCommandString);
    }

    // Check if a button in the lower row was touched
    if ((y>=105) && (y<=190))  // yes, it's in the lower row
    {     
      if ((x>=3) && (x<=38))  // Button: I/O 0.7 (unused)
      {
        waitForIt(3, 105, 38, 190);
        myGLCD.print("I/O 0.7",CENTER,208);
        I2CCommandString[2] = I2CCommandString[2] ^ B10000000;
        updateLabel(8, bitRead(I2CCommandString[2], 7));
// DEBUG        
//        Serial.print("Buttonstatus: ");
//        Serial.print(bitRead(I2CCommandString[2], 7));
//        Serial.print("\n");
      }

      if ((x>=43) && (x<=78))  // Button: ANT S
      {
        waitForIt(43, 105, 78, 190);
        myGLCD.print(" ANT S ",CENTER,208);
        I2CCommandString[2] = I2CCommandString[2] ^ B01000000;
        updateLabel(9, bitRead(I2CCommandString[2], 6));
// DEBUG        
//        Serial.print("Buttonstatus: ");
//        Serial.print(bitRead(I2CCommandString[2], 6));
//        Serial.print("\n");
      }

      if ((x>=83) && (x<=118))  // Button: EN PA
      {
        waitForIt(83, 105, 118, 190);
        myGLCD.print(" EN PA ",CENTER,208);
        I2CCommandString[2] = I2CCommandString[2] ^ B00100000;
        updateLabel(10, bitRead(I2CCommandString[2], 5));
// DEBUG        
//        Serial.print("Buttonstatus: ");
//        Serial.print(bitRead(I2CCommandString[2], 5));
//        Serial.print("\n");
      }

      if ((x>=123) && (x<=158))  // Button: RX/TX
      {
        waitForIt(123, 105, 158, 190);
        myGLCD.print(" RX/TX ",CENTER,208);
        I2CCommandString[2] = I2CCommandString[2] ^ B00010000;
        updateLabel(11, bitRead(I2CCommandString[2], 4));
// DEBUG        
//        Serial.print("Buttonstatus: ");
//        Serial.print(bitRead(I2CCommandString[2], 4));
//        Serial.print("\n");
      }

      if ((x>=163) && (x<=198))  // Button: VV 2
      {
        waitForIt(163, 105, 198, 190);
        myGLCD.print(" VV 2  ",CENTER,208);
        I2CCommandString[2] = I2CCommandString[2] ^ B00001000;
        updateLabel(12, bitRead(I2CCommandString[2], 3));
// DEBUG        
//        Serial.print("Buttonstatus: ");
//        Serial.print(bitRead(I2CCommandString[2], 3));
//        Serial.print("\n");        
      }

      if ((x>=203) && (x<=238))  // Button: VV 1
      {
        waitForIt(203, 105, 238, 190);
        myGLCD.print(" VV 1  ",CENTER,208);
        I2CCommandString[2] = I2CCommandString[2] ^ B00000100;
        updateLabel(13, bitRead(I2CCommandString[2], 2));
// DEBUG        
//        Serial.print("Buttonstatus: ");
//        Serial.print(bitRead(I2CCommandString[2], 2));
//        Serial.print("\n");
      }

      if ((x>=243) && (x<=278))  // Button: Attenuator 24dB
      {
        waitForIt(243, 105, 278, 190);
        myGLCD.print("A 24dB ",CENTER,208);
        I2CCommandString[2] = I2CCommandString[2] ^ B00000010;
        updateLabel(14, bitRead(I2CCommandString[2], 1));
// DEBUG        
//        Serial.print("Buttonstatus: ");
//        Serial.print(bitRead(I2CCommandString[2], 1));
//        Serial.print("\n");
      }
      
      if ((x>=283) && (x<=318))  // Button: Attenuator 12dB
      {
        waitForIt(283, 105, 318, 190);
        myGLCD.print("A 12dB ",CENTER,208);
        I2CCommandString[2] = I2CCommandString[2] ^ B00000001;
        updateLabel(15, bitRead(I2CCommandString[2], 0));
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
