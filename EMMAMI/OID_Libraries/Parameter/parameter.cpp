#include "parameter.h"


Parameter::Parameter( )
{
	 
	value = 0;
	scaleFactor = 0;
	active = 0;
	count = 0;
	offset = 0;
	precision = 1;

	for( int i = 0; i < MAX_TAG_SIZE; i++ )
		tag[i] = ' ';
	for( int i = 0; i < MAX_UNIT_SIZE; i++ )
		unit[i] = ' ';
	scaleFactor_str[0] = 0;
	offset_str[0] = 0;

	
}

void Parameter::updateValue()
{
	
	if( active == true )
	{
	  // map it to the range of the analog input:
	  mapCountToValue();		
	}
	
	
}

void Parameter::mapCountToValue()
{
	/*
	if (count < 206)
	{
		value = 0;
		return;
	}
	*/

	//Serial.println(offset);
	//Serial.println(scaleFactor);
	//Serial.println(initValue);
	value = (count) * scaleFactor + offset;
	//Serial.println(value);
	
}

void Parameter::readFromEEPROM(int index )
{
	int i;
	int StartIndex = index * (MAX_TAG_SIZE + MAX_UNIT_SIZE +(3 * MAX_LIMIT_SIZE) + 1);
	for( i = 0; i < MAX_TAG_SIZE  ; i++,StartIndex++)
	{
		tag[i] = EEPROM.read(StartIndex + i);
	}
	
	for( i =0;  i < MAX_UNIT_SIZE ; i++,StartIndex++)
	{
		unit[i] = EEPROM.read(StartIndex+i);
	}
	for( i = 0; i < MAX_LIMIT_SIZE; i++,StartIndex++)
	{
		scaleFactor_str[i] = EEPROM.read(StartIndex + i);
	}
	
	for( i =0;  i < MAX_LIMIT_SIZE; i++,StartIndex++)
	{
		offset_str[i] = EEPROM.read(StartIndex + i);
	}
	
	for (i = 0; i < MAX_LIMIT_SIZE; i++, StartIndex++)
	{
		precision_str[i] = EEPROM.read(StartIndex + i);
	}
	


	scaleFactor = atof(scaleFactor_str);
	offset = atof(offset_str);
	precision = atoi(precision_str);

	active =  EEPROM.read(StartIndex);
		
	
}
void Parameter::updateEEPROM(int index )
{
	int i;
	int StartIndex = index * (MAX_TAG_SIZE + MAX_UNIT_SIZE +(3 * MAX_LIMIT_SIZE) + 1);
	for( i = 0; i < MAX_TAG_SIZE  ; i++,StartIndex++)
	{
		EEPROM.write(StartIndex+i, tag[i]); 
	}
	
	for( i =0;  i < MAX_UNIT_SIZE ; i++,StartIndex++)
	{
		EEPROM.write(StartIndex+i, unit[i]); 
	}
	for( i = 0; i < MAX_LIMIT_SIZE; i++,StartIndex++)
	{
		EEPROM.write(StartIndex + i, scaleFactor_str[i]);
	}
	
	for( i =0;  i < MAX_LIMIT_SIZE; i++,StartIndex++)
	{
		EEPROM.write(StartIndex + i, offset_str[i]);
	}
	for (i = 0; i < MAX_LIMIT_SIZE; i++, StartIndex++)
	{
		EEPROM.write(StartIndex + i, precision_str[i]);
	}


	EEPROM.write(StartIndex, active); 

}
void Parameter::toString( char*  temp )
{
	char valueStr[10];
	strcat(temp,tag);
	strcat(temp,": ");
	strcat(temp, dtostrf(value,-2, precision, valueStr));
	strcat(temp,unit);
	strcat(temp," ");
	
}