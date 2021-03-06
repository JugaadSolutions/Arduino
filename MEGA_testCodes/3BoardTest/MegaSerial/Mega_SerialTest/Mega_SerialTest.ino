/*
  Mega multple serial test

 Receives from the main serial port, sends to the others.
 Receives from serial port 1, sends to the main serial (Serial 0).

 This example works only on the Arduino Mega

 The circuit:
 * Any serial device attached to Serial port 1
 * Serial monitor open on Serial port 0:

 created 30 Dec. 2008
 modified 20 May 2012
 by Tom Igoe & Jed Roach

 This example code is in the public domain.

 */


void setup() {
  // initialize both serial ports:
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
}

void loop() {
  // read from port 1, send to port 0:
  if (Serial.available()) {
    //int inByte = Serial1.read();

    int inByte = Serial.read();
    Serial1.write(inByte); 
    Serial2.write(inByte); 
  }

 
  if (Serial1.available()) {
    int inByte = Serial1.read();
    //Serial1.write(inByte);
    //Serial.write("MEGA");
    Serial.write(inByte);
  }

  if (Serial2.available()) {
    int inByte = Serial2.read();
    //Serial1.write(inByte);
    //    Serial.write("UNO");
    Serial.write(inByte);
  }
}
