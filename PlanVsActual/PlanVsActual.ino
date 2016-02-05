#include <DMD.h>
#include <SPI.h>
#include <TimerOne.h>   //
#include <EEPROM.h>

#include <SimpleModbusSlave.h>
#include <MsTimer2.h>

#include <Wire.h>
#include <RTClib.h>

#include "SystemFont5x7.h"
#include "Arial_black_16.h"
#include "Arial14.h"

//LIBRARY USED FOR PUSH BUTTON HANDLING
#include <PB.h>


////////////////////////////////////////////////////////////
//Fire up the DMD library as dmd
#define DISPLAYS_ACROSS       1
#define DISPLAYS_DOWN         1
#define DISPLAYS_BPP          1
#define WHITE                 0xFF
#define BLACK                 0

#define MAX_SCREENS           2
#define DATACOUNT_ADD         25*2
#define MESSAGE_OFFSET_ADD    25
#define TARGET_ADD            100

#define SLAVE_ID              1


#define CMD_TARGET            0X70
#define CMD_MODEL             0X71
#define CMD_PING              0X75


#define DEBOUNE_COUNT         40 


enum
{
  STATIC = 0, 
  SCROLLING,
};

unsigned int holdingRegs[50]; // function 3 and 16 register array
char *ptr = (char *)&holdingRegs;
unsigned int dataCount = 0;
boolean dataReceived = false;

volatile byte screenCounter = 0;

char actual = '0';
char previousInput = '0';

class Screen
{
  public:
  char message[25];
  char message2[8];
  int messageLength;
};


int sensorInput = 2;
PushButton sensInput = PushButton( DEBOUNE_COUNT, sensorInput );

DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN );
Screen screen[MAX_SCREENS];
RTC_DS1307 rtc;
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
  
   modbus_configure(&Serial, 9600, SERIAL_8N1, SLAVE_ID, 2, 50, holdingRegs);
  

   //readEEPROM();

    
   //timer two initializtion for screen switching
   MsTimer2::set(10000, switchScreen); // 500ms period
   MsTimer2::start();
   
   //set default parameters for the screens
   screen[1].message2[0] = 'P';
   screen[1].message2[1] = '9';//EEPROM.read(TARGET_ADD);
   screen[1].message2[2] = ' ';
   screen[1].message2[3] = 'A';
   screen[1].message2[4] = actual;
   screen[1].message2[5] = ' ';
   
   screen[1].message[0] = 'M';
   screen[1].message[1] = '0';
   screen[1].message[2] = 'D';
   screen[1].message[3] = 'E';
   screen[1].message[4] = 'L';
   screen[1].message[5] = ' ';
   screen[1].message[6] = '\0';
   
   //set colon for time 
   screen[0].message2[2] = ':';
   
   //sensor input
   //pinMode(sensorInput, INPUT);
   pinMode(sensorInput, INPUT_PULLUP);
   pinMode(A4, INPUT_PULLUP);
   pinMode(A5, INPUT_PULLUP);
   
   //Initialize I2C and RTC
   Wire.begin();
   rtc.begin();
   //rtc.adjust(DateTime(2016, 01, 18, 10, 40, 55));   
}

unsigned long time = millis();
int n;
boolean ret = false;
boolean boolBit = true;
    
void loop() {
  
    dataReceived = modbus_update(&dataCount);
  
    collectMBdata();
    
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

    //get RTC Data
    DateTime now = rtc.now();  
    unsigned char minutes = now.minute();
    unsigned char hours = now.hour();

    //convert BCD into ASCII
    screen[0].message2[4] = (minutes & 0x0f) + '0' ;
    screen[0].message2[3] = ((minutes & 0xf0) >> 4) + '0';

    screen[0].message2[1] = (hours & 0x0f) + '0' ;
    screen[0].message2[0] = ((hours & 0xf0) >> 4) + '0';
    
    //code for blinking the leds
    if(millis() >= time+500)
    {
      if( boolBit == true )
      {
        boolBit = false;
        screen[0].message2[2] = 33;
      }
      else if( boolBit == false )
      {
        boolBit = true;
        screen[0].message2[2] = ':';
      }
      time = millis();
    }
          
    dmd.selectFont(Arial_14);   // BOLD FONT
    dmd.drawString(1, 2, screen[0].message2, sizeof(screen[0].message2),GRAPHICS_NORMAL); 
 
    if(screenCounter != 0)
       dmd.clearScreen( true );   
   
 

}

void screenTwo()
{

     dmd.selectFont(SystemFont5x7);
     //dmd.selectFont(Arial_14);
     dmd.drawString(0,8,screen[1].message2,sizeof(screen[1].message2),GRAPHICS_NORMAL); // the stationary string
     dmd.drawMarquee(screen[1].message,sizeof(screen[1].message),(32*DISPLAYS_ACROSS)-1,0);       

     time = millis();
     n=0;
     //while(n<2) 
     {
       while (!ret) {
        if ((time+70) < millis()) 
        {
           handleInput();
           if( previousInput != actual )
           {
             previousInput = actual;
             dmd.drawString(0,8,screen[1].message2,sizeof(screen[1].message2),GRAPHICS_NORMAL); // the stationary string                       
             dmd.stepSplitMarquee(0,7); 
           }

           ret = dmd.stepSplitMarquee(0,7); // parameters are the top & bottom rows to be scrolled               
           time = millis();

        }
      }
      ret = false;
      n++;
    }
      
   if(screenCounter != 1)
      dmd.clearScreen( true );     

}

void collectMBdata()
{
  if( dataReceived == true && dataCount != 0 )
  { 
       //disable screen switching timer
      MsTimer2::Stop();   
      
      int i;
      
      //Two bytes are deducted from the dataCount
      //Since first two bytes contains information about screen number and command
      int j = dataCount-2;
      
      int screenNo = ptr[1];
      
      if(screenNo == 0 || screenNo == CMD_PING)
        return;
        
      //check for screen-1 and screen-2 data
      if(screenNo == 0x01 || screenNo == 0x02)
      {      
        int command = ptr[0];
        int data = ptr[3];
        
        switch( command )
        {
          case CMD_TARGET:
              //screen[screenNo-1].scrolling = ptr[3];
              //EEPROM.put((PARAMETER_ADD+(screenNo-1*10)+1), ptr[3]);
              screen[1].message2[0] = 'P';
              screen[1].message2[1] = ptr[3];
              screen[1].message2[2] = ' ';
              screen[1].message2[3] = 'A';
              screen[1].message2[3] = '0';
              EEPROM.put(TARGET_ADD, ptr[3]);
              
          break;
          
          case CMD_MODEL:
          
              memset(screen[1].message, '\0', sizeof(screen[1].message));
              //store the modbus data into the buffer
              for(  i = 0; i < j/2; i++ )
              {
                screen[1].message[i*2] = *(ptr+((i*2)+1+2));
                screen[1].message[((i*2)+1)] = *(ptr+(i*2)+2);
              }
              
              //store the modbus data into the eeprom
              for(  i = 0; i < j; i++ )
                EEPROM.put(i, screen[1].message[i]);

              EEPROM.write(i, '\0');
              
              EEPROM.write(DATACOUNT_ADD, j);

              screen[1].messageLength = j;
              
          break;
              
          default:
          break;   
        }
           
      }

      
      dataReceived = false;
      dmd.clearScreen( true );  
      
      //Enable screen switching timer
      MsTimer2::modifyTime(0);
      MsTimer2::start();   
      
  }
 
}

void readEEPROM( void )
{

    //clear buffer before restoring data
  memset(screen[1].message, '\0', sizeof(screen[1].message));
  
  //restore the data from the eeprom
  int  msgLength = EEPROM.read(DATACOUNT_ADD);

  for(int i = 0; i < msgLength; i++ )
     screen[1].message[i] = EEPROM.read(i);
     
  screen[1].messageLength = msgLength;

}

void switchScreen()
{
  screenCounter++;
  
  if( screenCounter >= MAX_SCREENS )
    screenCounter = 0;
    
  //MsTimer2::modifyTime(screen[screenCounter].duration);
  MsTimer2::modifyTime(8000);
}

boolean previousState = HIGH ;
void handleInput()
{
  
  sensInput.update();
  
  if( sensInput.read() == false )
  {
    
    Serial.println("Input received");
    actual++;
    
    if( actual > '9' )
      actual = '0';
      
    screen[1].message2[4] = actual;
    previousState = LOW;
   
  }
}


