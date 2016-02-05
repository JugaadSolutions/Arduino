#include <Ethernet.h>
#include <SPI.h>
#include <EEPROM.h>

#include "communicationManager.h"

/*-------------------------------------------------------------
* macros
--------------------------------------------------------------*/
#define FACTORY_CONFIGURATIONS

#define MODELBUFFER_SIZE		  20
#define SERIALNOBUFFER_SIZE		  20
#define CUSTOMERIDBUFFER_SIZE	          100
#define MACBUFFER_SIZE                    6
#define IPBUFFER_SIZE                     4
#define GATEWAYBUFFER_SIZE		  4
#define SUBNETBUFFER_SIZE		  4
#define MAX_BUFFER_SIZE			  200

#define EEPROM_MODEL_INDEX		  0
#define EEPROM_SERIALNO_INDEX	          EEPROM_MODEL_INDEX + MODELBUFFER_SIZE
#define EEPROM_CUSTOMERID_INDEX           EEPROM_SERIALNO_INDEX + SERIALNOBUFFER_SIZE
#define EEPROM_MAC_INDEX		  EEPROM_CUSTOMERID_INDEX + CUSTOMERIDBUFFER_SIZE
#define EEPROM_IP_INDEX       	          EEPROM_MAC_INDEX + MACBUFFER_SIZE 
#define EEPROM_GATEWAY_INDEX	          EEPROM_IP_INDEX  + IPBUFFER_SIZE
#define EEPROM_SUBNET_INDEX               EEPROM_GATEWAY_INDEX + GATEWAYBUFFER_SIZE


#define BAUD_RATE             9600
#define MAX_ANALOG_CHANNEL    5

/*-------------------------------------------------------------
* Global variables
--------------------------------------------------------------*/
CommunicationManager com(MAX_BUFFER_SIZE);
bool readChannel = false;
byte currentChannel = 0;

// These constants won't change.  They're used to give names
// to the pins used:
const int analogInPin_1 = A0;  // Analog input pin that the potentiometer is attached to
const int analogInPin_2 = A1;
const int analogInPin_3 = A2;
const int analogInPin_4 = A3;

uint16_t count = 0;

int ChannelUpdateTimestamp = 0;

char MODEL[MODELBUFFER_SIZE] = "IDEONICS - OID_V1";
char SERIALNO[SERIALNOBUFFER_SIZE] = "201511015001";
char CUSTOMERID[CUSTOMERIDBUFFER_SIZE] = "EMAMI PAPER MILLS BALASORE";

byte MAC[] = { 0xDE, 0xA9, 0xBE, 0xA3, 0xFE, 0xED };

//the IP address for the shield:
byte IP[] = { 192, 168, 14, 139 };

byte GATEWAY[] = { 192, 168, 14, 1 };

byte SUBNET[] = { 255, 255, 255, 0 };

int tempCount = 235;

uint8_t temp[12];
uint8_t temp1[12];
uint8_t t[3] ={0x32,0x33,0x35};
/*-------------------------------------------------------------
* void setup()
--------------------------------------------------------------*/
void setup() {

#if defined (FACTORY_CONFIGURATIONS)  
	FactoryConfiguation();
#endif
       // pinMode(4,OUTPUT);
       // digitalWrite(4,HIGH);
	Restore();

	com.configure(IP, MAC, GATEWAY, SUBNET, BAUD_RATE, 10);

        analogReference(EXTERNAL);
        delay(200);

}

void loop() {
	bool processingRequired = false;
	com.task(&processingRequired);

	if (processingRequired)
	{
		processData();
		return;
	}


	int currentTime = millis();

	if ((currentTime - ChannelUpdateTimestamp) > 500)
	{
		count = 0;
          switch(currentChannel)
          {
            case 1:
				for (int r = 0; r < 10; r++)
				{
					count += analogRead(analogInPin_1);
					delay(20);
				}
				count /= 10;
        
             
         
              break;
        	case 2 :
				for (int r = 0; r < 5; r++)
				{
					count += analogRead(analogInPin_2);
					delay(50);
				}
				count /= 5;
      	       break;
      
      	case 3:
			for (int r = 0; r < 5; r++)
			{
				count += analogRead(analogInPin_3);
				delay(50);
			}
			count /= 5;
              
      	break;
      
      	case 4:
			for (int r = 0; r < 5; r++)
			{
				count += analogRead(analogInPin_4);
				delay(50);
			}
			count /= 5;
                  
      	break;
      
      	default:
               currentChannel= 1;
      	return;
      			
      	}
/*
        Serial.print(currentChannel);
        Serial.print("\t");
        Serial.print(count);
        Serial.print("\n");
        */
        
        
        int tCount = count;
             
             int i = 0;
             while(tCount)
             {
               temp[i++] = tCount %10 + '0';
               tCount /= 10;
             }
             
            
         for(int j = 0; j < i ; j++)
         {
           temp1[j] = temp[i-j-1];
           Serial.write(temp1[j]);
         }
        
        com.transact((byte) CMD_SET_PARAMETER_VALUE, (byte)currentChannel, temp1, i);
           
	ChannelUpdateTimestamp = currentTime;
        currentChannel++;
		
  }

}


void FactoryConfiguation()
{
	//clear EEPROM
	for (int i = 0; i < EEPROM.length(); i++)
		EEPROM.write(i, 0);

	for (int i = 0; i < MODELBUFFER_SIZE; i++)
		EEPROM.write(i + EEPROM_MODEL_INDEX, MODEL[i]);

	for (int i = 0; i < SERIALNOBUFFER_SIZE; i++)
		EEPROM.write(i + EEPROM_SERIALNO_INDEX, SERIALNO[i]);

	for (int i = 0; i < CUSTOMERIDBUFFER_SIZE; i++)
		EEPROM.write(i + EEPROM_CUSTOMERID_INDEX, CUSTOMERID[i]);

	//Write default MAC Address into EEPROM
	for (int i = 0; i < MACBUFFER_SIZE; i++)
		EEPROM.write(i + EEPROM_MAC_INDEX, MAC[i]);

	//Write default IP Address into EEPROM
	for (int i = 0; i < IPBUFFER_SIZE; i++)
		EEPROM.write(i + EEPROM_IP_INDEX, IP[i]);

	//Write default GATEWAY  into EEPROM
	for (int i = 0; i < GATEWAYBUFFER_SIZE; i++)
		EEPROM.write(i + EEPROM_GATEWAY_INDEX, GATEWAY[i]);

	//Write default SUBNET  into EEPROM
	for (int i = 0; i < SUBNETBUFFER_SIZE; i++)
		EEPROM.write(i + EEPROM_SUBNET_INDEX, SUBNET[i]);

}

void Restore()
{

	for (int i = 0; i < MODELBUFFER_SIZE; i++)
		MODEL[i] = EEPROM.read(i + EEPROM_MODEL_INDEX);
	

	for (int i = 0; i < SERIALNOBUFFER_SIZE; i++)
	
		SERIALNO[i] = EEPROM.read(i + EEPROM_SERIALNO_INDEX);
		

	for (int i = 0; i < CUSTOMERIDBUFFER_SIZE; i++)
		CUSTOMERID[i] = EEPROM.read(i + EEPROM_CUSTOMERID_INDEX);

	//Write default MAC Address into EEPROM
	for (int i = 0; i < MACBUFFER_SIZE; i++)
		MAC[i] = EEPROM.read(i + EEPROM_MAC_INDEX);

	//Write default IP Address into EEPROM
	for (int i = 0; i < IPBUFFER_SIZE; i++)
		IP[i] = EEPROM.read(i + EEPROM_IP_INDEX);

	//Write default GATEWAY  into EEPROM
	for (int i = 0; i < GATEWAYBUFFER_SIZE; i++)
		GATEWAY[i] = EEPROM.read(i + EEPROM_GATEWAY_INDEX);

	//Write default SUBNET  into EEPROM
	for (int i = 0; i < SUBNETBUFFER_SIZE; i++)
		SUBNET[i] = EEPROM.read(i + EEPROM_SUBNET_INDEX);
}

void processData()
{
	byte *data;
	byte command = 0;
	byte length = 0;
	command = com.getCommand();
	length = com.getData(&data);


	
	switch (command)
	{
	case CMD_SET_IP:
		//get the IP Address from buffer
		for (int i = 0; i < IPBUFFER_SIZE; i++)
			IP[i] = data[i];
		updateEEPROM(EEPROM_IP_INDEX, IP, IPBUFFER_SIZE);
		com.configure(IP, MAC, GATEWAY, SUBNET, BAUD_RATE, 10);

		break;

	case CMD_SET_MAC:
		//get the mac ID from buffer
		for (int i = 0; i < MACBUFFER_SIZE; i++)
			MAC[i] = data[i];
		updateEEPROM(EEPROM_MAC_INDEX, MAC, MACBUFFER_SIZE);
		com.configure(IP, MAC, GATEWAY, SUBNET, BAUD_RATE, 10);

		break;

	case CMD_SET_GATEWAY:
		//get the mac ID from buffer
		for (int i = 0; i < GATEWAYBUFFER_SIZE; i++)
			GATEWAY[i] = data[i];
		updateEEPROM(EEPROM_GATEWAY_INDEX, GATEWAY, GATEWAYBUFFER_SIZE);
		com.configure(IP, MAC, GATEWAY, SUBNET, BAUD_RATE, 10);

		break;

	case CMD_SET_SUBNET:
		//get the mac ID from buffer
		for (int i = 0; i < SUBNETBUFFER_SIZE; i++)
			SUBNET[i] = data[i];
		updateEEPROM(EEPROM_SUBNET_INDEX, SUBNET, SUBNETBUFFER_SIZE);
		com.configure(IP, MAC, GATEWAY, SUBNET, BAUD_RATE, 10);

		break;

	case CMD_GET_MODEL:
		com.sendResponse((byte*)MODEL, strlen(MODEL));
		break;

	case CMD_GET_SERIALNO:
		com.sendResponse((byte*)SERIALNO, strlen(SERIALNO));
		break;
	case CMD_GET_CUSTOMERID:
		com.sendResponse((byte*)CUSTOMERID, strlen(CUSTOMERID));
		break;

	case CMD_SET_SCROLLING_SPEED:
		com.transact((byte)CMD_SET_SCROLLING_SPEED, (byte)data[0], (byte*)&data[1], strlen((char*)&data[1]));
		break;
	}

	//dmd.clearScreen( WHITE );   //true is normal (all pixels off), false is negative (all pixels on)


}

void updateEEPROM(int from, byte *data, int length)
{
	for (int i = 0; i < length; i++)
	{
		EEPROM.write(i + from, data[i]);
	}

}


