#include "communicationManager.h"

static char error[8] = "ERROR 0";
static char Success[] = "SUCCESS";

CommunicationManager::CommunicationManager(int bufferSize)
{
	this->buffer = (byte*)malloc(bufferSize*sizeof(byte));

	this->retryCount = 0;
}


void CommunicationManager::configure(byte *ip, byte *mac, byte* gateway, byte *subnet, int baudRate, int retries)
{

	this->ip = ip;


	this->mac = mac;
	this->gateway = gateway;
	this->subnet = subnet;
	this->baudRate = baudRate;

	this->retries = retries;

	Server = EthernetServer(8888);

	//Ethernet.begin(mac, ip, gateway, subnet);
	Ethernet.begin(mac, ip);

	// start listening for clients
	Server.begin();



	Serial.begin(baudRate);
	Serial1.begin(baudRate);
	Serial2.begin(baudRate);
	Serial3.begin(baudRate);

}

void CommunicationManager::task(bool* processingRequired)
{
	switch (state)
	{
	case IDLE:

		//check flag to send the analog data on serial

		Client = Server.available();
		if (Client.available() > 0)
		{
			memset(buffer, '\0', sizeof(buffer));

			tcpPacketSize = Client.available() + 1;  //get the length of the packet

			for (int i = 0; i < tcpPacketSize; i++)
			{
				buffer[i] = Client.read();

			}

			cmd = buffer[COMMAND_INDEX];

			switch (cmd)
			{
			case CMD_SET_IP:
			case CMD_SET_MAC:
			case CMD_SET_GATEWAY:
			case CMD_SET_SUBNET:
			case CMD_GET_MODEL:
			case CMD_GET_SERIALNO:
			case CMD_GET_CUSTOMERID:
			case CMD_SET_SCROLLING_SPEED:
				*processingRequired = true;
				//sendResponse(&buffer[DATA_INDEX], tcpPacketSize);
				break;

			case CMD_GET_IP:
				sendResponse(ip, 4);
				break;

			case CMD_GET_MAC:
				sendResponse(mac, 6);
				break;

			case CMD_GET_GATEWAY:
				sendResponse(gateway, 4);
				break;

			case CMD_GET_SUBNET:
				sendResponse(subnet, 4);
				break;


			case CMD_SET_MARQUEE_SEGMENT:
			case CMD_DISABLE_MARQUEE_SEGMENT:
				transactionStatus[SERIAL_0] = true;
				transactionStatus[SERIAL_1] = true;
				state = TRANSACTION_START;
				retryCount = 0;

				break;

			case CMD_SET_PARAMETER_VALUE:
			case CMD_DISABLE_PARAMETER:
			case CMD_SET_OFFSET:
			case CMD_SET_SCALEFACTOR:
			case CMD_CONFIGURE_PARAMETER:
				transactionStatus[SERIAL_2] = true;
				transactionStatus[SERIAL_3] = true;
				state = TRANSACTION_START;
				retryCount = 0;

				break;

			default:
				break;
			}

		}
		break;

	case TRANSACTION_START:

		for (int i = 0; i < MAX_SERIAL; i++)
		{
			if (transactionStatus[i] == true)
			{
				//store UART number
				serialID = i;

				//store transaction start time
				startTime = millis();

				//send data on serial port
				sendData();


				//change the state
				state = IN_TRANSACTION;
				return;
			}


		}
		state = IDLE;

		break;

	case IN_TRANSACTION:
	{

		byte responseID = 0xFF;

		responseID = responseReceived();

		if (responseID != 0xFF)
		{

			transactionStatus[responseID] = false;

			state = TRANSACTION_START;

			retryCount = 0;
			sendResponse((byte*)Success, strlen(Success));
			return;
		}
		int currentTime = millis();
		if (currentTime - startTime >= RESPONSE_TIMEOUT)
		{
			if (buffer[COMMAND_INDEX] == CMD_SET_PARAMETER_VALUE)
			{
				state = IDLE;
				return; //no retry for channel value updation
			}
			//check for retry count
			if (retryCount >= retries)
			{
				for (int i = 0; i < MAX_SERIAL; i++)
					transactionStatus[i] = false;

				//send error response
				error[7] = serialID + '0';
				
				sendResponse((byte*)error, strlen(error));

				state = IDLE;
				retryCount = 0;

				return;
			}
			//state = TRANSACTION_START;
			retryCount++;

			startTime = currentTime;
			//resend data on serial port incase of no response
			sendData();


		}

	}

	break;

	default:
		break;
	}



}


bool CommunicationManager::sendResponse(byte*responseBuffer, int length)
{
	
		Client.write(responseBuffer, length);
		return true;
	
	
}

void CommunicationManager::getIP(byte *buffer)
{

}

void CommunicationManager::getMAC(byte *buffer)
{

}

byte CommunicationManager::getCommand()
{
	return this->buffer[COMMAND_INDEX];
}

int CommunicationManager::getData(byte **dataBuffer)
{
	*dataBuffer = &buffer[DATA_INDEX];
	return tcpPacketSize;
}
void CommunicationManager::sendData()
{

	int i = 0;

	switch (serialID)
	{
	case SERIAL_0:

		while (buffer[i] != 0xBB)
		{
			Serial.write(buffer[i]);
			i++;
		}
		Serial.write(buffer[i]);
		break;

	case SERIAL_1:

		while (buffer[i] != 0xBB)
		{
			Serial1.write(buffer[i]);
			i++;
		}
		Serial1.write(buffer[i]);
		break;

	case SERIAL_2:

		while (buffer[i] != 0xBB)
		{
			Serial2.write(buffer[i]);
			i++;
		}
		Serial2.write(buffer[i]);
		break;

	case SERIAL_3:

		while (buffer[i] != 0XBB)
		{
			Serial3.write(buffer[i]);
			i++;
		}
		Serial3.write(buffer[i]);

		

		break;

	default:
		break;
	}
}

byte CommunicationManager::responseReceived()
{
	byte ID = 0xFF;
	byte command = 0xFF;

	switch (serialID)
	{
	case SERIAL_0:

		if (Serial.available() > 0)
		{
			command = Serial.read();
		}

		break;

	case SERIAL_1:

		if (Serial1.available() > 0)
		{
			command = Serial1.read();
		}

		break;

	case SERIAL_2:

		if (Serial2.available() > 0)
		{
			command = Serial2.read();
		}

		break;

	case SERIAL_3:

		if (Serial3.available() > 0)
		{
			command = Serial3.read();
		}

		break;

	default:
		break;
	}


	if (command == buffer[COMMAND_INDEX])
	{
		ID = serialID;
	}

	return ID;
}

void CommunicationManager::transact(byte command, byte id, byte *data, byte length)
{


	int i = 0;
	
	if (state == IDLE)
	{

		switch (command)
		{
		case CMD_SET_PARAMETER_VALUE:
		case CMD_DISABLE_PARAMETER:
		case CMD_SET_OFFSET:
		case CMD_SET_SCALEFACTOR:
		case CMD_CONFIGURE_PARAMETER:
			buffer[i++] = 0xAA;
			buffer[i++] = command;
			buffer[i++] = id;

			for (int j = 0; j < length; j++, i++)
			{
				buffer[i] = data[j];

			}
			buffer[i++] = '\0';
			buffer[i] = 0xBB;

			/*
			for (int k = 0; k < i; k++)
			{
				Serial.write(buffer[k]);
			}
			*/

			transactionStatus[SERIAL_2] = true;
			transactionStatus[SERIAL_3] = true;
			state = TRANSACTION_START;

			break;

		case CMD_SET_SCROLLING_SPEED:
			buffer[i++] = 0xAA;
			buffer[i++] = command;
			buffer[i++] = 0; //dummy id
			
			for (int j = 0; j < length; j++, i++)
			{
				buffer[i] = data[j];
			}
			buffer[i++] = '\0';
			buffer[i] = 0xBB;

			transactionStatus[id] = true;

			state = TRANSACTION_START;

			break;

		}

		//

	}
}
