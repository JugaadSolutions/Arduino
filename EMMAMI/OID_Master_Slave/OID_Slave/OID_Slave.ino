




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

 - Place the DMD library folder into the "arduino/libraries/" folder of your Arduino installation.u
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
#include <DMDMegaMulti.h>        //
#include <TimerOne.h>   //
#include <EEPROM.h>
#include "CommunicationManagerOIDSlave.h"
#include "parameter.h"


#include "SystemFont5x7.h"
#include "Arial_black_16.h"

/*-------------------------------------------------------------
* macros
--------------------------------------------------------------*/
//#define FACTORY_CONFIGURATIONS

//Fire up the DMD library as dmd
#define DISPLAYS_ACROSS 4
#define DISPLAYS_DOWN 1
#define DISPLAYS_BPP 1
#define WHITE 0xFF
#define BLACK 0
#define DATACOUNTADD   200
typedef enum
{
	

	CMD_SET_MARQUEE_SEGMENT = 0x91,
	CMD_DISABLE_MARQUEE_SEGMENT = 0x92,


	CMD_SET_PARAMETER_VALUE = 0xE0,
	CMD_DISABLE_PARAMETER = 0xE1,
	CMD_SET_OFFSET = 0xE2,
	CMD_SET_SCALEFACTOR = 0xE3,
	CMD_CONFIGURE_PARAMETER = 0xE4,

	CMD_SET_SCROLLING_SPEED = 0xC0

}CMD;
#define MAX_PARAMETERS        5
#define MAX_FIELDS            5
#define MAX_DISPLAY_SIZE      (MAX_PARAMETERS)*( MAX_TAG_SIZE + MAX_UNIT_SIZE + (3 * MAX_LIMIT_SIZE) + 1)

#define EEPROM_MARQUEE_STEPINTERVAL_INDEX		  MAX_DISPLAY_SIZE + 1


DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN );

/*-------------------------------------------------------------
* Global variables
--------------------------------------------------------------*/

CommunicationManagerOIDSlave comSlave;
Parameter   parameters[MAX_PARAMETERS];

char disBuffer[MAX_DISPLAY_SIZE] = "";
int dataCount = strlen(disBuffer);

int updateTime = 0;
int updateInterval = 30;




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
  #if defined (FACTORY_CONFIGURATIONS)  
   //clear EEPROM
   for ( int i = 0 ; i < EEPROM.length() ; i++ )
	EEPROM.write(i, 0); 
   EEPROM.write(EEPROM_MARQUEE_STEPINTERVAL_INDEX, 30);
#endif
   int i;
  Serial2.begin(9600);
   
   comSlave.configure(9600);
   //initialize TimerOne's interrupt/CPU usage used to scan and refresh the display
   Timer1.initialize( 2000/DISPLAYS_BPP );           //period in microseconds to call ScanDMD. Anything longer than 5000 (5ms) and you can see flicker.
   Timer1.attachInterrupt( ScanDMD );   //attach the Timer1 interrupt to ScanDMD which goes to dmd.scanDisplayBySPI()

   //clear/init the DMD pixels held in RAM
   dmd.clearScreen( WHITE );   //true is normal (all pixels off), false is negative (all pixels on)
   dmd.selectFont(Arial_Black_16);   // BOLD FONT

 for(i = 1 ; i < MAX_PARAMETERS ; i++ )
 {
   parameters[i].readFromEEPROM(i);
   
 }

 for( i = 1 ; i < MAX_PARAMETERS ; i++)
 {
   if(parameters[i].active == true)
   {
             parameters[i].toString(disBuffer);
        
          
   }
 }

Serial.begin(9600);

 updateInterval = EEPROM.read(EEPROM_MARQUEE_STEPINTERVAL_INDEX);
 dataCount = strlen(disBuffer);
 dmd.drawMarquee(disBuffer,dataCount,(32*DISPLAYS_ACROSS)-1,1); // set up the marquee
 
}

/*--------------------------------------------------------------------------------------
  loop
  Arduino architecture main loop
--------------------------------------------------------------------------------------*/

void loop(void)
{
  
   unsigned long time;
   boolean packetReceived ;
   int n;
   boolean ret = false;
   
  comSlave.task ( &packetReceived );
   
   if(packetReceived == true)
   {
     processData();      
   }
   
   
   int currentTime = millis();
   
    if ((currentTime - updateTime) > updateInterval ) 
    {
      ret = dmd.stepMarquee(-1,0); // parameters are the top & bottom rows to be scrolled
      updateTime = currentTime;
      
    }
    if(ret)
    {
      disBuffer[0] = '\0';
      for( int i = 1 ; i < MAX_PARAMETERS ; i++)
     {
       if(parameters[i].active == true)
       {
          parameters[i].toString(disBuffer);
       }
     }
     dataCount = strlen(disBuffer);
     dmd.drawMarquee(disBuffer,dataCount,(32*DISPLAYS_ACROSS)-1,1); // set up the marquee
    }
}


  void processData()
  {

   byte command = 0;
   byte ID = 0;
    command = comSlave.getCommand();
    ID = comSlave.getID();
    
    // Serial2.write(command);
    // Serial2.write(ID);
     char **fields;    
    switch(command)
    {
      case CMD_SET_PARAMETER_VALUE:

		  fields = comSlave.getFields("|", 1);
                  //Serial.println(fields[0]);
		  parameters[ID].count = atoi(fields[0]);
                  //Serial.println(parameters[ID].count);
                //  Serial.print("\n");
		  parameters[ID].updateValue();

      break;
      
	  case CMD_CONFIGURE_PARAMETER:
        fields = comSlave.getFields("|", 5);
        
        memset(parameters[ID].tag,0 , MAX_TAG_SIZE);
         memset(parameters[ID].unit,0 , MAX_UNIT_SIZE);
          memset(parameters[ID].scaleFactor_str,0 , MAX_LIMIT_SIZE);
         memset(parameters[ID].offset_str,0 , MAX_LIMIT_SIZE);
         memset(parameters[ID].precision_str,0 , MAX_LIMIT_SIZE);
         
        strcpy(parameters[ID].tag,fields[0]);
        strcpy(parameters[ID].unit,fields[1]);
        strcpy(parameters[ID].scaleFactor_str, fields[2]);
        strcpy(parameters[ID].offset_str, fields[3]);
        strcpy(parameters[ID].precision_str,fields[4]);

        parameters[ID].scaleFactor = atof(fields[2]);
        parameters[ID].offset = atof(fields[3]);
        parameters[ID].precision = atoi(fields[4]);
        
        parameters[ID].active = true;
        
        parameters[ID].updateEEPROM(ID);
      
          
        break;    

	  case CMD_DISABLE_PARAMETER:
		  parameters[ID].active = false;
		  parameters[ID].updateEEPROM(ID);
		  break;

	case CMD_SET_SCROLLING_SPEED:
		fields = comSlave.getFields("|", 1);
		updateInterval = atoi(fields[0]);
		EEPROM.write(EEPROM_MARQUEE_STEPINTERVAL_INDEX, updateInterval);
		break;
    
    }
    
    //dmd.clearScreen( WHITE );   //true is normal (all pixels off), false is negative (all pixels on)
   
 
  }
