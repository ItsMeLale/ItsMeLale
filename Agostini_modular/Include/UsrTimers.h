//---------------------------------------------------------------------------
//
// MODULE           :   USRTIMERS .H
// DESCRIPTION      :   USRTIMERS  module header file
//                              
//
// HISTORY           :
//      AUTHOR       :  Luca AGOSTINI
//      DATE         :  04/09/2004   
//      DESCRIPTION  :  First release
//
//      AUTHOR       :  Luca AGOSTINI
//      DATE         :  24/01/2010   
//      DESCRIPTION  :  Second release 
//
//
// Property of Lumineed
//---------------------------------------------------------------------------


/****************************************************************************
 * definitions
 ***************************************************************************/
#ifdef USRTIMERS_GLOBAL
    #define USRTIMERS_EXT   
#else 
    #define USRTIMERS_EXT extern
#endif      
                                
#ifndef USERDEFS
#include "userdefs.h"
#endif

#define F16MHZ       1
//#define F25MHZ       1

#define uS                              /* indicates 1 us timing resolution */
#define mS  * (unsigned long)1000 uS    /* indicates 1 ms timing resolution */
#define S   * (unsigned long)1000000 uS /* indicates 1 s  timing resolution */
#define mSL                             /* indicates 1 ms timing resolution */
#define SL  * (unsigned long)1000 mSL   /* indicates 1 s  timing resolution */
#define Hz  *1/1000000                  /* indicates the frequency unit     */

//#define DELAY_SYNC      (int)(((long)0.20  * 65536)/(62.5))  // Delay from the pulse centre to the PWM ON
//#define DELAY_SYNC_250u (int)(((long)0.250 * 65536)/(62.5)) // Delay correction from syncro to ZC
//#define PULSE_DURATION  (int)(((long)5.00  * 65536)/(62.5))  // Delay from the pulse centre to the PWM ON
//#define PWM_ON_INT      (int)(((long)2.50  * 65536)/(62.5))  // PWM ON interval duration
#define MIN_PERIOD      (int)(((long)15.87 * 65536)/(31.24)) // Minimum period corresponding to 60Hz + 5% 
#define MAX_PERIOD      (int)(((long)21.10 * 65536)/(31.24)) // Maximum period corresponding to 50Hz - 5% 
#define HYST_PERIOD     (int)((MAX_PERIOD-MIN_PERIOD)/32)

//#define DELAY_SYNC_ON   0                                   // Delay from syncro to ZC when already ON
//#define PWM_ON_INT_ON   (int)(((long) 2 * 65536)/(62.5))    // PWM ON interval duration

//#define DELAY_ALWAYSON  (int)(((long)-2 * 65536)/(62.5))    // Delay from syncro to ZC always ON
//#define PWM_ALWAYSON    (int)(((long) 25 * 65536)/(62.5))   // PMW ON always ON when in continuos mode

#define TCYCLE          (1000 uS)                   /* the reload period of timer T1:   */
#define INTTIMER_FREQ   2097000                     /* internal timer frequency [MHz]   */
#define FREQ_1MHz       1000000                     /* 1MHz */

// PWM outputs define
#define PWM_PERIOD		1024

#define RESET_SYNC_OFFSET   cDelayCalc = DELAY_CALC_STATE;



// USER TIMERS DEFINITION                                                           
enum
{
USRTIM0=0,      //  1ms   main cycle 
USRTIM1,		//  10ms  main cycle
USRTIM2,		//  100ms main cycle
USRTIM3,		//  1s    main cycle
USRTIM4,		//  10s   main cycle
USRTIM5,		//
USRTIM6,		//  
USRTIM7,		//  
USRTIMERNUM    
};    

// MONOSTABLE TIMERS DEFINITION
enum
{
MSTIM0=0,       //
MSTIM1,         //  
MSTIM2,         //
MSTIM3,         // 
MSTIM4,         //
MSTIM5,         //
MSTIM6,         //
MSTIM7,         //
MSTIM8,         //
MSTIM9,         //  
MSTIM10,        //
MSTIM11,        // 
MSTIM12,        //
MSTIM13,        //
MSTIM14,        //
MSTIM15,        //
MSTIMERNUM    
};    

/****************************************************************************
 * macros
 ***************************************************************************/
/* These macros are used to set and check the delay timers */
#define SET_USRTIMERS(ntimer,timeint)   auiUsrTimersTable[(ntimer)] = (unsigned int) (((2*(INT32U)(timeint))+((TCYCLE+1) * (USRTIMERNUM))/2)/((TCYCLE) * (USRTIMERNUM))) 

#define END_USRTIMERS(ntimer)    (auiUsrTimersTable[(ntimer)] == 0)  

#define DELAY_USRTIMERS(timeint) (unsigned int) (((2*(INT32U)(timeint))+((TCYCLE) * (USRTIMERNUM))/2)/((TCYCLE) * (USRTIMERNUM))) 

#define SET_MSTIMERS(ntimer,timeint)    auiMsTimersTable[(ntimer)] = (unsigned int) ((((INT32U)(timeint))+((TCYCLE) * (MSTIMERNUM))/2)/((TCYCLE) * (MSTIMERNUM))) 

#define END_MSTIMERS(ntimer)     (auiMsTimersTable[(ntimer)] == 0)  

#define DELAY_MSTIMERS(timeint)  (unsigned int) ((((INT32U)(timeint))+((TCYCLE) * (MSTIMERNUM))/2)/((TCYCLE) * (MSTIMERNUM))) 

// NOTES FOR USER:
//
// Limits of maximum duration, to avoid overflow problems,
// must be calculated as below:
//
// USRTIMERS: maxtime [ms] = [TCYLCE * USRTIMNUM * 65536 - (TCYCLE * USRTIMNUM)/2] / 1000 -1  
// i.e. 131.123 ms
//
// MSTIMERS: maxtime [ms] = [TCYLCE * MSTIMERNUM * 65536 - (TCYCLE * MSTIMERNUM)/2] / 1000 -1  
// i.e. 262.141 ms
//

/****************************************************************************
 * constants
 ***************************************************************************/


/****************************************************************************
 * data types
 ***************************************************************************/


/****************************************************************************
 * variables
 ***************************************************************************/
USRTIMERS_EXT INT16U auiUsrTimersTable[USRTIMERNUM];
USRTIMERS_EXT INT16U auiMsTimersTable[MSTIMERNUM];

/****************************************************************************
 * prototypes
 ***************************************************************************/
void vUsrTimersInit (void);
void vUsrTimers (void);

