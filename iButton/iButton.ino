#include <OneWire.h>

OneWire ds(12);
byte addr[8];
String tag = "01c36df1";
String keyStatus = "";


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  getKeyCode();
  
  if( keyStatus == "ok" )
  {
    byte i;
    String tmp = "";
    keyStatus ="";
    
    for( i = 5; i > 0; i-- )
    {
      //Serial.print(addr[i], HEX);
      tmp += String( addr[i], HEX );
    }
    
    Serial.println( tmp );
    
    if( tmp.equals( tag ) )
    {
      
      Serial.println( "OK" );
      
    }
    else
      Serial.println( "Invalid" );
      
      
  }
  else if( keyStatus != "")
  {
    Serial.print(keyStatus);
    Serial.println("NOK");
  }


  
  delay(1000);
}

void getKeyCode()
{
  byte present = 0;
  byte data[12];
  keyStatus = "";
  
  if( !ds.search(addr))
  {
    ds.reset_search();
    return;
  }
  
  if( OneWire::crc8( addr, 7) != addr[7] )
  {
    keyStatus = "CRC Invalid";
    return;
  }
  
  if( addr[0] != 0x01 )
  {
    keyStatus = "not DS1990A";
    return;
  }
  
  keyStatus = "ok";
  ds.reset();
}
