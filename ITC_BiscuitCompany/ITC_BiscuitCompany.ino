#include <DMD.h>
#include <SPI.h>
#include <TimerOne.h>   //
#include <EEPROM.h>

//LIBRARY USED FOR SCREEN SWITCHING
#include <MsTimer2.h>

//LIBRARY USED FOR PUSH BUTTON HANDLING
#include <PB.h>

#include "SystemFont5x7.h"
#include "Arial_black_16.h"
#include "Arial14.h"

////////////////////////////////////////////////////////////
//Fire up the DMD library as dmd
#define DISPLAYS_ACROSS       1
#define DISPLAYS_DOWN         1
#define DISPLAYS_BPP          1
#define WHITE                 0xFF
#define BLACK                 0

#define MAX_SCREENS           2

#define ACTUAL_ADD            200
#define CUMULATIVE_ADD        210
#define HOUR_ADD              220
#define SHIFT_ADD             221

#define SLAVE_ID              1

#define CMD_TARGET            0X70
#define CMD_MODEL             0X71
#define CMD_PING              0X75

#define DEBOUNE_COUNT         40      //in msec

enum
{
  STATIC = 0, 
  SCROLLING,
};

enum
{
  ACTUAL = 0,
  CUMULATIVE,
  HOUR,
  SHIFT
};

unsigned int holdingRegs[50]; // function 3 and 16 register array
char *ptr = (char *)&holdingRegs;
unsigned int dataCount = 0;
boolean dataReceived = false;

volatile byte screenCounter = 0;


class Screen
{
  public:
  char rowOneMessage[8];
  char rowTwoMessage[8];
};

DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN );
Screen screen[MAX_SCREENS];

//input pin configurations

int hourlyResetPB = 4;
int shiftResetPB = 3;
int sensorInput = 2;
PushButton hryRstPB = PushButton( DEBOUNE_COUNT, hourlyResetPB );
PushButton shiftRstPB = PushButton( DEBOUNE_COUNT, shiftResetPB );
PushButton sensInput = PushButton( DEBOUNE_COUNT, sensorInput );


/*--------------------------------------------------------------------------------------
  Interrupt handler for Timer1 (TimerOne) driven DMD refresh scanning, this gets
  called at the period set in Timer1.initialize();
--------------------------------------------------------------------------------------*/
void ScanDMD()
{ 
  dmd.scanDisplayBySPI();
}

void setup() {
  // put your setup code here, to run once:

     //initialize TimerOne's interrupt/CPU usage used to scan and refresh the display
   Timer1.initialize( 2000/DISPLAYS_BPP );           //period in microseconds to call ScanDMD. Anything longer than 5000 (5ms) and you can see flicker.
   Timer1.attachInterrupt( ScanDMD );   //attach the Timer1 interrupt to ScanDMD which goes to dmd.scanDisplayBySPI()
  
   //clear/init the DMD pixels held in RAM
   dmd.clearScreen( WHITE );   //true is normal (all pixels off), false is negative (all pixels on)
  
   //modbus_configure(&Serial, 9600, SERIAL_8N1, SLAVE_ID, 2, 50, holdingRegs);
  
    
   //timer two initializtion for screen switching
   MsTimer2::set(0, switchScreen); // 500ms period
   MsTimer2::start();
   
   /* SCREEN - 1 CONFIGURATIONS */
   //set default parameters for the screens
   screen[1].rowOneMessage[0] = 'H';
   screen[1].rowOneMessage[1] = 'r';//EEPROM.read(TARGET_ADD);
   screen[1].rowOneMessage[2] = ' ';
   screen[1].rowOneMessage[3] = ':';
   screen[1].rowOneMessage[4] = ' ';
   screen[1].rowOneMessage[5] = '1';

   //set default parameters for the screens
   screen[1].rowTwoMessage[0] = 'A';
   screen[1].rowTwoMessage[1] = ':';//EEPROM.read(TARGET_ADD);
   screen[1].rowTwoMessage[2] = '0';
   screen[1].rowTwoMessage[3] = '0';
   screen[1].rowTwoMessage[4] = '0';
   screen[1].rowTwoMessage[5] = '0';
  
   /* SCREEN - 2 CONFIGURATIONS */
   //set default parameters for the screens
   screen[0].rowOneMessage[0] = 'S';
   screen[0].rowOneMessage[1] = 'h';//EEPROM.read(TARGET_ADD);
   screen[0].rowOneMessage[2] = ' ';
   screen[0].rowOneMessage[3] = ':';
   screen[0].rowOneMessage[4] = ' ';
   screen[0].rowOneMessage[5] = '1';
   
      //set default parameters for the screens
   screen[0].rowTwoMessage[0] = 'C';
   screen[0].rowTwoMessage[1] = ':';//EEPROM.read(TARGET_ADD);
   screen[0].rowTwoMessage[2] = '0';
   screen[0].rowTwoMessage[3] = '0';
   screen[0].rowTwoMessage[4] = '0';
   screen[0].rowTwoMessage[5] = '0';
   
   readEEPROM();
  
   //sensor input
   //pinMode(sensorInput, INPUT);
   pinMode(sensorInput, INPUT_PULLUP);
   pinMode(A4, INPUT_PULLUP);
   pinMode(A5, INPUT_PULLUP);
   
   //pushbutton input
   pinMode( hourlyResetPB, INPUT_PULLUP );
   pinMode( shiftResetPB, INPUT_PULLUP );

}

unsigned long time = millis();
int n;
boolean ret = false;
boolean boolBit = true;
    
void loop() {
    
    handleInput();

    switch(screenCounter)
    {
      case 0:
        screenOne();
        //screenTwo();
      break;

      case 1:
        screenTwo();
      break;

      default:
      break;
    }

     
 }

void screenOne()
{
     dmd.selectFont(SystemFont5x7);
     //dmd.selectFont(Arial_14);
     dmd.drawString(0,8,screen[0].rowTwoMessage,sizeof(screen[0].rowTwoMessage),GRAPHICS_NORMAL); // the stationary string
     dmd.drawMarquee(screen[0].rowOneMessage,sizeof(screen[0].rowOneMessage),0,0);   
     
     if(screenCounter != 0)
       dmd.clearScreen( true );   
}

void screenTwo()
{
     dmd.selectFont(SystemFont5x7);
     //dmd.selectFont(Arial_14);
     dmd.drawString(0,8,screen[1].rowTwoMessage,sizeof(screen[1].rowTwoMessage),GRAPHICS_NORMAL); // the stationary string
     dmd.drawMarquee(screen[1].rowOneMessage,sizeof(screen[1].rowOneMessage),0,0);       
      
     //if(screenCounter != 1)
      // dmd.clearScreen( true );     
}



unsigned long actual = 0;
unsigned long cumulative = 0;
void readEEPROM( void )
{
  //read actual from EEPROM
  for(int i = 0; i < 6; i++ )
     screen[1].rowTwoMessage[i] = EEPROM.read(ACTUAL_ADD+i);

  actual = atoi(screen[1].rowTwoMessage+2);
  
  //read cumulative from EEPROM
  for(int i = 0; i < 6; i++)
     screen[0].rowTwoMessage[i] = EEPROM.read(CUMULATIVE_ADD+i);     
     
  cumulative = atoi(screen[0].rowTwoMessage+2);
     
  //get current hour from EEPROM
  screen[1].rowOneMessage[5] = EEPROM.read(HOUR_ADD); 
     
  //get current shift from EEPROM
  screen[0].rowOneMessage[5] = EEPROM.read(SHIFT_ADD);
}

void updateEEPROM( int type )
{
  switch( type )
  {
    case ACTUAL:
      for( int i = 0; i < 6; i++ )
        EEPROM.put(ACTUAL_ADD+i, screen[1].rowTwoMessage[i] );    
        
    break;
    
    case CUMULATIVE:
      for(int i = 0; i < 6; i++)
        EEPROM.put(CUMULATIVE_ADD+i, screen[0].rowTwoMessage[i]);
    break;
    
    case HOUR:
        EEPROM.put(HOUR_ADD, screen[1].rowOneMessage[5]);
    break;
    
    case SHIFT:
        EEPROM.put(SHIFT_ADD, screen[0].rowOneMessage[5]);
    break;
    
    default:
    break;
  }
 
}

void switchScreen()
{
  screenCounter++;
  
  if( screenCounter >= MAX_SCREENS )
    screenCounter = 0;
    
  //MsTimer2::modifyTime(screen[screenCounter].duration);
  MsTimer2::modifyTime(8000);
}


void handleInput()
{
  //hourly reset pb input handling
  sensInput.update();
  hryRstPB.update();
  shiftRstPB.update();
  
  if( sensInput.read() == false )
  { 
    //update the cumulative
    cumulative += 1;
    converToAscii( cumulative, screen[0].rowTwoMessage );
    updateEEPROM( CUMULATIVE );
    
    actual += 1;
    converToAscii( actual, screen[1].rowTwoMessage );
    updateEEPROM( ACTUAL );
  }
  
  if( hryRstPB.read() == false )
  {
    //increment the hour
//    if( screen[1].rowTwoMessage[5] >= '1' )
    {
      screen[1].rowOneMessage[5]++;
      
      if( screen[1].rowOneMessage[5] > '8' )
        screen[1].rowOneMessage[5] = '1';
        
      updateEEPROM( HOUR );
    }
    
    //reset the actual
    actual = 0;
    converToAscii( actual, screen[1].rowTwoMessage );
    updateEEPROM( ACTUAL );
}
  
  if( shiftRstPB.read() == false )
  {
    //increment shift 
    screen[0].rowOneMessage[5]++;
    
    if( screen[0].rowOneMessage[5] > '3' )
      screen[0].rowOneMessage[5] = '1';
      
    updateEEPROM( SHIFT );

    screen[1].rowOneMessage[5] = '1';
        
    updateEEPROM( HOUR );
    
    //reset the actual
    actual = 0;
    converToAscii( actual, screen[1].rowTwoMessage );
    updateEEPROM( ACTUAL );
    
    //reset cumulative
    cumulative = 0;
    converToAscii( cumulative, screen[0].rowTwoMessage );    
    updateEEPROM( CUMULATIVE );
  }
}

void converToAscii( int value, char *buf )
{
  for( int i = 0; i < 3; i++ )
  {
    buf[5-i] = value % 10 + '0';
    value /= 10;
  }

}






