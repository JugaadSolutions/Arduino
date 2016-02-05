/*
*------------------------------------------------------------------------------
* digitdisplay.h
*
* Header for display module supporting 7 segment digit displays.
* Data to be displayed is either a digit(0-9) or space(' ')
* Space represents all clear i.e all segments of the 7 segment display
* turned off.
*------------------------------------------------------------------------------
*/


#ifndef __DIGIT_DISPLAY__
#define __DIGIT_DISPLAY__


/*
*------------------------------------------------------------------------------
* Include Files
*------------------------------------------------------------------------------
*/
#include "config.h"


/*
*------------------------------------------------------------------------------
* DEFINES
*------------------------------------------------------------------------------
*/


#define DISPLAY_REFRESH_PERIOD	2	//period of refresh in milliseconds


/*
*------------------------------------------------------------------------------
* ENUMERATIONS
*------------------------------------------------------------------------------
*/

typedef enum
{
	MAX_DIGITS = 8
}DIGITDISPLAY_CONFIG;



typedef enum
{
	STATIC = 0,
	BLINK 
}DISPLAY_MODE;

#define MAX_DIGITS_ROW (6)



class DIGITDISPLAY
{
	private:
		
	DISPLAY_MODE 	mode;
	UINT8 			buffer[2][MAX_DIGITS];	//buffer containing data to be displayed
	UINT8 			noDigits;			//no of digits used in the application
	UINT8 			digitIndex;	// index of the current digit to be displayed
	UINT16			blinkCount;	//counter to be used in blink mode
	UINT16 			blinkPeriod;	//blink period represented in counts
	UINT8* 			dispBuffer; // pointer to current display buffer
	UINT16 			blinkIndex; //counter used to blink perticular digit
	
	volatile UINT8*	dataPort;
	volatile UINT8*	digitPort;
	
	void writeToDisplayPort( UINT8 );
	UINT8 validate( UINT8 value );
	
	public:

	//pre-condition
	//dataPort and digitPort direction has to be set as output port
	DIGITDISPLAY( volatile UINT8* dataPort, volatile UINT8* digitPort, UINT8 numberOfDigits );
	DIGITDISPLAY( UINT8 numberOfDigits );
	
	BOOL init( UINT8 noDigits );	
	BOOL updateBuffer(UINT8 *buffer);
	BOOL updateBufferBinary(UINT8 *buffer);
	BOOL updateDigit(UINT8 index , UINT8 value);
	void clear(void);	
	
	void blinkOn(UINT16 blinkPeriod);
	void blinkOff(void);
	
	void DotOn(UINT8 from, UINT8 length);
	BOOL updateBufferPartial(UINT8 *buffer, UINT8 from, UINT8 length);
	void blinkOn_ind(UINT16 blinkPeriod, UINT8 index);
	
	void displayTest();
	
	void task(void);
	
};




#endif