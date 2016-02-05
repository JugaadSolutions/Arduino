/*
*------------------------------------------------------------------------------
* c
*
* Display module supporting 7 segment digit displays.
* Refreshing scheme is used to provide a stationary display.
* Data to be displayed is either a digit(0-9) or space(' ') in ascii
* Space represents clear i.e all segments of the 7 segment display
* turned off.
* Digit select lines are DIGIT_SELECT_A(LSB) , DIGIT_SELECT_B , 
* DIGIT_SELECT_C(MSB). These lines are assumed to be connected to
* a 3 to 8 decoder(74LS138) giving maximum of 8 digit selection.
* The data for each particular digit is put on DISPLAY_PORT, which
* is common to all the digits.
* 
*------------------------------------------------------------------------------
*/


/*
*------------------------------------------------------------------------------
* INCLUDES
*------------------------------------------------------------------------------
*/

#include <avr/pgmspace.h>
#include "digitdisplay.h"
#include "Arduino.h"


/*------------------------------------------------------------------------------
* Structure declarations
*------------------------------------------------------------------------------
*/


/*
*------------------------------------------------------------------------------
* DigitDisplay - the Digit display structure. 
*------------------------------------------------------------------------------
*/




/*
*------------------------------------------------------------------------------
* Private Variables (static) 
*------------------------------------------------------------------------------
*/

const UINT8 SEVENSEGMENT[] ={0x3f,0x06,0x5b,0x4f,0x66,
							  0x6d,0x7d,0x07,0x7f,0x6f,0x00};
/*
*------------------------------------------------------------------------------
* Private Function Prototypes (static)
*------------------------------------------------------------------------------
*/


/*
*------------------------------------------------------------------------------
* Public Functions
*------------------------------------------------------------------------------
*/

DIGITDISPLAY:: DIGITDISPLAY( volatile UINT8* dataPort, volatile UINT8* digitPort, UINT8 numberOfDigits )
{
	noDigits = numberOfDigits;
	this->digitPort = digitPort;
	this->dataPort = dataPort;
	
	digitIndex = 0;
	dispBuffer = buffer[STATIC];
}

DIGITDISPLAY:: DIGITDISPLAY( UINT8 numberOfDigits )
{
	noDigits = numberOfDigits;
	//this->digitPort = digitPort;
	//this->dataPort = dataPort;
	
	digitIndex = 0;
	dispBuffer = buffer[STATIC];
	
		//DIGIT SELECTION
    DDRL = B11111111 ;
    DDRL = DDRL | B11111111;
    PORTL = B00000000;  
	
	//row selection
    DDRK = B11111111 ;
    DDRK = DDRK | B11111111;
    PORTK = B00000000; 

}
/*
*------------------------------------------------------------------------------
*BOOL DigitDisplay_init( UINT8 noDigits )
*
* Function to initialize the digit display module
* 
* Initializes digit index.
* 
*Input : number of digits used in the application
* return value: none.
* 
*------------------------------------------------------------------------------
*/

BOOL DIGITDISPLAY::init( UINT8 noDigits )
{
	UINT8 i,j,k;
	if( noDigits > MAX_DIGITS )	//check limits 
		return FAILURE;

	noDigits = noDigits;				//set no of digits 
	for( i = 0; i < noDigits; i++)
	{
		buffer[BLINK][i] = SEVENSEGMENT[10];		//clear buffer to be used  during blink mode
	}
	dispBuffer = buffer[STATIC];	//set initial display buffer to data(i.e. buffer[0])

#ifdef __DISPLAY_TEST__
	for( i = 0; i < 11 ; i++)
	{
		for(j = 0 ; j < noDigits; j++)
		{
			buffer[STATIC][j] = SEVENSEGMENT[i];	
		}
		for( k = 0; k < 100; k++)
		{
			task();
			DelayMs(3);
		}
	
	}
#endif	//__DISPLAY_TEST__
	digitIndex  = 0;
	
	return SUCCESS;
}


void DIGITDISPLAY::displayTest()
{
	dispBuffer = buffer[STATIC];
	
	//Serial.println(noDigits, HEX);
	
	for( int i = 0; i < 11 ; i++ )
	{
		for( int j = 0 ; j < noDigits; j++ )
		{
			buffer[STATIC][j] = (SEVENSEGMENT[i]);	
			//Serial.println(buffer[STATIC][j], HEX);
		}
		
		for( int k = 0; k < 100; k++ )
		{
			task();
			delay(3);
		}
	
	}
	
	

	
	
/*
	for(int j = 0; j < noDigits ; j++)
	{
		for( int i = 0; i < 11 ; i++)
		{
			writeToDisplayPort( SEVENSEGMENT[i] )	;
			delay(200);
		}
		digitIndex++;
	}
	*/
	
	digitIndex  = 0;	
}
/*
*------------------------------------------------------------------------------
*void DigitDisplay_task( void )
*
* Task to refresh the display. 7 segment code corresponding
* to the data value in the displayBuffer[digitIndex] is 
* output on to the DISPLAY_PORT. If the data value is ' ' then
* all the segments are turned off.
* Input : none
* 
* return value: none.
* 
*------------------------------------------------------------------------------
*/


void DIGITDISPLAY::task(void)
{
	switch(mode)
	{
		case STATIC :
			writeToDisplayPort(dispBuffer[digitIndex]); 
			digitIndex++;	
			//Serial.print(digitIndex, HEX);
			if(digitIndex >= noDigits)
			{
				digitIndex = 0;
			}
		break;

		case BLINK:

			writeToDisplayPort(dispBuffer[digitIndex]);
			digitIndex++;	
			if(digitIndex >= noDigits)
			{
				digitIndex = 0;
			}

			blinkCount++;
			if( blinkCount >= blinkPeriod )
			{
				blinkCount = 0;
				if( dispBuffer == buffer[STATIC] )
					dispBuffer = buffer[BLINK];
				else
					dispBuffer = buffer[STATIC];
			}
		break;

		default:
		break;
	}
				
		
	
}


/*
*------------------------------------------------------------------------------
*BOOL DigitDisplay_updateBuffer(UINT8 *buffer)
*
* Function to reset the digit index
*  
* Input : buffer containing the new values for the digits
* 
* output: the display buffer is updated with the new values
*
* return value: boolean indicating success or failure.
* 
*------------------------------------------------------------------------------
*/

BOOL DIGITDISPLAY::updateBuffer(UINT8 *buffer)
{
	int i = 0;
	for ( i = 0 ; i < noDigits ; i++)
	{

		if ( validate(buffer[i]) == FAILURE )
			return FAILURE;
	}
	for ( i = 0 ; i < noDigits ; i++)
	{
		if( buffer[i] == ' ')
		{
			this->buffer[0][i] = (SEVENSEGMENT[10]);
		}
		else
		{
			this->buffer[0][i] = (SEVENSEGMENT[buffer[i] - '0']);
		}
	}
	digitIndex = 0;
	return SUCCESS;
}


/*
*------------------------------------------------------------------------------
*void DigitDisplay_updateBufferBinary(UINT8 *buffer)
*
* Function to update display buffer with binary data
*  
* Input : buffer containing the new values for the display
* 
* output: none
*
* return value: none
* 
*------------------------------------------------------------------------------
*/

BOOL DIGITDISPLAY::updateBufferBinary(UINT8 *buffer)
{
	UINT8 i = 0;

	for ( i = 0 ; i < noDigits ; i++)
	{
		this->buffer[STATIC][i] = buffer[i] ;

	}
	digitIndex = 0;
	
}



/*
*------------------------------------------------------------------------------
*BOOL DigitDisplay_updateDigit(UINT8 index , UINT8 value)
*
* Function to update a particular digit
*  
* Input : value - value of the digit
* 		  index - index of the digit to be updated.
*
* output: the display buffer is updated with the new value
*
* return value: boolean indicating success or failure.
* 
*------------------------------------------------------------------------------
*/

BOOL DIGITDISPLAY::updateDigit(UINT8 index , UINT8 value)
{
	UINT8 i = 0;
	if(index > noDigits )	//check if index is within limits
		return FAILURE;

	if ( validate(value) == FAILURE )
		return FAILURE;

	if( value == ' ')
	{
		buffer[STATIC][index] = SEVENSEGMENT[10];
	}
	else
	{
		buffer[STATIC][index] = SEVENSEGMENT[value - '0'];
	}

	return SUCCESS;
}


/*
*------------------------------------------------------------------------------
*void DigitDisplay_blinkOn(UINT16 blinkPeriod, UINT8 value)
*
* Function to switch on blink mode
*  
* Input : blinkPeriod - period of blink in millisecond
* 		  
*
* output: none
*
* return value: none
* 
*------------------------------------------------------------------------------
*/

void DIGITDISPLAY::blinkOn(UINT16 blinkPeriod)
{
	blinkPeriod = blinkPeriod / DISPLAY_REFRESH_PERIOD;	//convert period in milliseconds to period in count
	blinkCount = 0;										//reset counter
	mode = BLINK;											//set blink mode
}	



/*
*------------------------------------------------------------------------------
*void DigitDisplay_blinkOff()
*
* Function to switch off blink mode
*  
* Input : none
* 		  
*
* output: none
*
* return value: none
* 
*------------------------------------------------------------------------------
*/

void DIGITDISPLAY::blinkOff()
{
	dispBuffer = buffer[STATIC]; //set current display buffer to data buffer
	mode = STATIC;							   //set static mode
}	





/*
*------------------------------------------------------------------------------
*void DigitDisplay_clear(void)
*
* Function clear the digits
*  
* Input : none
* output: the display buffer is updated 
* return value: none
* 
*------------------------------------------------------------------------------
*/


void DIGITDISPLAY::clear()
{
	UINT8 i;
	for( i = 0 ; i < noDigits ; i++)
	{
		buffer[STATIC][i] = SEVENSEGMENT[10];
	}
	digitIndex = 0;
}






/*
*------------------------------------------------------------------------------
* Private Functions
*------------------------------------------------------------------------------
*/


/*
*------------------------------------------------------------------------------
*void writeToDisplayPort( UINT8 value )
*
* Function to output the seven segment value on to the 
* DISPLAY_PORT. digitIndex gives the digit to be 
* enabled. 
* Input : value to be output on to the DISPLAY_PORT
* 
* return value: none.
* 
*------------------------------------------------------------------------------
*/
//#define COMMON_CATHODE 
#if defined(COMMON_CATHODE)
void DIGITDISPLAY::writeToDisplayPort( UINT8 value )
{

	//switch off display
	//*digitPort = 0x00;
	PORTL = B11111111;
	
	Serial.println(value, HEX);
	//write the data into the data port
	//*dataPort |= value;
	PORTK = ~value;
	
	switch( digitIndex )
	{
		case 0:
			PORTL = B11111110;
			
		break;
		case 1:
   			PORTL = B11111101;

		break;
		case 2:
   			PORTL = B11111011;

   		break;

		case 3:
   			PORTL = B11110111;
		
   		break;

		case 4:
   			PORTL = B11101111;
		
   		break;

		case 5:
   			PORTL = B11011111;
		
   		break;
		case 6:
   			PORTA = B10111111;
		
   		break;

		case 7:
   			PORTL = B01111111;
        break;

		default:
		break;
	}
}
#else
void DIGITDISPLAY::writeToDisplayPort( UINT8 value )
{

	//switch off display
	//*digitPort = 0x00;
	PORTL = B00000000;
	
	//write the data into the data port
	//*dataPort |= value;
	PORTK = value;
	
	switch( digitIndex )
	{
		case 0:
			PORTL = B00000001;
			
		break;
		case 1:
   			PORTL = B00000010;

		break;
		case 2:
   			PORTL = B00000100;

   		break;

		case 3:
   			PORTL = B00001000;
		
   		break;

		case 4:
   			PORTL = B00010000;
		
   		break;

		case 5:
   			PORTL = B00100000;
		
   		break;
		case 6:
   			PORTA = B01000000;
		
   		break;

		case 7:
   			PORTL = B10000000;
        break;

		default:
		break;
	}	
}
#endif

/*
*------------------------------------------------------------------------------
*void DigitDisplay_DotOn(UINT8 from, UINT8 length)
*
* Function to turn on dot in the display
* 
* 
* Input : form and length 
* 
* return value: none.
* 
*------------------------------------------------------------------------------
*/
void DIGITDISPLAY::DotOn(UINT8 from, UINT8 length)
{
	UINT8 i, data;
	for( i = from; i < length+from; i++ )
	{
		data = buffer[STATIC][i];
		buffer[STATIC][i] = (data | 0X80);
	}	
} 


BOOL DIGITDISPLAY::validate( UINT8 value)
{
	BOOL result = FAILURE;
	if( value == ' ' )
	{
		result = SUCCESS;
	}
	else if( value >= '0' && value <= '9'  )
	{
		result = SUCCESS;
	}
	return result;
}
		
/*
*------------------------------------------------------------------------------
*BOOL DigitDisplay_updateBuffer(UINT8 *buffer)
*
* Function to reset the digit index
*  
* Input : buffer containing the new values for the digits,
*			from - zero based index from where the update has to happen
			length - number of digits that have to be updated
* 
* output: the display buffer is updated with the new values
*
* return value: boolean indicating success or failure.
* 
*------------------------------------------------------------------------------
*/

BOOL DIGITDISPLAY::updateBufferPartial( UINT8 *buffer, UINT8 from, UINT8 length)
{
	UINT8 i = 0,j=0;
	for ( i = 0 ; i < length ; i++)
	{

		if ( validate(buffer[i]) == FAILURE )
			return FAILURE;
	}
	for ( i = from ; i < from+length ; i++,j++)
	{
		if( buffer[i] == ' ')
		{
			this->buffer[STATIC][i] = (SEVENSEGMENT[10]);
		}
		else
		{
			this->buffer[STATIC][i] = (SEVENSEGMENT[buffer[j] - '0']);
		}
	}
	digitIndex = 0;
	return SUCCESS;
}

/*
*------------------------------------------------------------------------------
*void DigitDisplay_blinkOn_ind(UINT16 blinkPeriod, UINT8 index)
*
* Function to switch on blink individual segment mode
*  
* Input : blinkPeriod - period of blink in millisecond
* 		  index - The digit to be blinked
*
* output: none
*
* return value: none
* 
*------------------------------------------------------------------------------
*/

void DIGITDISPLAY::blinkOn_ind(UINT16 blinkPeriod, UINT8 index)
{
	UINT8 i;
	blinkPeriod = blinkPeriod /DISPLAY_REFRESH_PERIOD;	//convert period in milliseconds to period in count
	blinkCount = 0;									//reset counter

	blinkIndex = index;
	
	for( i = 0 ; i < noDigits ; i++)
	{
		if ( i != blinkIndex )
			buffer[BLINK][i] = buffer[STATIC][i];
	}

	buffer[BLINK][blinkIndex] = 0X00;	

		mode = BLINK;										//set blink mode
}


