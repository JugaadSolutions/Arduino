#ifndef		_MMD_H_
#define		_MMD_H_

#include "Arduino.h"
#include "config.h"
#include <TimerOne.h> 


#ifndef TRUE
#define TRUE            1
#endif

#ifndef FALSE
#define FALSE           0
#endif

#define ROWS_PER_SYMBOL		7
#define COLUMNS_PER_SYMBOL	5
#define	COLUMNS_PER_CHARACTER	5
#define ROWS_PER_CHARACTER		8

#define SYM_1_COL		99+32
#define SYM_ALL			100+32
#define SYM_SEC_LEFT	101+32
#define SYM_SEC_RIGHT	102+32
#define SYM_7_COL		103+32
#define SYM_76_COL		104+32
#define SYM_765_COL		105+32
#define SYM_7654_COL	106+32
#define SYM_76543_COL	107+32
#define SYM_765432_COL	108+32
#define SYM_7654321_COL	109+32
#define SYM_ONGOING 	110+32
#define SYM_COMPLETE	111+32
#define SYM_CANCEL	 	112+32
#define SYM_PICKING	 	113+32
#define SYM_STAGING	 	114+32
#define SYM_LOADING	 	115+32




enum
{
	UP_ARROW ,
	P_SMALL_FONT,
	S_SMALL_FONT,
	L_SMALL_FONT,
	COMPLETED_O,
	CANCELLED_X,
	X_SMALL_FONT
};

typedef enum 
{
	SCROLL_SPEED_NONE = 0,
	SCROLL_SPEED_LOW = 3,
    SCROLL_SPEED_MEDIUM = 2,
	SCROLL_SPEED_HIGH = 1,
	MAX_SPEED = 4
}SCROLL_SPEED;

typedef struct _MMD_CONFIG
{
	UINT8 startAddress;
	UINT8 len;
	UINT8 *symbolBuffer;
	UINT8 symbolCount;
	SCROLL_SPEED scrollSpeed;
}MMD_Config;


#ifdef OLD
typedef struct _MMD_SEGMENT
{
	UINT8 id;
	UINT8 startAddress;
	UINT8 length;
	UINT8 *symbolBuffer;
	UINT8 symbolCount;
	SCROLL_SPEED scrollSpeed;
	UINT8 speedCount;
	UINT8 curSymbolIndex;
	UINT8 switchBuffer;
	UINT8 shiftCount;
	UINT8 curShiftBuffer;
	UINT8 curDispBuffer;
	UINT8* DispBuffer[2];
	UINT8* DispDataBuffer;


}MMD_Segment;


void MMD_init( UINT8* mmdBuffer );
BOOL MMD_configSegment( UINT8 id,  MMD_Config *config);
extern void MMD_setScrollSpeed( UINT8 segmentID , SCROLL_SPEED speed );
extern void MMD_setSegment( UINT8 segmentID , UINT8 *data);
extern void MMD_clearSegment( UINT8 segmentID);
extern void MMD_updateSegmentFromTo( UINT8 segmentID , UINT8 fromSymbolIndex , UINT8 toSymbolIndex, UINT8 *data );
extern void MMD_refreshDisplay( void );
extern void MMD_task( void );
void WriteDataToDisplay( UINT8 digit, UINT8 data );
void MMD_changeSymbol( UINT8 segmentID , UINT8 symbolID );
#else

typedef struct _MMD_Segment
{
	UINT8 id;
	UINT8 startAddress;
	UINT8 length;
	UINT8 *symbolBuffer;
	UINT8 symbolCount;
	SCROLL_SPEED scrollSpeed;
	UINT8 speedCount;
	UINT8 curSymbolIndex;
	UINT8 switchBuffer;
	UINT8 shiftCount;
	UINT8 curShiftBuffer;
	UINT8 curDispBuffer;
	UINT8 DispBuffer[2][ROWS_PER_SYMBOL][MMD_MAX_CHARS];
	UINT8 DispDataBuffer[ROWS_PER_SYMBOL][MMD_MAX_CHARS];


}MMD_Segment;	

extern void MMD_init(void);
extern BOOL MMD_configSegment( UINT8 id,  MMD_Config *config );
extern void MMD_setScrollSpeed( UINT8 segmentID , SCROLL_SPEED speed );
extern void MMD_setSegment( UINT8 segmentID , UINT8 *data);
extern void MMD_clearSegment( UINT8 segmentID);
extern void MMD_updateSegmentFromTo( UINT8 segmentID , UINT8 fromSymbolIndex , UINT8 toSymbolIndex, UINT8 *data);
extern void MMD_refreshDisplay(void);
extern void MMD_task(void);
void WriteDataToDisplay(UINT8 digit, UINT8 data);
void MMD_changeSymbol( UINT8 segmentID , UINT8 symbolID);
#endif


#endif

