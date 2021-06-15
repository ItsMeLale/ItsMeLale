//---------------------------------------------------------------------------
//
// MODULE           :   DataBaseApp.c
// DESCRIPTION      :   
//                              
//
// HISTORY           :
//      AUTHOR       :  Luca AGOSTINI
//      DATE         :  02/09/2004   
//      DESCRIPTION  :  First relase
//
//      AUTHOR       :  Luca AGOSTINI 
//      DATE         :  17/01/2010    
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
//---------------------------------------------------------------------------

/****************************************************************************
 * pragma
 ***************************************************************************/

/****************************************************************************
 * DATABASE extern define exsclusion
 ***************************************************************************/
#define DATABASE_GLOBAL

/****************************************************************************
 * include
 ***************************************************************************/
#include <stdio.h>                /* standard I/O .h-file              */
#include <string.h>
#include "userdefs.h"
//#include "usrtimers.h"
#include "database.h"             // all internal data definitions
//#include "serialcom.h"            // serial communication parameters

/****************************************************************************
 * definitions
 ***************************************************************************/

/****************************************************************************
 * constants
 ***************************************************************************/

/****************************************************************************
 * prototypes
 ***************************************************************************/
//void vModBusMapsInit(void);
void vIDInit(void);
void vSettInit(void);
void vParamInit();
void vDateTimeInit(void);
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
//      void vDataBaseInit (void)
//
// DESCRIPTION  
//      Inits data base variables to their default value
//      This function hs to be if the EEPROM read has gived unuseful result. 
//
// INPUTS           
//
// OUTPUTS      
//      
// NOTES  
//
//---------------------------------------------------------------------------
void vDataBaseInit(void)
{
    vIDInit();
    vSettInit();
//    vDateTimeInit();
//    vParamInit();
}   

//---------------------------------------------------------------------------
//
// FUNCTION
//      void vIDInit (void)
//
// DESCRIPTION
//      Inits the identifier to their default value
//
// INPUTS
//

// OUTPUTS
//
// NOTES
//
//---------------------------------------------------------------------------
void vIDInit(void)
{
    // IDENTIFIERS
    stIdent.ID00 =                   ST_ID_NUM;
    memcpy(stIdent.ID01FWVers,       ST_FW_VERS      , 5);
    memcpy(stIdent.ID04ModelName,    ST_MODEL_NAME   , 16);
    memcpy(stIdent.ID12Manufacturer, ST_COMPANY_NAME , 16);
    memcpy(stIdent.ID20SN,           ST_SN           , 16);

}

//---------------------------------------------------------------------------
//
// FUNCTION
//      void vSettInit (void)
//
// DESCRIPTION
//      Inits the settings to their default value
//
// INPUTS
//
// OUTPUTS
//
// NOTES
//
//---------------------------------------------------------------------------
void vSettInit (void)
{
    // MODBUS SLAVE Configuration
    stSett.SET15SlvNum= SLAVE_NUM;
}


//---------------------------------------------------------------------------
//
// FUNCTION
//      void vParamInit (void)
//
// DESCRIPTION
//      Inits the parameters to their default value
//
// INPUTS
//
// OUTPUTS
//
// NOTES
//
//---------------------------------------------------------------------------
void vParamInit (void)
{
    // Default setting

}

