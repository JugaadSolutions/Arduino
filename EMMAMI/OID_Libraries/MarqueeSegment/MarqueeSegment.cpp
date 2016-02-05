#include "MarqueeSegment.h"


MarqueeSegment::MarqueeSegment( )
{
	 
	
	active = 0;
	language = 0;
	
	for (int i = 0; i < MAX_MESSAGE_SIZE; i++)
		message[i] = 0;

	
	
}





void MarqueeSegment::readFromEEPROM(int index )
{
	int i;
	int StartIndex = index * (MAX_MESSAGE_SIZE + 2);
	for (i = 0; i < MAX_MESSAGE_SIZE; i++, StartIndex++)
	{
		message[i] = EEPROM.read(StartIndex + i);
	}
	
	language = EEPROM.read(StartIndex++);
	active =  EEPROM.read(StartIndex);
		
	
}
void MarqueeSegment::updateEEPROM(int index)
{
	int i;
	int StartIndex = index * (MAX_MESSAGE_SIZE + 2);

	for (i = 0; i <= strlen(message); i++, StartIndex++)
	{
		EEPROM.write(StartIndex + i, message[i]);
	}
	for (; i < MAX_MESSAGE_SIZE; i++, StartIndex++)
	{
		EEPROM.write(StartIndex+i, 0); 
	}
	

	EEPROM.write(StartIndex++, language);
	
	EEPROM.write(StartIndex, active); 

}

void MarqueeSegment::changeActivation(int index,bool status)
{
	int i;
	this->active = status;

	int activeIndex = index*(MAX_MESSAGE_SIZE + 2) + EEPROM_ACTIVE_INDEX;

	EEPROM.write(activeIndex,status);

}

