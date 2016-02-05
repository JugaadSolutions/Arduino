#ifndef CONFIG_H
#define CONFIG_H

/*
*------------------------------------------------------------------------------
* config.h
*------------------------------------------------------------------------------
*/

#ifndef TRUE
#define TRUE            1
#endif

#ifndef FALSE
#define FALSE           0
#endif

#ifndef SUCCESS
#define SUCCESS			1
#endif

#ifndef FAILURE			
#define FAILURE			0
#endif


typedef	unsigned char 		BOOL;		//8 bits 	0 				255
typedef	char	 			INT8;		//8 bits 	-128 			127
//signed char 							//8 bits 	-128 			127
typedef	unsigned char 		UINT8;		//8 bits 	0 				255
typedef	int 				INT16;		//16 bits 	-32,768 		32,767
typedef	unsigned int 		UINT16;		//16 bits 	0 				65,535
//short 								//16 bits 	-32,768 		32,767
//unsigned short 						//16 bits 	0 				65,535
typedef	long 				INT32;		//32 bits 	-2,147,483,648 	2,147,483,647
typedef	unsigned long 		UINT32;		//32 bits 	0 				4,294,967,295
typedef float 				FLOAT;		//32 bits 	-126(Exponent)	128(Exponent)
typedef double 				DOUBLE;		//32 bits 	-126(Exponent) 	128(Exponent)

#define	OLD
//MMD module configuration
#define MMD_MAX_CHARS		32
#define MMD_MAX_SEGMENTS	1
#define MMD_MAX_ADDRESS		128
//#define __MMD_STATIC__


/*
*------------------------------------------------------------------------------
* Public Data Types
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* Public Variables (extern)
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* Public Constants (extern)
*------------------------------------------------------------------------------
*/


/*
*------------------------------------------------------------------------------
* Public Function Prototypes (extern)
*------------------------------------------------------------------------------
*/

#endif
/*
*  End of config.h
*/



