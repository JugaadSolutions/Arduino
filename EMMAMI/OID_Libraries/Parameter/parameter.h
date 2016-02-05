#ifndef  _PARAMETER_H_
#define  _PARAMETER_H_

#include "Arduino.h"
#include <EEPROM.h>

#define MAX_TAG_SIZE	32
#define MAX_UNIT_SIZE	10
#define MAX_LIMIT_SIZE	12

class Parameter
{
	public: 
	
		Parameter( );
		
		char tag[MAX_TAG_SIZE];	//	
		char unit[MAX_UNIT_SIZE];
		float offset;
		float scaleFactor;
		int count;
		int precision;
	
		char  scaleFactor_str[MAX_LIMIT_SIZE];
		char  offset_str[MAX_LIMIT_SIZE];
		char  precision_str[MAX_LIMIT_SIZE];
		

		bool active;				// used to check the active channels
		
		void updateValue();
		
	    void toString( char*); //take the tag name ,value and uints , give it as one string.
		void readFromEEPROM(int );
		void updateEEPROM(int);
		
	private:

		float value;
		void mapCountToValue();
		
		
};


#endif