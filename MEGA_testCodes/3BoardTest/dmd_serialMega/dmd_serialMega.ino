

/*--------------------------------------------------------------------------------------

 dmd_test.cpp 
   Demo and example project for the Freetronics DMD, a 512 LED matrix display
   panel arranged in a 32 x 16 layout.

 Copyright (C) 2011 Marc Alexander (info <at> freetronics <dot> com)

 See http://www.freetronics.com/dmd for resources and a getting started guide.

 Note that the DMD library uses the SPI port for the fastest, low overhead writing to the
 display. Keep an eye on conflicts if there are any other devices running from the same
 SPI port, and that the chip select on those devices is correctly set to be inactive
 when the DMD is being written to.

 USAGE NOTES
 -----------

 - Place the DMD library folder into the "arduino/libraries/" folder of your Arduino installation.
 - Get the TimerOne library from here: http://code.google.com/p/arduino-timerone/downloads/list
   or download the local copy from the DMD library page (which may be older but was used for this creation)
   and place the TimerOne library folder into the "arduino/libraries/" folder of your Arduino installation.
 - Restart the IDE.
 - In the Arduino IDE, you can open File > Examples > DMD > dmd_demo, or dmd_clock_readout, and get it
   running straight away!

 * The DMD comes with a pre-made data cable and DMDCON connector board so you can plug-and-play straight
   into any regular size Arduino Board (Uno, Freetronics Eleven, EtherTen, USBDroid, etc)
  
 * Please note that the Mega boards have SPI on different pins, so this library does not currently support
   the DMDCON connector board for direct connection to Mega's, please jumper the DMDCON pins to the
   matching SPI pins on the other header on the Mega boards.

 This example code is in the public domain.
 The DMD library is open source (GPL), for more see DMD.cpp and DMD.h

--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------
  Includes
--------------------------------------------------------------------------------------*/
#include <SPI.h>        //SPI.h must be included as DMD is written by SPI (the IDE complains otherwise)
#include <DMDMega.h>        //
#include <TimerOne.h>   //
#include <EEPROM.h>


#include "SystemFont5x7.h"
#include "Arial_black_16.h"

//Fire up the DMD library as dmd
#define DISPLAYS_ACROSS 4
#define DISPLAYS_DOWN 1
#define DISPLAYS_BPP 1
#define WHITE 0xFF
#define BLACK 0
#define DATACOUNTADD   200

DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN );

char inputBuffer[75] = "Ideonics Ideas & Electronics";
boolean stringComplete = false;  // whether the string is complete
int dataCount = 0;


/*--------------------------------------------------------------------------------------
  Interrupt handler for Timer1 (TimerOne) driven DMD refresh scanning, this gets
  called at the period set in Timer1.initialize();
--------------------------------------------------------------------------------------*/
void ScanDMD()
{ 
  dmd.scanDisplayBySPI();
}

/*--------------------------------------------------------------------------------------
  setup
  Called by the Arduino architecture before the main loop begins
--------------------------------------------------------------------------------------*/
void setup(void)
{
   Serial.begin(9600);
   Serial1.begin(9600);
   //initialize TimerOne's interrupt/CPU usage used to scan and refresh the display
   Timer1.initialize( 2000/DISPLAYS_BPP );           //period in microseconds to call ScanDMD. Anything longer than 5000 (5ms) and you can see flicker.
   Timer1.attachInterrupt( ScanDMD );   //attach the Timer1 interrupt to ScanDMD which goes to dmd.scanDisplayBySPI()

   //clear/init the DMD pixels held in RAM
   dmd.clearScreen( WHITE );   //true is normal (all pixels off), false is negative (all pixels on)
   dmd.selectFont(Arial_Black_16);   // BOLD FONT

/*   dataCount = EEPROM.read(DATACOUNTADD);

   int i;
   for( i = 0; i < dataCount - 1; i++ )
   {
     inputBuffer[i] = EEPROM.read(i);   
     delay(1);
   }
   inputBuffer[i] = '\0';
   */
 
}

/*--------------------------------------------------------------------------------------
  loop
  Arduino architecture main loop
--------------------------------------------------------------------------------------*/

void loop(void)
{
  
    unsigned long time;
   int n;
  boolean ret = false;

/*   dmd.drawMarquee("Test",4,(32*DISPLAYS_ACROSS)-1,1); // set up the marquee
    time = millis();
    n=0;
  //  while(n<3) 
    {
      while (!ret) {
        if ((time+30) < millis()) 
        {
          ret = dmd.stepMarquee(-1,0); // parameters are the top & bottom rows to be scrolled
          time = millis();
        }
      }
      ret = false;
      n++;
    } */
 

   dmd.drawMarquee(inputBuffer,dataCount-1,(32*DISPLAYS_ACROSS)-1,1); // set up the marquee
    time = millis();
    n=0;
  //  while(n<3) 
    {
      while (!ret) {
        if ((time+30) < millis()) 
        {
          ret = dmd.stepMarquee(-1,0); // parameters are the top & bottom rows to be scrolled
          time = millis();
        }
      }
      ret = false;
      n++;
    }
     
   if(stringComplete == true)
   {
      for(int i = 0; i < dataCount - 1; i++ )
        EEPROM.write(i, inputBuffer[i]);    
        
        EEPROM.write(DATACOUNTADD, dataCount);  
 
       stringComplete = false;     
   }
   
   delay(5);
   
}


void serialEvent1() 
{
  while (Serial1.available()) 
  {
    // get the new byte:
    memset(inputBuffer, '\0', sizeof(inputBuffer));
    Serial1.setTimeout(1); // second input timeout
    dataCount = Serial1.readBytesUntil('\n', inputBuffer, sizeof(inputBuffer)-1); // Read line of input
    inputBuffer[dataCount] ='\0';
    Serial1.println(dataCount);
    delay(10);
    dmd.clearScreen( true );
    Serial1.println(inputBuffer);
    stringComplete = true;  
  }

}

