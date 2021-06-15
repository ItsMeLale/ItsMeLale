//---------------------------------------------------------------------------
//
// MODULE           :   USRTIMERS.C
// DESCRIPTION      :   
//                              
//
// HISTORY           :
//      AUTHOR       :  Luca AGOSTINI
//      DATE         :  04/09/2004   
//      DESCRIPTION  : 
//
//      AUTHOR       :  
//      DATE         :     
//      DESCRIPTION  : 
//
//      AUTHOR       :  
//      DATE         :     
//      DESCRIPTION  : 
//
//
// DEVELOPMENT TOOLS
//      TYPE         :  METROWERKS CodeWarrior - HC12
//      RELEASE      :  5.5.1272
//      TYPE         :
//      RELEASE      :
//      TYPE         :
//      RELEASE      :
//
//
// Property of Lumineed
//---------------------------------------------------------------------------

/****************************************************************************
 * pragma
 ***************************************************************************/

/****************************************************************************
 * USRTIMERS  extern define exsclusion
 ***************************************************************************/
#define USRTIMERS_GLOBAL

/****************************************************************************
 * include
 ***************************************************************************/
// Library includes
#include "string.h"
// User includes
#include "userdefs.h"
#include "usrtimers.h"

/****************************************************************************
 * definitions
 ***************************************************************************/

/****************************************************************************
 * constants
 ***************************************************************************/


/****************************************************************************
 * variables
 ***************************************************************************/


/****************************************************************************
 * prototypes  
 ***************************************************************************/


/****************************************************************************
 * code
 ***************************************************************************/

//---------------------------------------------------------------------------
//
// FUNCTION
//      void vUsrTimersInit (void) 
//
// DESCRIPTION 
//      Initilise properly the user timers table
// 
// INPUTS           
//                                  
// OUTPUTS      
//
// NOTES
//
//---------------------------------------------------------------------------
void vUsrTimersInit (void)
{
    memset(auiUsrTimersTable, (unsigned char)0x00, sizeof(auiUsrTimersTable));
}


//---------------------------------------------------------------------------
//
// FUNCTION
//      void UsrTimers (void) 
//
// DESCRIPTION 
//      This function decrements each main timer tick, and decrements
//      timer register to perform system delay.
//      Each timer can be associated to a monostable or it can be 
//      set/checked directly
// 
// INPUTS           
//                                            
// OUTPUTS      
//
// NOTES
//      The timing resolution is directly linked to the number of timers 
//      available. To maximise it, two timers are decreased at once    
//      NOTE: due to they larger number, the monostable timers have
//      a lower resolution, and they are updated once in this function.  
//
//---------------------------------------------------------------------------
void vUsrTimers (void)
{
    static INT8U timerIndex1=0;
    static INT8U timerIndex2=0;
        
    
    // USER TIMER UPDATING
    /* Change the time table pointer */
    if (timerIndex1>=USRTIMERNUM)
        timerIndex1 = 0;        

    /* Decrements the first time-table element */
    if(auiUsrTimersTable[timerIndex1]!=0)
        auiUsrTimersTable[timerIndex1]--;
    timerIndex1++;
        
    /* Decrements the second time-table element */
    if(auiUsrTimersTable[timerIndex1]!=0)
        auiUsrTimersTable[timerIndex1]--;
    timerIndex1++;

    // MONOSTABLE TIMER UPDATING
    /* Change the time table pointer */
    if (timerIndex2>=MSTIMERNUM)
        timerIndex2 = 0;        

    /* Decrements the first time-table element */
    if(auiMsTimersTable[timerIndex2]!=0)
        auiMsTimersTable[timerIndex2]--;
    timerIndex2++;


}


