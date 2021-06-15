//---------------------------------------------------------------------------
//
// MODULE           :   MODBUSMAPS.C
// DESCRIPTION      :   This module manages the external keyboard
//                          
//                              
//
// HISTORY           :
//      AUTHOR       :  Luca AGOSTINI
//      DATE         :  30/06/2004   
//      DESCRIPTION  :  First relase
//
//      AUTHOR       :  Luca AGOSTINI
//      DATE         :  05/06/2016
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
// Property of inTech
//---------------------------------------------------------------------------
/****************************************************************************
 * MODBUS extern define exsclusion
 ***************************************************************************/
#define MODBUSMAPS_GLOBAL   1

/****************************************************************************
 * include
 ***************************************************************************/
#include <string.h>
#include "UserDefs.h"

#include "database.h"
#include "modbusmaps.h"
                
/****************************************************************************
 * prototypes
 ***************************************************************************/
static INT16U uiModBusDateTime(void);
static INT16U uiModBusSetting(void);
static INT16U uiModBusParam(void);
static INT16U uiModBusUsrCmd(void);

/****************************************************************************
 * variables
 ***************************************************************************/
//static unsigned char noChar = '-';
static unsigned char noVal = 0x00;
 

// READ ONLY MAPS
// IDENTIFIERS
const T_MAPPA_MODBUS ID_Map[] = {
		   (unsigned char *)&(stIdent.ID00),
           (unsigned char *)&(stIdent.ID01FWVers[1]),
           (unsigned char *)&(stIdent.ID01FWVers[2]),
           (unsigned char *)&(stIdent.ID01FWVers[3]),
           (unsigned char *)&(stIdent.ID01FWVers[4]),
           (unsigned char *)&(stIdent.ID04ModelName[0]),
           (unsigned char *)&(stIdent.ID04ModelName[1]),
           (unsigned char *)&(stIdent.ID04ModelName[2]),
           (unsigned char *)&(stIdent.ID04ModelName[3]),
           (unsigned char *)&(stIdent.ID04ModelName[4]),
           (unsigned char *)&(stIdent.ID04ModelName[5]),
           (unsigned char *)&(stIdent.ID04ModelName[6]),
           (unsigned char *)&(stIdent.ID04ModelName[7]),
           (unsigned char *)&(stIdent.ID04ModelName[8]),
           (unsigned char *)&(stIdent.ID04ModelName[9]),
           (unsigned char *)&(stIdent.ID04ModelName[10]),
           (unsigned char *)&(stIdent.ID04ModelName[11]),
           (unsigned char *)&(stIdent.ID04ModelName[12]),
           (unsigned char *)&(stIdent.ID04ModelName[13]),
           (unsigned char *)&(stIdent.ID04ModelName[14]),
           (unsigned char *)&(stIdent.ID04ModelName[15]),
           (unsigned char *)&(stIdent.ID12Manufacturer[0]),
           (unsigned char *)&(stIdent.ID12Manufacturer[1]),
           (unsigned char *)&(stIdent.ID12Manufacturer[2]),
           (unsigned char *)&(stIdent.ID12Manufacturer[3]),
           (unsigned char *)&(stIdent.ID12Manufacturer[4]),
           (unsigned char *)&(stIdent.ID12Manufacturer[5]),
           (unsigned char *)&(stIdent.ID12Manufacturer[6]),
           (unsigned char *)&(stIdent.ID12Manufacturer[7]),
           (unsigned char *)&(stIdent.ID12Manufacturer[8]),
           (unsigned char *)&(stIdent.ID12Manufacturer[9]),
           (unsigned char *)&(stIdent.ID12Manufacturer[10]),
           (unsigned char *)&(stIdent.ID12Manufacturer[11]),
           (unsigned char *)&(stIdent.ID12Manufacturer[12]),
           (unsigned char *)&(stIdent.ID12Manufacturer[13]),
           (unsigned char *)&(stIdent.ID12Manufacturer[14]),
           (unsigned char *)&(stIdent.ID12Manufacturer[15]),
           (unsigned char *)&(stIdent.ID20SN[0]),
           (unsigned char *)&(stIdent.ID20SN[1]),
           (unsigned char *)&(stIdent.ID20SN[2]),
           (unsigned char *)&(stIdent.ID20SN[3]),
           (unsigned char *)&(stIdent.ID20SN[4]),
           (unsigned char *)&(stIdent.ID20SN[5]),
           (unsigned char *)&(stIdent.ID20SN[6]),
           (unsigned char *)&(stIdent.ID20SN[7]),
           (unsigned char *)&(stIdent.ID20SN[8]),
           (unsigned char *)&(stIdent.ID20SN[9]),
           (unsigned char *)&(stIdent.ID20SN[10]),
           (unsigned char *)&(stIdent.ID20SN[11]),
           (unsigned char *)&(stIdent.ID20SN[12]),
           (unsigned char *)&(stIdent.ID20SN[13]),
           (unsigned char *)&(stIdent.ID20SN[14]),
           (unsigned char *)&(stIdent.ID20SN[15]),
		   (unsigned char *)NULL_MAPPTR
           };

// STATES
const T_MAPPA_MODBUS states_Map[] = {
           (unsigned char *)&(uSTAT0.W.S00)+1,
           (unsigned char *)&(uSTAT0.W.S00),
           (unsigned char *)&(uSTAT1.W.S01),
           (unsigned char *)&(uSTAT1.W.S01)+1,
           (unsigned char *)NULL_MAPPTR
           };

// FLAGS
const T_MAPPA_MODBUS flags_Map[] = {
           (unsigned char *)&(uFLAG0.W.F00)+1,
           (unsigned char *)&(uFLAG0.W.F00),
           (unsigned char *)&(uFLAG1.W.F01)+1,
           (unsigned char *)&(uFLAG1.W.F01),
           (unsigned char *)&(noVal),
           (unsigned char *)&(noVal),
           (unsigned char *)&(noVal),
           (unsigned char *)&(noVal),
//           (unsigned char *)&(uFLAG2.W.F02),
//           (unsigned char *)&(uFLAG2.W.F02)+1,
//           (unsigned char *)&(uFLAG3.W.F03),
//           (unsigned char *)&(uFLAG3.W.F03)+1,
           (unsigned char *)NULL_MAPPTR
           };

// ALARMS
const T_MAPPA_MODBUS alarms_Map[] = {
           (unsigned char *)&(uALM0.W.A00)+1,
           (unsigned char *)&(uALM0.W.A00),
           (unsigned char *)&(noVal),
           (unsigned char *)&(noVal),
//           (unsigned char *)&(uALM1.W.A01),
//           (unsigned char *)&(uALM1.W.A01)+1,
           (unsigned char *)NULL_MAPPTR
           };


// INTERNAL DATA
const T_MAPPA_MODBUS meas_Map[] = {
           (unsigned char *)&(stMeas.M00IntTemp)+1,
           (unsigned char *)&(stMeas.M00IntTemp),
           (unsigned char *)&(stMeas.M01Out1Val)+1,
           (unsigned char *)&(stMeas.M01Out1Val),
           (unsigned char *)&(stMeas.M02Out2Val)+1,
           (unsigned char *)&(stMeas.M02Out2Val),
           (unsigned char *)&(stMeas.M03Out3Val)+1,
           (unsigned char *)&(stMeas.M03Out3Val),
           (unsigned char *)&(stMeas.M04VINVal)+1,
           (unsigned char *)&(stMeas.M04VINVal),
           (unsigned char *)&(stMeas.M05FanRPM)+1,
           (unsigned char *)&(stMeas.M05FanRPM),
           (unsigned char *)&(stMeas.M06Free)+1,
           (unsigned char *)&(stMeas.M06Free),
           (unsigned char *)&(stMeas.M07Free)+1,
           (unsigned char *)&(stMeas.M07Free),
           (unsigned char *)&(stMeas.M08Free)+1,
           (unsigned char *)&(stMeas.M08Free),
           (unsigned char *)&(stMeas.M09Free)+1,
           (unsigned char *)&(stMeas.M09Free),
           (unsigned char *)&(stMeas.M10Free)+1,
           (unsigned char *)&(stMeas.M10Free),
           (unsigned char *)&(stMeas.M11Free)+1,
           (unsigned char *)&(stMeas.M11Free),
           (unsigned char *)&(stMeas.M12Free)+1,
           (unsigned char *)&(stMeas.M12Free),
           (unsigned char *)&(stMeas.M13Free)+1,
           (unsigned char *)&(stMeas.M13Free),
           (unsigned char *)&(stMeas.M14Free)+1,
           (unsigned char *)&(stMeas.M14Free),
           (unsigned char *)&(stMeas.M15Free)+1,
           (unsigned char *)&(stMeas.M15Free),
           (unsigned char *)NULL_MAPPTR
           };



// READ AND WRITE MAP

// SETTING
const T_MAPPA_MODBUS sett_Map[] = {
           (unsigned char *)&(stSett.SET00Free)+1,
           (unsigned char *)&(stSett.SET00Free),
           (unsigned char *)&(stSett.SET01Free)+1,
           (unsigned char *)&(stSett.SET01Free),
           (unsigned char *)&(stSett.SET02Free)+1,
           (unsigned char *)&(stSett.SET02Free),
           (unsigned char *)&(stSett.SET03Free)+1,
           (unsigned char *)&(stSett.SET03Free),
           (unsigned char *)&(stSett.SET04Free)+1,
           (unsigned char *)&(stSett.SET04Free),
           (unsigned char *)&(stSett.SET05Free)+1,
           (unsigned char *)&(stSett.SET05Free),
           (unsigned char *)&(stSett.SET06Free)+1,
           (unsigned char *)&(stSett.SET06Free),
           (unsigned char *)&(stSett.SET07Free)+1,
           (unsigned char *)&(stSett.SET07Free),
           (unsigned char *)&(stSett.SET08Free)+1,
           (unsigned char *)&(stSett.SET08Free),
           (unsigned char *)&(stSett.SET09Free)+1,
           (unsigned char *)&(stSett.SET09Free),
           (unsigned char *)&(stSett.SET10Free)+1,
           (unsigned char *)&(stSett.SET10Free),
           (unsigned char *)&(stSett.SET11Free)+1,
           (unsigned char *)&(stSett.SET11Free),
           (unsigned char *)&(stSett.SET12Free)+1,
           (unsigned char *)&(stSett.SET12Free),
           (unsigned char *)&(stSett.SET13Free)+1,
           (unsigned char *)&(stSett.SET13Free),
           (unsigned char *)&(stSett.SET14Free)+1,
           (unsigned char *)&(stSett.SET14Free),
           (unsigned char *)&(stSett.SET15SlvNum)+1,
           (unsigned char *)&(stSett.SET15SlvNum),
           (unsigned char *)NULL_MAPPTR
           };


// PARAMETERS
const T_MAPPA_MODBUS param_Map[] = {
		   (unsigned char *)&(stParam.P00Free)+1,
           (unsigned char *)&(stParam.P00Free),
		   (unsigned char *)&(stParam.P01Free)+1,
           (unsigned char *)&(stParam.P01Free),
		   (unsigned char *)&(stParam.P02Free)+1,
           (unsigned char *)&(stParam.P02Free),
		   (unsigned char *)&(stParam.P03Free)+1,
           (unsigned char *)&(stParam.P03Free),
		   (unsigned char *)&(stParam.P04Free)+1,
           (unsigned char *)&(stParam.P04Free),
		   (unsigned char *)&(stParam.P05Free)+1,
           (unsigned char *)&(stParam.P05Free),
		   (unsigned char *)&(stParam.P06Free)+1,
           (unsigned char *)&(stParam.P06Free),
		   (unsigned char *)&(stParam.P07Free)+1,
           (unsigned char *)&(stParam.P07Free),
		   (unsigned char *)&(stParam.P08Free)+1,
           (unsigned char *)&(stParam.P08Free),
		   (unsigned char *)&(stParam.P09Free)+1,
           (unsigned char *)&(stParam.P09Free),
		   (unsigned char *)&(stParam.P10Free)+1,
           (unsigned char *)&(stParam.P10Free),
		   (unsigned char *)&(stParam.P11Free)+1,
           (unsigned char *)&(stParam.P11Free),
		   (unsigned char *)&(stParam.P12Free)+1,
           (unsigned char *)&(stParam.P12Free),
		   (unsigned char *)&(stParam.P13Free)+1,
           (unsigned char *)&(stParam.P13Free),
		   (unsigned char *)&(stParam.P14Free)+1,
           (unsigned char *)&(stParam.P14Free),
		   (unsigned char *)&(stParam.P15Free)+1,
           (unsigned char *)&(stParam.P15Free),
		   (unsigned char *)&(stParam.P16Free)+1,
           (unsigned char *)&(stParam.P16Free),
		   (unsigned char *)&(stParam.P17Free)+1,
           (unsigned char *)&(stParam.P17Free),
		   (unsigned char *)&(stParam.P18Free)+1,
           (unsigned char *)&(stParam.P18Free),
		   (unsigned char *)&(stParam.P19Free)+1,
           (unsigned char *)&(stParam.P19Free),
		   (unsigned char *)&(stParam.P20Free)+1,
           (unsigned char *)&(stParam.P20Free),
		   (unsigned char *)&(stParam.P21Free)+1,
           (unsigned char *)&(stParam.P21Free),
		   (unsigned char *)&(stParam.P22Free)+1,
           (unsigned char *)&(stParam.P22Free),
		   (unsigned char *)&(stParam.P23Free)+1,
           (unsigned char *)&(stParam.P23Free),
		   (unsigned char *)&(stParam.P24Free)+1,
           (unsigned char *)&(stParam.P24Free),
		   (unsigned char *)&(stParam.P25Free)+1,
           (unsigned char *)&(stParam.P25Free),
		   (unsigned char *)&(stParam.P26Free)+1,
           (unsigned char *)&(stParam.P26Free),
		   (unsigned char *)&(stParam.P27Free)+1,
           (unsigned char *)&(stParam.P27Free),
		   (unsigned char *)&(stParam.P28Free)+1,
           (unsigned char *)&(stParam.P28Free),
		   (unsigned char *)&(stParam.P29Free)+1,
           (unsigned char *)&(stParam.P29Free),
		   (unsigned char *)&(stParam.P30Free)+1,
           (unsigned char *)&(stParam.P30Free),
		   (unsigned char *)&(stParam.P31Free)+1,
           (unsigned char *)&(stParam.P31Free),
           (unsigned char *)NULL_MAPPTR
           };



/* COMMANDS */
const T_MAPPA_MODBUS cmd_Map[] = {
		   (unsigned char *)&(stComm.C00CmdCode)+1,
		   (unsigned char *)&(stComm.C00CmdCode),
		   (unsigned char *)&(stComm.C01CmdPar1)+1,
		   (unsigned char *)&(stComm.C01CmdPar1),
		   (unsigned char *)&(stComm.C02CmdPar2)+1,
		   (unsigned char *)&(stComm.C02CmdPar2),
		   (unsigned char *)&(stComm.C03CmdPar3)+1,
		   (unsigned char *)&(stComm.C03CmdPar3),
		   (unsigned char *)&(stComm.C04CmdPar4)+1,
		   (unsigned char *)&(stComm.C04CmdPar4),
		   (unsigned char *)&(stComm.C05CmdPar5)+1,
		   (unsigned char *)&(stComm.C05CmdPar5),
		   (unsigned char *)&(stComm.C06CmdPar6)+1,
		   (unsigned char *)&(stComm.C06CmdPar6),
		   (unsigned char *)&(stComm.C07CmdPar7)+1,
		   (unsigned char *)&(stComm.C07CmdPar7),
		   (unsigned char *)NULL_MAPPTR
           };

     
/* MODBUS MAPS */
const t_DESCR_MAPPA_MODBUS descr_mappe[MAX_MAP_DESCR] =
{
	  {0x0100, 0x011C,  &ID_Map[0]      ,   NULL_WRITE      ,&BRW_True  ,&BRW_True},       // IDENTIFIERS
      {0x0200, 0x0201,  &states_Map[0]  ,   NULL_WRITE      ,&BRW_True  ,&BRW_True},       // STATES
      {0x0300, 0x0303,  &flags_Map[0]   ,   NULL_WRITE      ,&BRW_True  ,&BRW_True},       // FLAGS
      {0x0400, 0x0401,  &alarms_Map[0]  ,   NULL_WRITE      ,&BRW_True  ,&BRW_True},       // ALARMS
      {0x0500, 0x053F,  &meas_Map[0]    ,   NULL_WRITE      ,&BRW_True  ,&BRW_True},       // MEASUREMENTS
      {0x0600, 0x060F,  &sett_Map[0]    ,   &uiModBusSetting,&BRW_True  ,&BRW_True},       // SETTING
      {0x0800, 0x081F,  &param_Map[0]   ,   &uiModBusParam  ,&BRW_True  ,&BRW_True},       // PARAMETERS
      {0x0F00, 0x0F07,  &cmd_Map[0]     ,   &uiModBusUsrCmd ,&BRW_True  ,&BRW_True}       // COMMANDS
};


//---------------------------------------------------------------------------
//
// FUNCTION
//      static INT16U uiModBusSetting(void)
//
// DESCRIPTION  
//      Checks the data integrity and postprocess the information after
//      the MODBUS has written the Setting datas
//
// INPUTS           
//
// OUTPUTS      
//      INT16U -    The result of the CHECK
//      
// NOTES  
//
//---------------------------------------------------------------------------
static INT16U uiModBusSetting(void)
{

	// Check the conditions to validate the new SETTINGS
//    if (stSett.SET02MinVElemCh <= 100)
	if (0)
    {
        //vSettInit();
        return(0);
    }
    else
        return(1);
}

//---------------------------------------------------------------------------
//
// FUNCTION
//      static INT16U uiModBusParam(void)
//
// DESCRIPTION  
//      Checks the data integrity and postprocess the information after
//      the MODBUS has written the Setting datas
//
// INPUTS           
//
// OUTPUTS      
//      INT16U -    The result of the CHECK
//      
// NOTES  
//
//---------------------------------------------------------------------------
static INT16U uiModBusParam(void)
{
    // Check the conditions to validate the new PARAMETERS 
//    if (stParam.P00Marker != MARKER_INIT)
	if (0)
    {
//        vParamInit();
        return(0);
    }
    else
        return(1);
}


//---------------------------------------------------------------------------
//
// FUNCTION
//      INT16U uiModBusUsrCmd(void)
//
// DESCRIPTION  
//      Checks the data integrity and postprocess the information after
//      the MODBUS has written the ID datas
//
// INPUTS           
//
// OUTPUTS      
//      INT16U -    The result of the CHECK
//      
//
// NOTE
//
//---------------------------------------------------------------------------
static INT16U uiModBusUsrCmd(void)
{
#define MODBUS_CMD_SWITCH_OFF   0xFFFF
#define MODBUS_CMD_SET_PWM           1
#define MODBUS_CMD_MASTER_MODE     128      // Set Master mode MODBUS - no internal logic

#define MAX_UVC_DURMIN             120      // MAX UVC minute duration 

	switch (stComm.C00CmdCode)
	{
	}
	return(1);

}
