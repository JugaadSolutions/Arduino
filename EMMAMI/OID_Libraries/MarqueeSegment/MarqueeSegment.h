#ifndef  _MARQUEESEGMENT_H_
#define  _MARQUEESEGMENT_H_

#include "Arduino.h"
#include <EEPROM.h>

#define MAX_MESSAGE_SIZE	100

enum
{
	EEPROM_MSG_INDEX = 		0,
	EEPROM_LANGUAGE_INDEX = EEPROM_MSG_INDEX + MAX_MESSAGE_SIZE,
	EEPROM_ACTIVE_INDEX	 = 	EEPROM_LANGUAGE_INDEX + 1

};

class MarqueeSegment
{
	public: 
	
		MarqueeSegment( );
		
		char message[MAX_MESSAGE_SIZE];	//	
		byte language;				//used to decide language of segment
		bool active;				// used to check the active segments
		
		
		void readFromEEPROM(int );
		void updateEEPROM(int);
		void changeActivation(int index, bool status);

		
		
		
};


#endif