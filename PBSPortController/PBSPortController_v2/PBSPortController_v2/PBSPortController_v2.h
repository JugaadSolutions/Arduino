#ifndef _PBSPORTCONTROLLER_H_
#define	_PBSPORTCONTROLLER_H_

#include "Arduino.h"


#define MAX_SIZE 	50
#define USER_ID		1
//const char *val = "VALID";
//const char *inval = "INVALID";

typedef enum
{
		CHECKED_IN = 0,
		CHECKED_OUT,
		IN_PROCESS
}STATE;

typedef enum
{
	CMD_VALIDATE_USER = 0X80,
	CMD_VALIDATE_CYCLE
}COMMANDS;

enum
{
	VALID,
	INVALID
};


class PBSPORTCONTROLLER
{
	private:
	
	unsigned int 	baudRate;			// used to store the baudRate to set the UARTs
	STATE 			state;				// used to store three different states of the controller
	byte 			buffer[MAX_SIZE];  	// used to hold the RF-ID data
	char 			validateBuff[MAX_SIZE];
	unsigned int 	index;				// used as buffer index
	STATE 			previousState;		// used to store previous state in order to change the state
	
	const int 		checkInLED;
	const int 		checkOutLED;
	const int 		inProcessLED;
	const int		relay;
	
	bool 			response;
	bool			operationDone;
	byte 			validated;
	
	//Method used to send RF-ID data on UART-3
	void transact( byte command, byte id, byte *data, byte length );
	
	bool responseReceived( );
	
	byte validate();
	
	void process();
	
	public:
	
	//constructor
	PBSPORTCONTROLLER(const int led1, const int led2, const int led3, const int _relay):checkInLED(led1),checkOutLED(led2),inProcessLED(led3),relay(_relay)
	{
		pinMode( checkInLED, OUTPUT );
		pinMode( checkOutLED, OUTPUT );
		pinMode( inProcessLED, OUTPUT );
		pinMode( relay, OUTPUT );
		
		digitalWrite( checkInLED, LOW );
		digitalWrite( checkOutLED, HIGH );
		digitalWrite( inProcessLED, HIGH );
		digitalWrite( relay, LOW );
	}
	
	//Method used to set the baud rate of UARTs
	void configure( unsigned int baudRate );
	
	//Method used to handle the state transition
	void task();
	
	
	
};

#endif

