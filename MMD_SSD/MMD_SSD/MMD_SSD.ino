#include "MMD.h"
#include "config.h"
#include "digitdisplay.h"
#include <TimerOne.h> 
#include <EEPROM.h>
#include <MsTimer2.h>



MMD_Config mmdConfig= {0};
//unsigned char line[] ="The Mega 2560 is a microcontroller board based on the ATmega2560. It has 54 digital input/output pins (of which 15 can be used as PWM outputs), 16 analog inputs, 4 UARTs (hardware serial ports), a 16 MHz crystal oscillator. "; 
UINT8 buf[700] = {0};
unsigned char line[] = "The Mega 2560 is a microcontroller board based on the ATmega2560. ";


UINT8 test[] = "12345678123456789012345678901234567890";

DIGITDISPLAY ssd( 32 );

void ScanSSD()
{ 
  ssd.task();
}

void setup() {
  
  Serial.begin( 9600 );
 
  // put your setup code here, to run once:
  MMD_init( buf );  // Display initialization
  
  Serial.println("MMD_init_completed");
  MMD_clearSegment(0);
  Serial.println("MMD_clearSegmentcompleted");  
  Serial.println(sizeof(int));
  
  mmdConfig.startAddress = 0;
  mmdConfig.len = MMD_MAX_CHARS;
  mmdConfig.symbolCount = strlen((const char *)line);
  mmdConfig.symbolBuffer = line;
  mmdConfig.scrollSpeed = SCROLL_SPEED_LOW;//SCROLL_SPEED_NONE;
    			
  MMD_configSegment( 0 , &mmdConfig);
  Serial.println("MMD_configured succesfully");

  Timer1.initialize( 2500 );           //period in microseconds to call ScanDMD. Anything longer than 5000 (5ms) and you can see flicker.
  Timer1.attachInterrupt( MMD_refreshDisplay );   //attach the Timer1 interrupt to ScanDMD which goes to dmd.scanDisplayBySPI()
  Serial.println("MMD_refreshDisplay");

  ssd.displayTest();
  
  ssd.updateBuffer( test );
  
  pinMode( 13, OUTPUT );
  
  MsTimer2::set(1, ScanSSD); // 500ms period
  MsTimer2::start();  
}

int incomingByte = 0; 
static long count = 5000;
static long hbCount = 100000;
bool hb = false;
bool dataReceived = false;

void loop() {
  // put your main code here, to run repeatedly:
  count++;
  hbCount++;
  
  if( count >= 500 )
  {
    MMD_task();
    count = 0;
  }

  if( hbCount >= 10000 )
  {
    if( hb == true )
    {
      digitalWrite( 13, HIGH );
      hb = false;
    }
    else if( hb == false )
    {
      digitalWrite( 13, LOW );
      hb = true;
    }
    hbCount = 0;
  }

 /*  if(dataReceived == true)
   {
      Serial.print("I received: ");
      Serial.println(incomingByte);      
     
     dataReceived = false;
   }
   */
  //delay(1);
}



void serialEvent() 
{
  while (Serial.available()) 
  {
      // read the incoming byte:
      incomingByte = Serial.read();
  
      Serial.write( incomingByte );
      Serial.write("\n");
      // say what you got:
     // Serial.print("I received: ");
     // Serial.println(incomingByte);  
      dataReceived = true;  
  }
  
}


