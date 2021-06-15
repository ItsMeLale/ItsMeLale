//---------------------------------------------------------------------------
//
// MODULE           :   USERDEFS.H
// DESCRIPTION      :   All user definitions including data types and 
//                      useful macros
//                              
//
// HISTORY           :
//      AUTHOR       :  Luca AGOSTINI
//      DATE         :  2/10/2004   
//      DESCRIPTION  :  First release
//
//
// Property of Lumineed
//---------------------------------------------------------------------------


/****************************************************************************
 * definitions
 ***************************************************************************/

#define USERDEFS	1
  

/*********************************************************************************************************
*                                              DATA TYPES
*                                         (Compiler Specific)
*********************************************************************************************************/

typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;                    /* Unsigned  8 bit quantity                           */
typedef signed   char  INT8S;                    /* Signed    8 bit quantity                           */
typedef unsigned short INT16U;                   /* Unsigned 16 bit quantity                           */
typedef signed short   INT16S;                   /* Signed   16 bit quantity                           */
typedef unsigned int   INT32U;                   /* Unsigned 32 bit quantity                           */
typedef signed   int   INT32S;                   /* Signed   32 bit quantity                           */
typedef float          FP32;                     /* Single precision floating point                    */
typedef double         FP64;                     /* Double precision floating point                    */

// byte ordering on the M0 core they are organised as little endian
#define LOBYTE(w)       ( *( (INT8U *) (&(w)) + 0) )
#define HIBYTE(w)       ( *( (INT8U *) (&(w)) + 1) )

// byte ordering for the M0 core for a 16 bit variable w
#define LOWORDLOBYTE(w) ( *( (INT8U *) (&(w)) + 0)) 
#define LOWORDHIBYTE(w) ( *( (INT8U *) (&(w)) + 1))
#define HIWORDLOBYTE(w) ( *( (INT8U *) (&(w)) + 2))
#define HIWORDHIBYTE(w) ( *( (INT8U *) (&(w)) + 3))

// word ordering for the HC12 core for a 16 bit variable w
#define LOWORD(w)       ( *( (INT16U *) (&(w)) + 1))
#define HIWORD(w)       ( *( (INT16U *) (&(w)) + 0))
#define LOINT(w)        ( *( (INT16S *) (&(w)) + 1))
#define HIINT(w)        ( *( (INT16S *) (&(w)) + 0))



#define NOP { _asm("nop"); }

// enable the following line to activate debug code option
//#define DEBUG_CODE_EXP    1
// enable the following line to activate simulator option

//#define SIMULATOR_FLAG  0  


/****************************************************************************
 * constants
 ***************************************************************************/
#define FALSE		    0
#define TRUE		    1

// ERROR CODING
#define ERR_OK          0
#define ERR_WRNGMSG     1

/****************************************************************************
 * data types
 ***************************************************************************/


/****************************************************************************
 * variables
 ***************************************************************************/

/****************************************************************************
 * prototypes
 ***************************************************************************/
