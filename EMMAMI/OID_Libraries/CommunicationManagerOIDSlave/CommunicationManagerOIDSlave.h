#ifndef  _COMMUNICATION_MANAGEROIDSLAVE_H_
#define  _COMMUNICATION_MANAGEROIDSLAVE_H_

/*-------------------------------------------------------------
* HEADER FILES
--------------------------------------------------------------*/
#include "Arduino.h"
#include "EEPROM.h"


/*-------------------------------------------------------------
* MACROS
--------------------------------------------------------------*/

#define MAX_SERIAL				  4

#define	RESPONSE_TIMEOUT		  1000		// 1 SEC
#define MAX_RETRIES				  3


/*-------------------------------------------------------------
* enums
--------------------------------------------------------------*/



typedef enum
{
	COMMAND_INDEX = 0,
	ID_INDEX,
	DATA_INDEX
}BUFFER_INDEX;

typedef enum
{
	COM_START = 0,
	COM_IN_PACKET_COLLECTION,
	COM_PACKET_RECEIVED
}STATE;

enum
{
	PARSE_FAILURE = 0,
	PARSE_SUCCESS
};	

/*-------------------------------------------------------------
* CLASS
--------------------------------------------------------------*/
class CommunicationManagerOIDSlave
{
	public:

		
		CommunicationManagerOIDSlave( );
	
		void configure(int baudRate);
	
		void task(bool* packetReceived );
		
		void transact( int command, int *data);	
		
		byte getCommand();
		byte getID();
		char** getFields( char* token, byte count);
		
		
	private:
	
		int 	cmd;
		int 	baudRate;
		STATE 	state;
		byte 	buffer[128];
		int 	Index ;

		
		char   *fields[10];

		
		
		
		
		//void sendData( );
		
		//byte responseReceived( );

		


};


#endif

