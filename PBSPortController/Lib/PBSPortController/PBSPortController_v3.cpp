#include "PBSPortController_v3.h"

void PBSPORTCONTROLLER :: configure( unsigned int baudRate )
{
	this->baudRate = baudRate;

	Serial.begin( baudRate );
	Serial2.begin( baudRate );
	
	modbusSlave.modbus_configure(&Serial3, baudRate, SERIAL_8N1, USER_ID, 2, sizeof(mbBuf), mbBuf); 
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
				
				transact( CMD_CHECKED_IN, USER_ID, buffer, index );
				
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
				
				transact( CMD_CHECKED_OUT, USER_ID, buffer, index );	
				
				previousState = CHECKED_OUT;
				
				index = 0;
			}
			else
				index = 0;
			
		break;
		
		case IN_PROCESS:
		
	/*		if( response == false )
				response = responseReceived();
			
			
			if( response == true && validated == 0XFF )				
				validated = validate();
						
			if( validated == VALID )
				process();
			else if( validated == INVALID )
				state = previousState;
	*/
	
			dataReceived = modbusSlave.modbus_update(&dataCount, &functionCode);

			if( dataReceived == true && dataCount != 0 )
			{
				Serial.print( "function code " );
				Serial.write(functionCode+'0');
				Serial.write("\n");
				
				if( functionCode == 3 )
				{	
					if( log.readIndex < log.writeIndex )
					{
						Serial.print( "INSIDE FUNCTION 3" );
						modbusSlave.sendResponse(log.entries[log.readIndex], dataCount, functionCode );
						log.readIndex++;
						if( log.readIndex >= MAX_LOG_ENTRIES )
							log.readIndex = 0;
					}
				}
				else if( functionCode == 16 )
				{
					process();
				}
				dataReceived = false;
			}
			break;
		
		default:
		break;
	}
}

void PBSPORTCONTROLLER :: transact( byte command, byte id, byte *data, byte length )
{
	log.updateLog( data, command, length, id );
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
	
	state = IN_PROCESS;
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
	if( mbBuf[0] == true )
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
	else if( mbBuf[0] == 0 )
	{
		state = previousState;
	}
}


/*

LOG CLASS

*/

LOG:: LOG()
{
	writeIndex = 0;
	readIndex = 0;
}

void LOG:: updateLog(unsigned char* data, unsigned char command, byte len, byte ID )
{
	unsigned char i = 0;
	
	//store command in the log entry
	entries[writeIndex][0] = (unsigned short) command  << 8;
	Serial.write(command);
	
	//store the unit ID
	entries[writeIndex][0] |= (unsigned short)ID;
	Serial.write(command);
		
	for( i = 0 ; i < len/2; i++ )
	{
		entries[writeIndex][i+1] = (unsigned short)( *( data + i*2 ) << 8 );
		Serial.write(*( data + i*2 ));
		entries[writeIndex][i+1] |= (unsigned short)( *( data + (i*2)+1 ) );
		Serial.write(*( data + i*2 + 1 ));
	}
	Serial.write("\n");
	entries[writeIndex][i+1]= '\0';
	writeIndex++;
	if( writeIndex >= MAX_LOG_ENTRIES)
		writeIndex = 0;	
}

unsigned short LOG:: getStatusLog(unsigned char *buffer, unsigned char length)
{
/*	entry = entries[readIndex];
		
	readIndex++;
	if( readIndex >= MAX_LOG_ENTRIES )
		readIndex = 0;
*/	
	return (1);	
}

