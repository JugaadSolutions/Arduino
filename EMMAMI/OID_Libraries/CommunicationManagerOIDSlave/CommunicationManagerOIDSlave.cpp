#include "CommunicationManagerOIDSlave.h"

CommunicationManagerOIDSlave::CommunicationManagerOIDSlave()
{
	
}

void CommunicationManagerOIDSlave::configure( int baudRate)
{
	this->baudRate = baudRate;

	Serial1.begin(baudRate);
	
	state = COM_START ;
}

void CommunicationManagerOIDSlave::task( bool* packetReceived )
{
	
	volatile  byte serialData = 0 ;
	*packetReceived = false;
	
	switch(state)
	{
		
		case COM_START:
		
			if( Serial1.available() > 0 )
			{
				serialData = Serial1.read( );				
			}
			
			if( serialData == 0xAA )
			{
				Index = 0;
				//Serial2.write(serialData);
				state = COM_IN_PACKET_COLLECTION;
				
			}
		break;
		
		case COM_IN_PACKET_COLLECTION :
		
			if( Serial1.available() > 0 )
			{
				serialData = Serial1.read( );				
			
			
					if(serialData == 0xBB )
					{
						
						*packetReceived = true;
						//Serial2.write(0xBB);						
						state = COM_PACKET_RECEIVED;
						Serial1.write(buffer[0]);			//send command byte as response

						//Serial.print((char*)&buffer[2]);
					}
					else
					{
						buffer[Index++] = serialData;
						//Serial.println(serialData);

					}
			
			}
			break;	
			
		
	
		break;

		default:
			Index = 0;

			state = COM_START;
		break;
			
	}	
			
	
}	

byte CommunicationManagerOIDSlave::getCommand()
{
	byte command = buffer[0];
	return command;
	
}
byte CommunicationManagerOIDSlave::getID()
{
	byte id = buffer[1];
	return id;
}
char** CommunicationManagerOIDSlave::getFields( char* token, byte count)
{
	//Serial2.println( count);
	
	if(state == COM_PACKET_RECEIVED)
	{
		for(int i = 0 ; i < count ; i++)
		{
			
			if(i == 0 )
			{
				fields[i] = strtok((char*)(buffer+DATA_INDEX),token);
				//Serial.println((char*)buffer + DATA_INDEX);
				
			}
			else				
			{
				fields[i] = strtok(NULL,token);
				//Serial2.println(fields[i]);
			}
		}
		char *temp;
		while (temp != NULL)
		{
			strtok(NULL, token);

		}
		Index = 0;
		state = COM_START;
		return fields;
	}
	else return NULL;


	
	
}

			
