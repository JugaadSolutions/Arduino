#ifndef  _COMMUNICATION_MANAGER_H_
#define  _COMMUNICATION_MANAGER_H_

/*-------------------------------------------------------------
* HEADER FILES
--------------------------------------------------------------*/
#include "Arduino.h"
#include "Ethernet.h"
#include "EthernetServer.h"
#include "EEPROM.h"


/*-------------------------------------------------------------
* MACROS
--------------------------------------------------------------*/



#define MAX_SERIAL				  4

#define	RESPONSE_TIMEOUT		  1000		// 1 SEC
#define MAX_RETRIES				  3

#define STATE_CHANGE_COUNT			500       // 500 mSec

/*-------------------------------------------------------------
* enums
--------------------------------------------------------------*/
typedef enum 
{
  CMD_SET_IP = 0xD0,
  CMD_SET_MAC = 0xD1,
  CMD_SET_GATEWAY = 0xD2, 
  CMD_SET_SUBNET = 0xD3,

  CMD_GET_IP = 0xD4,
  CMD_GET_MAC = 0xD5,
  CMD_GET_GATEWAY = 0xD6,
  CMD_GET_SUBNET = 0xD7, 
  CMD_GET_MODEL = 0xD8,
  CMD_GET_SERIALNO = 0xD9,
  CMD_GET_CUSTOMERID = 0xDA
  
}MISCELLANEOUS;


typedef enum
{
	CMD_SET_MARQUEE_SEGMENT = 0x91,
	CMD_DISABLE_MARQUEE_SEGMENT = 0x92,


	CMD_SET_PARAMETER_VALUE = 0xE0, 
	CMD_DISABLE_PARAMETER = 0xE1,
	CMD_SET_OFFSET = 0xE2,
	CMD_SET_SCALEFACTOR = 0xE3,
	CMD_CONFIGURE_PARAMETER = 0xE4,

	CMD_SET_SCROLLING_SPEED = 0xC0
	
}CMD;

typedef enum
{
	COMMAND_INDEX = 1,
	DATA_INDEX = 2
}BUFFER_INDEX;

typedef enum
{
	IDLE = 0,
	TRANSACTION_START,
	IN_TRANSACTION
}STATE;



typedef enum
{
	SERIAL_0 = 0,
	SERIAL_1,
	SERIAL_2,
	SERIAL_3
};


/*-------------------------------------------------------------
* CLASS
--------------------------------------------------------------*/
class CommunicationManager
{
	public:
		EthernetClient Client;
		EthernetServer Server = EthernetServer(8888);
		
		CommunicationManager(int );
	
		void configure(byte *ip, byte *mac, byte* gateway, byte *subnet, int baudRate, int retries);
	
		void task(bool*);
		
		void getIP( byte *buffer );
		void getMAC( byte *buffer );
		byte getCommand();
		int getData(byte**);
		void transact( byte command,byte id, byte *data,byte length);	
		bool sendResponse(byte* buffer, int length);
		
	private:
		byte 	*buffer;  // buffer to hold tcp/ip data
		int 	cmd;
		int 	baudRate;
		byte 	*ip;	   // buffer to hold the ip address
		byte 	*mac;			   // buffer to hold the mac address
		byte	*gateway;	//pointer to gateway buffer
		byte	*subnet;	//pointer to subnet buffer

		STATE 	state;
		
		bool 	transactionStatus[MAX_SERIAL]; // flags used to know the status of the each serial
		int 	startTime;					// to hold the start time of the transaction
		byte	serialID;					
		byte 	retryCount;
		byte    retries;
		int		tcpPacketSize;			
		
		
		void sendData( );
		
		byte responseReceived( );
		


};



#endif