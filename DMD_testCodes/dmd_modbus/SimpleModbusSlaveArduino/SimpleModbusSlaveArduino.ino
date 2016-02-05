#include <SimpleModbusSlave.h>
#include <DMD.h>
#include <SPI.h>
#include <TimerOne.h>   //
#include <EEPROM.h>

#include "SystemFont5x7.h"
#include "Arial_black_16.h"

/* 
   SimpleModbusSlaveV10 supports function 3, 6 & 16.
   
   This example code will receive the adc ch0 value from the arduino master. 
   It will then use this value to adjust the brightness of the led on pin 9.
   The value received from the master will be stored in address 1 in its own
   address space namely holdingRegs[].
   
   In addition to this the slaves own adc ch0 value will be stored in 
   address 0 in its own address space holdingRegs[] for the master to
   be read. The master will use this value to alter the brightness of its
   own led connected to pin 9.
   
   The modbus_update() method updates the holdingRegs register array and checks
   communication.

   Note:  
   The Arduino serial ring buffer is 64 bytes or 32 registers.
   Most of the time you will connect the arduino to a master via serial
   using a MAX485 or similar.
 
   In a function 3 request the master will attempt to read from your
   slave and since 5 bytes is already used for ID, FUNCTION, NO OF BYTES
   and two BYTES CRC the master can only request 58 bytes or 29 registers.
 
   In a function 16 request the master will attempt to write to your 
   slave and since a 9 bytes is already used for ID, FUNCTION, ADDRESS, 
   NO OF REGISTERS, NO OF BYTES and two BYTES CRC the master can only write
   54 bytes or 27 registers.
 
   Using a USB to Serial converter the maximum bytes you can send is 
   limited to its internal buffer which differs between manufactures. 
*/


////////////////////////////////////////////////////////////
//Fire up the DMD library as dmd
#define DISPLAYS_ACROSS 2
#define DISPLAYS_DOWN 1
#define DISPLAYS_BPP 1
#define WHITE 0xFF
#define BLACK 0
#define DATACOUNTADD   100
#define  SLAVE_ID  1

enum
{
  STATIC = 0, 
  SCROLLING
};

unsigned int holdingRegs[50]; // function 3 and 16 register array
char *ptr = (char *)&holdingRegs;
char buf[50];
unsigned int dataCount = 0;
byte scrolling = SCROLLING;
byte splitMarqueeScrolling = STATIC;
boolean dataReceived = false;
int msgLength = 0;

DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN );

/*--------------------------------------------------------------------------------------
  Interrupt handler for Timer1 (TimerOne) driven DMD refresh scanning, this gets
  called at the period set in Timer1.initialize();
--------------------------------------------------------------------------------------*/
void ScanDMD()
{ 
  dmd.scanDisplayBySPI();
}

void setup()
{
  /* parameters(HardwareSerial* SerialPort,
                long baudrate, 
		unsigned char byteFormat,
                unsigned char ID, 
                unsigned char transmit enable pin, 
                unsigned int holding registers size,
                unsigned int* holding register array)
  */
  
  /* Valid modbus byte formats are:
     SERIAL_8N2: 1 start bit, 8 data bits, 2 stop bits
     SERIAL_8E1: 1 start bit, 8 data bits, 1 Even parity bit, 1 stop bit
     SERIAL_8O1: 1 start bit, 8 data bits, 1 Odd parity bit, 1 stop bit
     
     You can obviously use SERIAL_8N1 but this does not adhere to the
     Modbus specifications. That said, I have tested the SERIAL_8N1 option 
     on various commercial masters and slaves that were suppose to adhere
     to this specification and was always able to communicate... Go figure.
     
     These byte formats are already defined in the Arduino global name space. 
  */
  
     //initialize TimerOne's interrupt/CPU usage used to scan and refresh the display
   Timer1.initialize( 2000/DISPLAYS_BPP );           //period in microseconds to call ScanDMD. Anything longer than 5000 (5ms) and you can see flicker.
   Timer1.attachInterrupt( ScanDMD );   //attach the Timer1 interrupt to ScanDMD which goes to dmd.scanDisplayBySPI()
  
   //clear/init the DMD pixels held in RAM
   dmd.clearScreen( WHITE );   //true is normal (all pixels off), false is negative (all pixels on)
   dmd.selectFont(Arial_Black_16);   // BOLD FONT
	
  modbus_configure(&Serial, 9600, SERIAL_8N1, SLAVE_ID, 2, 50, holdingRegs);

  // modbus_update_comms(baud, byteFormat, id) is not needed but allows for easy update of the
  // port variables and slave id dynamically in any function.
  //modbus_update_comms(9600, SERIAL_8E1, SLAVE_ID);

    memset(buf, '\0', sizeof(buf));
   int i = 0;
/*   do
   {
     buf[i] = EEPROM.read(i);   
     //delay(1);
     i++;
   }while(buf[i] != '\0');
*/

  msgLength = EEPROM.read(DATACOUNTADD);
  
  for( i = 0; i < msgLength; i++ )
     buf[i] = EEPROM.read(i);
}

void loop()
{
    unsigned long time;
    int n;
    boolean ret = false;
 
  // modbus_update() is the only method used in loop(). It returns the total error
  // count since the slave started. You don't have to use it but it's useful
  // for fault finding by the modbus master.

  dataReceived = modbus_update(&dataCount);
  
  if( dataReceived == true && dataCount != 0 )
  {
    int i;
    int j = dataCount;

    memset(buf, '\0', sizeof(buf));

    //store the modbus data into the buffer
    for(  i = 0; i < j/2; i++ )
    {
      buf[i*2] = *(ptr+((i*2)+1));
      buf[((i*2)+1)] = *(ptr+(i*2));
      msgLength++;
    }
    
    msgLength *=2;
    
    //store the modbus data into the eeprom
    for(  i = 0; i < j; i++ )
      EEPROM.put(i, buf[i]);
    
    EEPROM.write(i, '\0');
      
     EEPROM.write(DATACOUNTADD, msgLength);
     
     dataReceived = false;
     dmd.clearScreen( true );      
  }
 
   dmd.selectFont(Arial_Black_16);   // BOLD FONT 
   if(scrolling == SCROLLING)
   {
      dmd.drawMarquee(buf,sizeof(buf),(32*DISPLAYS_ACROSS)-1,1); // set up the marquee
      time = millis();
      n=0;
      while(n<3) 
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
   }
   else if(scrolling == STATIC)
     dmd.drawString(0, 0, buf, msgLength,GRAPHICS_NORMAL); 
     
  dmd.clearScreen( true );
  //time = millis();
     
   dmd.selectFont(SystemFont5x7);
 /*  if(splitMarqueeScrolling == SCROLLING)
   {
      // Show some stationary text on the bottom half, and scrolling text on the top half
      // scrolls 3 times
      dmd.drawString(0,8,buf,2,GRAPHICS_NORMAL); // the stationary string
      dmd.drawMarquee(buf+2,2,(32*DISPLAYS_ACROSS)-1,0); // set up the marquee
      time = millis();
      n=0;
      while(n<3) {
        while (!ret) {
          if ((time+30) < millis()) {
            ret = dmd.stepSplitMarquee(0,7); // parameters are the top & bottom rows to be scrolled
            time = millis();
          }
        }
        ret = false;
        n++;
      }
   }
   else if(splitMarqueeScrolling == STATIC)
   */
   {
       time = millis();
       while((millis() - time) < 10000)
       {
        dmd.drawString(0,8,buf,2,GRAPHICS_NORMAL); // the stationary string
        //dmd.drawString(0,13,buf+2,2,GRAPHICS_NORMAL); // the stationary string 
         dmd.drawMarquee(buf+2,2,0,0); 
       } 
   }
   
   dmd.clearScreen( true );
  
}

