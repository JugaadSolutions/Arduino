#include "PBSPortController_v2.h"

void PBSPORTCONTROLLER :: configure( unsigned int baudRate )
{
	this->baudRate = baudRate;
	Serial.begin( baudRate );
	Serial2.begin( baudRate );
	Serial3.begin( baudRate );
	
	state = CHECKED_IN;
	index = 0;
} 

void PBSPORTCONTROLLER :: task()
{
	switch( state )
	{
		case CHECKED_IN:
		
			if( Serial2.available() )
			{
				while( Serial2.available() && index < 12 )
				{
					buffer[index] = Serial2.read();
					//Serial.write(buffer[index]);
					index++;
					delay(2);
				}
				
				buffer[index] = '\0';
			}
			
			if( index >= 12 )
			{
				digitalWrite( checkInLED, HIGH );
				digitalWrite( inProcessLED, LOW );
				
				transact( CMD_VALIDATE_USER, USER_ID, buffer, index );
				previousState = CHECKED_IN;
							
				index = 0;
			}
			else
				index = 0;
			
		break;
		
		case CHECKED_OUT:
		
			if( Serial1.available() )
			{
				while( Serial1.available() && index < 12 )
				{
					buffer[index++] = Serial1.read();
					delay(2);
				}
				
				buffer[index] = '\0';
			}
			
			if( index >= 12 )
			{
				digitalWrite( checkOutLED, HIGH );
				digitalWrite( inProcessLED, LOW );
				
				transact( CMD_VALIDATE_USER, USER_ID, buffer, index );				
				previousState = CHECKED_OUT;
				
				index = 0;
			}
			else
				index = 0;
			
		break;
		
		case IN_PROCESS:
		
			if( response == false )
				response = responseReceived();
			
			
			if( response == true && validated == 0XFF )				
				validated = validate();
						
			if( validated == VALID )
				process();
			else if( validated == INVALID )
				state = previousState;

		break;
		
		default:
		break;
	}
}

void PBSPORTCONTROLLER :: transact( byte command, byte id, byte *data, byte length )
{
			Serial.write( command );
			Serial.write( id );
	
			for( int i = 0; i < length; i++ )
			{
				Serial.write( *(data+i) );
				delay(10);
			}

			state = IN_PROCESS;
			response = false;
			validated = 0XFF;
			
			memset( validateBuff, '\0', sizeof(validateBuff) );
			//Serial.println("IN_PROCESS");	
	switch( state )
	{
		case CHECKED_IN:
			Serial2.end();
			Serial1.begin(baudRate);
		break;
		
		case CHECKED_OUT:		
			Serial1.end();
			Serial2.begin(baudRate);
		
		break;
	}
}


bool PBSPORTCONTROLLER :: responseReceived()
{
	bool response = false;
	int i = 0;
	
	if(Serial.available())
	{
		while( Serial.available() )
		{
			validateBuff[i++] = Serial.read();
		}
		validateBuff[i] ='\0';
		response = true;
		Serial.println(validateBuff);		
	}

	return response;
}

byte PBSPORTCONTROLLER :: validate()
{
	byte value = 0xff;
	char buffer[]= "VALID";
	char buffer1[] = "INVALID";
	int temp = strcmp(validateBuff, buffer);
	if(  temp == 0)
	{
		value = VALID;	
	}
	else if( strcmp(validateBuff, buffer1) == 0 )
	{
		value = INVALID;
	}
/*	else 
	{
		Serial.println("ERROR");
	}
	*/
	
	return value;
	
}

void PBSPORTCONTROLLER :: process()
{
	digitalWrite(relay, HIGH);
	delay(5000);
	digitalWrite(relay, LOW);		
	
	switch( previousState )
	{
		case CHECKED_IN:
		
			digitalWrite(checkOutLED, LOW);
			digitalWrite(inProcessLED, HIGH);		

			Serial2.end();
			Serial1.begin(baudRate);
			
			state = CHECKED_OUT;
			
		break;
		
		case CHECKED_OUT:		

			digitalWrite(checkInLED, LOW);
			digitalWrite(inProcessLED, HIGH);		

			Serial1.end();
			Serial2.begin(baudRate);
			
			state = CHECKED_IN;
			
		break;
		
		default:
		break;
	}
}
