#include "PBSPortController_v2.h"
#include <mbSlave.h>


/*-------------------------------------------------------------
* Global variables
--------------------------------------------------------------*/
const int checkInLed = 4;
const int checkOutLed = 5;
const int processLed = 6;
const int Relay = 7;

PBSPORTCONTROLLER pbs(checkInLed,checkOutLed,processLed,Relay);



void setup() {
  // put your setup code here, to run once:
  pbs.configure(9600);
  //Serial.begin(9600);
  //Serial2.begin( 9600 );
}

void loop() {
  // put your main code here, to run repeatedly:
  pbs.task();
  

  delay(20);
}
