#include <mbSlave.h>
#include <EEPROM.h>
#include "PBSPortController_v3.h"


////////////////////////////////////////////////////////////
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
boolean dataReceived = false;
int msgLength = 0;

/*-------------------------------------------------------------
* Global variables
--------------------------------------------------------------*/
const int checkInLed = 4;
const int checkOutLed = 5;
const int processLed = 6;
const int Relay = 7;

PBSPORTCONTROLLER pbs(checkInLed,checkOutLed,processLed,Relay);
/*--------------------------------------------------------------------------------------
void setup()
--------------------------------------------------------------------------------------*/


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

  //modbus_configure(&Serial, 9600, SERIAL_8N1, SLAVE_ID, 2, 50, holdingRegs,tatusLog);

  // modbus_update_comms(baud, byteFormat, id) is not needed but allows for easy update of the
  // port variables and slave id dynamically in any function.
  //modbus_update_comms(9600, SERIAL_8E1, SLAVE_ID);
  //Serial.begin( 9600 );
  //pbs.configure(9600);
  
    pbs.configure(9600);
}

void loop()
{

  pbs.task();
  
   // delay(20);
   
}

