//---------------------------------------------------------------------------
//
// MODULE           :   CTRLLOGIC.C
// DESCRIPTION      :   This module includes logic function and control
//                      alghoritms and state machine for the RD    
//                              
//
// HISTORY           :
//      AUTHOR       :  Luca AGOSTINI
//      DATE         :  02/09/2004   
//      DESCRIPTION  :  First relase
//
//      AUTHOR       :  Luca AGOSTINI
//      DATE         :  17/02/2010     
//      DESCRIPTION  : 
//
//      AUTHOR       :                           
//      DATE         :     
//      DESCRIPTION  : 
//
//
// DEVELOPMENT TOOLS
//      TYPE         :
//      RELEASE      :
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
 * CTRL extern define exclusion
 ***************************************************************************/
#define CTRLLOGIC_GLOBAL

/****************************************************************************
 * include
 ***************************************************************************/
#include <string.h>
#include "main.h"
#include "stm32l0xx_hal_gpio.h"
#include "stm32l0xx_hal_tim.h"

// USER MODULES
#include "userdefs.h"
#include "ctrllogic.h"          // logic alarms and state computing
#include "meascomp.h"           // measurements 
#include "database.h"           // all internal data definitions
#include "usrtimers.h"          // user timers management


/****************************************************************************
 * definitions
 ***************************************************************************/
#ifdef DEBUG_CODE_EXP
#define DEBUG_CTRLLOGIC
#endif

#define RD_TYPE     0
#define RS_TYPE     1

// MANUAL VOLTAGE THREESHOLDS
#define MIN_DIMTH	       100  // Minimum voltage [V/100] - 0.08V on pin
#define MAX_DIMTH	      1200	// Maximum voltage [V/100] - 0.96V on pin
// FAN ACTIVACTION THREESHOLDS
#define PWM_FAN_ON			10	// Valore di PWM della ventola appena superato il limite
#define PWM_FAN_MAX		   100	// Valore di PWM della ventola alla temperatura massima
#define TEMP_FAN_ON		   450	// Valore di temperatura che attiva la ventola al valore di PWM_FAN_ON
#define TEMP_FAN_MAX	   700  // Valore di temperatura che attiva la ventola al valore di PWM_FAN_ON
#define FAN_MAX_SPEED     2500  // Massimo numero di giri
// DEVICE TIPE DEFINITION
#define SINGLE_UVA           1  // Single UVA strip in the center of the DEVICE
//#define SINGLE_UVC           1  // Single UVC strip in the center of the DEVICE


typedef struct
{
    INT8U ucType;
    INT8U ucTimer;
    INT16U uiDelay;
} tdMonoStable;             // Monostable table for logic

typedef struct
{
    INT8U pwm1Perc;
    INT8U pwm2Perc;
    INT8U pwm3Perc;
} tdCtrlStateOut;           // Control state output variable


/****************************************************************************
 * constants
 ***************************************************************************/
// Note: the delay are expressed in ms
const tdMonoStable Mono[MSTIMERNUM] =
{
    {RS_TYPE,    MSTIM0 ,   DELAY_MSTIMERS(  200 mS)  }, // PowerON delay
    {RS_TYPE,    MSTIM1 ,   DELAY_MSTIMERS( 3000 mS)  }, // S07 
    {RS_TYPE,    MSTIM2 ,   DELAY_MSTIMERS(  200 mS)  }, // S19 DEVICE Enabled
    {RS_TYPE,    MSTIM3 ,   DELAY_MSTIMERS( 3000 mS)  }, // F22 Button Set Prog 
    {RS_TYPE,    MSTIM4 ,   DELAY_MSTIMERS(  200 mS)  }, // F24 Button Prog
    {RS_TYPE,    MSTIM5 ,   DELAY_MSTIMERS(  200 mS)  }, // S19 sensor detection
    {RS_TYPE,    MSTIM6 ,   DELAY_MSTIMERS(  200 mS)  }, // F32 Button Suspension
    {RD_TYPE,    MSTIM7 ,   DELAY_MSTIMERS( 2000 mS)  }, // A02 FAN alarm
    {RS_TYPE,    MSTIM8 ,   DELAY_MSTIMERS(  200 mS)  }, // PowerON delay
    {RS_TYPE,    MSTIM9 ,   DELAY_MSTIMERS(  100 mS)  }, // 
    {RS_TYPE,    MSTIM10,   DELAY_MSTIMERS(  100 mS)  }, // 
    {RS_TYPE,    MSTIM11,   DELAY_MSTIMERS(  100 mS)  }, // 
    {RS_TYPE,    MSTIM12,   DELAY_MSTIMERS(  100 mS)  }, // 
    {RS_TYPE,    MSTIM13,   DELAY_MSTIMERS(  100 mS)  }, // 
    {RS_TYPE,    MSTIM14,   DELAY_MSTIMERS(  100 mS)  }, // 
    {RS_TYPE,    MSTIM15,   DELAY_MSTIMERS(  100 mS)  }  // 
};
 
/****************************************************************************
 * variables
 ***************************************************************************/
// Timer pointer to the handler
TIM_HandleTypeDef *htim_p;
TIM_OC_InitTypeDef *sConfigOC_p;
TIM_OC_InitTypeDef sConfigOC = {0};
tdCtrlStateOut sPwm;
/****************************************************************************
 * prototypes
 ***************************************************************************/
void vMSInit(void);
void vCtrlSetOUT1(uint8_t uiPerc); 
void vCtrlSetOUT2(uint8_t uiPerc); 
BOOLEAN bMonoStable (BOOLEAN in, INT8U monoind);
BOOLEAN bFFRsS (BOOLEAN s, BOOLEAN r, BOOLEAN q_s);
BOOLEAN bFFRsR (BOOLEAN s, BOOLEAN r, BOOLEAN q_r);
uint8_t vCtrl_PWM_ChanelUpdate(TIM_HandleTypeDef *htim, uint32_t Pulse, uint32_t Channel);
void vCtrlSaveCyStatus(uint8_t ucReason); 


/****************************************************************************
 * macros
 ***************************************************************************/

/****************************************************************************
 * code
 ***************************************************************************/

//---------------------------------------------------------------------------
//
// FUNCTION
//      void vCtrlInit(void) 
//
// DESCRIPTION  
//      This function initializes all the variables dealing with logic-state machine
//
// INPUTS
//                                  
// OUTPUTS      
//
// NOTES      
//
//---------------------------------------------------------------------------
void vCtrlInit(void)
{
    vMSInit();                  // set the timers for monostables

}

//---------------------------------------------------------------------------
//
// FUNCTION
//      void vCtrlInit_Handler(TIM_HandleTypeDef *ha_p, TIM_OC_InitTypeDef *sC_p)
//
// DESCRIPTION
//      This function passes pwm timer handler
//
// INPUTS
//
// OUTPUTS
//
// NOTES
//
//---------------------------------------------------------------------------

void vCtrlInit_Handler(TIM_HandleTypeDef *ha_p, TIM_OC_InitTypeDef *sC_p)
{
	htim_p = ha_p;
	sConfigOC_p = sC_p;
	sConfigOC_p->OCMode = TIM_OCMODE_PWM1;
	sConfigOC_p->Pulse = 0;
	sConfigOC_p->OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC_p->OCFastMode= TIM_OCFAST_DISABLE;

}

//---------------------------------------------------------------------------
//
// FUNCTION
//      void vCtrlLoigc() 
//
// DESCRIPTION  
//      Updates states, alarms and flags based on local measurements and data.
//      It has to be called in 1ms main loop 1ms 
//
// INPUTS
//      All input measurements, data and signals
//                                  
// OUTPUTS      
//      States, alarms and flag general information.
//
// NOTES      
//
//---------------------------------------------------------------------------
void vCtrlLogic(void)
{
#define sTEMP_MAX       750             // 75.0[°C]
#define sTEMP_MIN        10             //  1.0[°C]
#define sTEMP_HIST       30             //  3.0[°C]
#define sVIN_MAX       3000             // 30.00[V]
#define sVIN_MIN       1000             // 10.00[V] - cambiare con nuovo CS
#define sVIN_HIST       300             //  3.00[V]

    // Internal timers variables
    static uint16_t uiCtrltimer;
    static uint16_t uiFanOldTimer, uiFanPulsDur;

	//----------------------------------------------------------------------------------------------------
    // FLAGs UPDATE
    //----------------------------------------------------------------------------------------------------
    // F00 - POWER ON Delay 
    uFLAG0.B.f00PowOnDelay = bMonoStable(1, MSTIM0);   // Delay after power on before sensor initialization

    if (uFLAG0.B.f00PowOnDelay) {
        // F08 - Temperature OUT of LIMITS
        uFLAG0.B.f08TempOutLim = stMeas.M00IntTemp >= sTEMP_MAX;   				// Temperature over the threeshold
        // F09 - Temperature WITHIN the LIMITS
        uFLAG0.B.f09TempInLim = stMeas.M00IntTemp <(sTEMP_MAX-sTEMP_HIST);  	// Temperature within the threeshold
        // F10 - VIN OUT of LIMITS
        uFLAG0.B.f10VInOutLim = ((stMeas.M04VINVal>= sVIN_MAX)
        						 || (stMeas.M04VINVal< sVIN_MIN));				// VIN over the threeshold
        // F11 - WIN WITHIN the LIMITS
        uFLAG0.B.f11VInInLim= ((stMeas.M04VINVal< (sVIN_MAX-sVIN_HIST))
        						 || (stMeas.M04VINVal> (sVIN_MIN+sVIN_HIST)));	// VIN over the threeshold

  
  
        
        // F22 - BUTTON Set PROG Detection (5s delay)
        uFLAG1.B.f22ButtSetProg = bMonoStable((stMeas.M01Out1Val>MIN_DIMTH), MSTIM3);
        // F23 - BUTTON Set PROG OLD
        // F26 - SET PROG ACTION on BUTTON release
        if (uFLAG1.B.f23ButtSetPOld != uFLAG1.B.f22ButtSetProg) {
            if (uFLAG1.B.f22ButtSetProg==1)
                uFLAG1.B.f26SetProg = 1;
            uFLAG1.B.f23ButtSetPOld=uFLAG1.B.f22ButtSetProg;
        }
        
        // F24 - BUTTON PROG Detection (200ms delay)
        uFLAG1.B.f24ButtProg = bMonoStable((stMeas.M01Out1Val>MIN_DIMTH), MSTIM4);
        // F25 - BUTTON PROG OLD
        // F27 - PROG ACTION on BUTTON release (work only if not pressed Set Prog)
        if (uFLAG1.B.f25ButtProgOld != uFLAG1.B.f24ButtProg) {
            if ((uFLAG1.B.f24ButtProg==0) && !uFLAG1.B.f26SetProg)
                uFLAG1.B.f27Prog = 1;
            uFLAG1.B.f25ButtProgOld=uFLAG1.B.f24ButtProg;
        }

        // F32 - BUTTON SUSP Activation (200ms delay)
        uFLAG2.B.f32ButtSusp = bMonoStable((stMeas.M02Out2Val>MIN_DIMTH), MSTIM6);
        // F33 - BUTTON SUSP OLD
        // F20 - Force SUSPENSION action  on BUTTON release 
        if (uFLAG2.B.f33ButtSuspOld != uFLAG2.B.f32ButtSusp) {
            if (uFLAG2.B.f32ButtSusp==0)
                uFLAG1.B.f20ForceSusp = 1;
            uFLAG2.B.f33ButtSuspOld=uFLAG2.B.f32ButtSusp;
        }
        
        // Check FAN Pulse
        if (uFLAG0.B.f03FanFBIN!=uFLAG2.B.f38FanFBINOld) {
            uiFanPulsDur = uiCtrltimer-uiFanOldTimer;
            uiFanOldTimer=uiCtrltimer;
            uFLAG2.B.f38FanFBINOld=uFLAG0.B.f03FanFBIN;
            // Check the FAN is moving
            if (uiFanPulsDur>1) 
                uFLAG2.B.f39FanFBRecvd=1;     
        }
        
        
        
    }

    //----------------------------------------------------------------------------------------------------
    // STATEs UPDATE
    //----------------------------------------------------------------------------------------------------
    if (uFLAG0.B.f00PowOnDelay) {
    	// S07 - 
    	uSTAT0.B.s07Dev1Enabled = bMonoStable(!(uFLAG1.B.f16SensorIN), MSTIM1);
        // S21 - No sensor detection (3s delay)
        uSTAT1.B.s21Free = uSTAT0.B.s07Dev1Enabled; 
    	// S08 - Wait for sensor in
    	uSTAT0.B.s08WaitPres = 0;
    	// S10 - VIN within the LIMITS
        uSTAT0.B.s10VInOk = uFLAG0.B.f11VInInLim;   							// VIN Within the limits
#ifdef NODEF
        // S12 - 
		if (stMeas.M03Out3Val<MIN_DIMTH) {
			uSTAT0.B.s12DbgSensor = 0;
		}
		else if (stMeas.M03Out3Val>MIN_DIMTH) {
			uSTAT0.B.s12DbgSensor = 1;
		}
#endif
        // S16 - 
        uSTAT1.B.s16Free = bMonoStable((uFLAG1.B.f16SensorIN), MSTIM5);
        // S17 -  
        uSTAT1.B.s17Free = 0;

        uSTAT1.B.s19Free = bMonoStable((uFLAG1.B.f17SafeIN), MSTIM2);
// LAG: debug 
// scommentare l'istruzione sotto, se si vuole ignorare il consenso esterno (linea IN)
//        uSTAT1.B.s19Free = 1;

    }


    //----------------------------------------------------------------------------------------------------
    // ALARMs UPDATE
    //----------------------------------------------------------------------------------------------------
    // If the sensor is not changing the cyclic sequence number, it sets the alarm after 100ms
    if (uFLAG0.B.f00PowOnDelay) {

        // A01 Temperature limits reached
        //----------------------------------------------------------------------------------------------------
        uALM0.B.a01OverTemp= bFFRsS(uFLAG0.B.f08TempOutLim, uFLAG0.B.f09TempInLim, uALM0.B.a01OverTemp);

        // A02 FAM not moving for 1s
        //----------------------------------------------------------------------------------------------------
        if (!uSTAT0.B.s09FanON) 
            uALM0.B.a02FanNotOk = !bMonoStable(1,MSTIM7);
        else
            uALM0.B.a02FanNotOk = !bMonoStable(uFLAG2.B.f39FanFBRecvd,MSTIM7);

        uALM0.B.a02FanNotOk = 0;
        uFLAG2.B.f39FanFBRecvd=0;
        
        // A03 VIN Out of limits
        //----------------------------------------------------------------------------------------------------
        uALM0.B.a03VInNotOK= bFFRsS(uFLAG0.B.f10VInOutLim, uFLAG0.B.f11VInInLim, uALM0.B.a03VInNotOK);

        // A15 Modbus Timeout
        //----------------------------------------------------------------------------------------------------
        if (uSTAT0.B.s15ModBusON != uFLAG0.B.f15ModbusOn) {
        	if (uSTAT0.B.s15ModBusON == 0)
        		uALM0.B.a15ModbusTO = 1;
        	else
        		uALM0.B.a15ModbusTO = 0;
            // Save current Modbus ON Status
            uFLAG0.B.f15ModbusOn = uSTAT0.B.s15ModBusON;
        }


        // A00 General ALARM
        //----------------------------------------------------------------------------------------------------
        uALM0.B.a00GenAlarm = ((uALM0.W.A00 & 0xFFFE) !=0);

    }
    
}


//---------------------------------------------------------------------------
//
// FUNCTION
//      void vCtrlInRead() 
//
// DESCRIPTION  
//      Read all input and set corresponding flag 
//
// INPUTS
//      All analogue or digital input information
//                                  
// OUTPUTS      
//      All DATABASE general information
//
// NOTES      
//
//---------------------------------------------------------------------------
void vCtrlInRead(void)
{
	// FO3 - FAN feedbcak IN
	if (HAL_GPIO_ReadPin(uC_FAN_FB_GPIO_Port, uC_FAN_FB_Pin))
		uFLAG0.B.f03FanFBIN=1;
	else
		uFLAG0.B.f03FanFBIN=0;

	// F16 - Sensor IN
	if (HAL_GPIO_ReadPin(uC_SENSOR_IN_GPIO_Port, uC_SENSOR_IN_Pin))
   		uFLAG1.B.f16SensorIN = 1;
   	else
   		uFLAG1.B.f16SensorIN = 0;

	// F17 - SAFE IN
	if (HAL_GPIO_ReadPin(uC_SAFE_IN_GPIO_Port, uC_SAFE_IN_Pin))
   		uFLAG1.B.f17SafeIN = 1;
   	else
   		uFLAG1.B.f17SafeIN = 0;

	// F18 - CONFIGURATION IN1 Pin
	if (HAL_GPIO_ReadPin(uC_CONF1_GPIO_Port, uC_CONF1_Pin))
   		uFLAG1.B.f18Conf1IN = 1;
   	else
   		uFLAG1.B.f18Conf1IN= 0;

	// F19 - CONFIGURATION IN1 Pin
	if (HAL_GPIO_ReadPin(uC_CONF2_GPIO_Port, uC_CONF2_Pin))
   		uFLAG1.B.f19Conf2IN= 1;
   	else
   		uFLAG1.B.f19Conf2IN= 0;

}

//---------------------------------------------------------------------------
//
// FUNCTION
//      void vCtrlStateM() 
//
// DESCRIPTION  
//      Updates device state machine.
//      It has to be called in 50ms main loop  
//
// INPUTS
//      All input measurements, data and signals
//                                  
// OUTPUTS      
//      PWM Commands and states
//
// NOTES      
//
//---------------------------------------------------------------------------
void vCtrlStateM(void)
{
//#define UVA_FULL_LEVEL      100
//#define UVA_REDC_LEVEL       35
//#define UVC_FULL_LEVEL      100
//#define UVC_ECO_LEVEL         6
//#define UVC_REDC_LEVEL       35

#define UVA_FULL_LEVEL        8
#define UVA_REDC_LEVEL        4
#define UVC_FULL_LEVEL        8
#define UVC_ECO_LEVEL         2
#define UVC_REDC_LEVEL        4

#define TIME_CALL            50 mSL         // Routine calling time
#define TIME_OFF           5000 mSL         // 5S before switching to ECO
#define TIME_NOPRESENCE     240 SL          // 4 minutes
#define TIME_NOPRES_FAST      5 SL          // 5 seconds in case of sensor exclusion 
#define TIME_2ECO            20 SL          // 20 s to ECO activation
#define TIME_ECO      (16*3600) SL          // 16 hours
#define TIME_ECO_WAIT         3 SL          // 3 seconds to enter program mode
#define TIME_2UVA            20 SL          // 20 seconds
#define TIME_2PROG            3 SL          // 3 seconds to enter program mode
#define TIME_PROG_TO         60 SL          // 60 seconds program mode timeout
#define TIME_2UVC            20 SL          // 30 s to UVC strong activation
#define TIME_SUSP          3600 SL          // 1h suspension time
#define TIME_MIN             60 SL          // 1 minute timer
#define TIME_CYCLE_MIN       20             // Minutes per programming cycle    

#define BLINK_PERIOD  (256*TIME_CALL)       // 13s 
#define MAX_PROG_STEPS        6             // MAX Steps programming algorithm

#define CntOFF (uint16_t)((TIME_OFF)/(TIME_CALL))

#define DRIVER_CURRENT_MA       600         // Driver current avg in [ma]
#define OFFSET_CURRENT_MA        40         // Offset current avg in [ma]

#define PRES_SUSPENDED            1         // UVC CYcle Suspended for people presence
#define ALM_SUSPENDED             2         // UVC CYcle Suspended for alarm reason
#define MAN_SUSPENDED             3         // UVC CYcle Suspended for manual request
#define MBUS_SUSPENDED            4         // UVC CYcle Suspended for modbus request
#define CYC_COMPLETED             5         // UVC CYcle has been completed


// DEVICE STATEs AND MODEs
enum {
    DEVICE_OFF=0,     //  0-State OFF
    DEVICE_ECO,       //  1-ECO Mode (UVC low power)
	DEVICE_ECO_WAIT,  //  2-ECO Wait (Eco mode transition state)
    DEVICE_ALARM,     // 11-ALARM state
    DEVICE_PFAIL,     // 12-PROGRAM fail state due to presence detection 
	DEVICE_STOP,	  // 13-STOP state - low power mode
	DEVICE_TEST,      // 14-AUTOTEST mode
    DEVICE_MAX_STATE
};

    static uint8_t ucDevMode=DEVICE_OFF; // Main State variable 
    static uint32_t ulSTimer=CntOFF;    // Main timer variable
    static uint16_t uiNPTimer;          // No presence timer counter
    static uint16_t uiOneMinTimer;      // One minute timer
    static uint8_t ucBlkCnt=0;          // Blink counter
    static uint8_t ucBlkTimer;          // Blink timer variable
    static uint8_t ucBlkSteps;          // Blink steps
    static uint8_t ucUVCSteps;          // Programmed UVC number of steps
    static uint8_t ucPresCnt;		    // Presence counter
    static uint32_t ulPwmIntegral;      // PWM integral value
    
    uint16_t uiPwmFiltered;             // PWM filtered (low pass)
    uint32_t ulTmp;    
    uint16_t uiTmp;       

    // ---------------------------------------------------
    // STATE MACHINE
    // ---------------------------------------------------
    switch (ucDevMode)
    {
        // ------------------------------------------------
        // OFF State
        // ------------------------------------------------
        case DEVICE_OFF: default:
            // 1-reset all PWM
            //---------------------------------------------
            vCtrlSetOUT1(0);
            vCtrlSetOUT2(0);

            // 2-Check blinking
            //---------------------------------------------
            if (uFLAG1.B.f29NormalBlkOn) {
                vCtrlSetOUT1(UVA_REDC_LEVEL);
            }

            // 3-Check the transitions
            //---------------------------------------------
            // Alarm state
            if (uALM0.B.a00GenAlarm) {
                ucDevMode = DEVICE_ALARM;
            } 
            // Time transition
            else if (ulSTimer==0) {
                ucDevMode = DEVICE_ECO;
                ulSTimer = (uint32_t)((TIME_ECO)/(TIME_CALL));
            }
            else {
                ulSTimer--;                
            }

            break;

        // ------------------------------------------------
        // ECO state
        // ------------------------------------------------
        case DEVICE_ECO:
            // 1-Set ECO LEVEL PWM
            //---------------------------------------------
            vCtrlSetOUT1(0);
            vCtrlSetOUT2(UVC_ECO_LEVEL);

            // 2-Check blinking
            //---------------------------------------------

            // 3-Check the transitions
            //---------------------------------------------
            // Suspension
            if (uFLAG1.B.f20ForceSusp || uFLAG2.B.f34MBusFrcSusp) {
                ulSTimer = (uint32_t)((TIME_SUSP)/(TIME_CALL));
                ucDevMode = DEVICE_SUSP;
            }     
            // Alarm state
            else if (uALM0.B.a00GenAlarm) {
                ucDevMode = DEVICE_ALARM;
            } 
            // Presence detection
            else if (uSTAT1.B.s16Free || uSTAT1.B.s17Free || !uSTAT1.B.s19Free) {
                ucDevMode = DEVICE_ECO_WAIT;
                ulSTimer = (uint32_t)((TIME_ECO_WAIT)/(TIME_CALL));
                ucPresCnt = (uint32_t)((TIME_ECO_WAIT-(TIME_ECO_WAIT/2))/(TIME_CALL));
            }
            // MODBUS UVC force command
            else if (uFLAG2.B.f35MBusFrcUVC) {
                ucDevMode = DEVICE_UVC;
            }
            // Program mode activated 
            else if (uFLAG1.B.f26SetProg && !uSTAT0.B.s15ModBusON) {
                ucDevMode = DEVICE_2PROG;
                ulSTimer = (uint32_t)((TIME_2PROG)/(TIME_CALL));
                // Blink synchornization
                ucBlkTimer=0;
            }     
            // Time transition
            else if (ulSTimer==0) {
                ucDevMode = DEVICE_STOP;
            }
            else {
                ulSTimer--;                
            }

            break;    

        // ------------------------------------------------
        // ECO_WAIT state
        // ------------------------------------------------
        case DEVICE_ECO_WAIT:
            // 1-Set ECO LEVEL PWM
            //---------------------------------------------
            vCtrlSetOUT1(0);
            vCtrlSetOUT2(UVC_ECO_LEVEL);

            // 2-Check blinking
            //---------------------------------------------

            // 3-Check the transitions
            //---------------------------------------------
            // Suspension
            if (uFLAG1.B.f20ForceSusp || uFLAG2.B.f34MBusFrcSusp) {
                ulSTimer = (uint32_t)((TIME_SUSP)/(TIME_CALL));
                ucDevMode = DEVICE_SUSP;
                ucPresCnt = 0;
            }
            // Alarm state
            else if (uALM0.B.a00GenAlarm) {
                ucDevMode = DEVICE_ALARM;
                ucPresCnt = 0;
            }
            // Presence detection
            else if (uSTAT1.B.s16Free || uSTAT1.B.s17Free || !uSTAT1.B.s19Free) {
                // Check for delay on presence detection
                if (ucPresCnt>0) {
                	ucPresCnt--;
                }
                else {
                	ucDevMode = DEVICE_UVA;
                }
            }
            // MODBUS UVC force command
            else if (uFLAG2.B.f35MBusFrcUVC) {
                ucDevMode = DEVICE_UVC;
                ucPresCnt = 0;
            }
            // Program mode activated
            else if (uFLAG1.B.f26SetProg && !uSTAT0.B.s15ModBusON) {
                ucDevMode = DEVICE_2PROG;
                ulSTimer = (uint32_t)((TIME_2PROG)/(TIME_CALL));
                // Blink synchornization
                ucBlkTimer=0;
                ucPresCnt = 0;
            }
            // Time transition
            else if (ulSTimer==0) {
                ucDevMode = DEVICE_ECO;
            }
            else {
                ulSTimer--;
            }

            break;


        // ------------------------------------------------
        // ALARM MODE - very slow blink
        // ------------------------------------------------
        case DEVICE_ALARM:
            // 1-Set UVA LEVEL PWM
            //---------------------------------------------
            vCtrlSetOUT1(0);
            vCtrlSetOUT2(0);
            
            // 2-Check blinking
            //---------------------------------------------
            if (uFLAG0.B.f13BlkOld != uFLAG1.B.f28FastBlinkOn) {
                // Increase the counter on level 0
                if (uFLAG0.B.f13BlkOld==0) {
                    if ((++ucBlkCnt)>=20)
                        ucBlkCnt = 1;
                }        
                uFLAG0.B.f13BlkOld = uFLAG1.B.f28FastBlinkOn;
            }

            // blink fast 10 times and then stops for 10
            if (uFLAG1.B.f28FastBlinkOn & (ucBlkCnt<=10)) {
                vCtrlSetOUT1(UVA_REDC_LEVEL);
            }
            
            // 3-Check the transitions
            //---------------------------------------------
            //if (uFLAG1.B.f20ForceSusp) {
            // RG: per uscire dallo stato di allarme si esce solo con la pressione simultanea dei due pulsanti
            if ((uFLAG1.B.f24ButtProg && uFLAG2.B.f32ButtSusp) || uFLAG2.B.f42MBusExtALM) {
            // Reset blink counter for next states
                ucBlkCnt = 0;
                // Go to OFF and reset the alarm
                ucDevMode = DEVICE_OFF;
                uALM0.W.A00 = 0;
                // Reset Modbus Exit Alarm flag
                uFLAG2.B.f42MBusExtALM = 0;
                // Set OFF Timer 
                ulSTimer=CntOFF;
            }

            break;    

        // ------------------------------------------------
        // STOP MODE - low power mode - all off
        // ------------------------------------------------
        case DEVICE_STOP:
            // 1-Set UVA LEVEL PWM
            //---------------------------------------------
            vCtrlSetOUT1(0);
            vCtrlSetOUT2(0);
            
            // 2-Check blinking
            //---------------------------------------------
            if (uFLAG0.B.f13BlkOld != uFLAG1.B.f28FastBlinkOn) {
                // Increase the counter on level 0
                if (uFLAG0.B.f13BlkOld==0) {
                    if ((++ucBlkCnt)>=50)
                        ucBlkCnt = 1;
                }        
                uFLAG0.B.f13BlkOld = uFLAG1.B.f28FastBlinkOn;
            }

            // blink fast 3 times and then stops for 47
            if (uFLAG1.B.f28FastBlinkOn & (ucBlkCnt<=3)) {
                vCtrlSetOUT1(UVA_REDC_LEVEL);
            }
 
            // 3-Check the transitions
            //---------------------------------------------
            // Exit stop mode
            // RG: per uscire dallo stato di STOP si esce solo con la pressione simultanea dei due pulsanti
            if (uFLAG1.B.f24ButtProg || uFLAG2.B.f32ButtSusp) {
                ucDevMode = DEVICE_UVA;
            }

            break;    

        // ------------------------------------------------
        // PROGRAM FAIL MODE - specific state when programming
        // mode has not completed 
        // ------------------------------------------------
        case DEVICE_PFAIL:
            // 1-Set UVA LEVEL PWM
            //---------------------------------------------
            vCtrlSetOUT1(0);
            vCtrlSetOUT2(0);
            
            // 2-Check blinking
            //---------------------------------------------
            if (uFLAG0.B.f13BlkOld != uFLAG1.B.f28FastBlinkOn) {
                // Increase the counter on level 0
                if (uFLAG0.B.f13BlkOld==0) {
// RG: MODIFICARE IL NUMERO 20 PER LA DURATA DEL DEVICEEGGIO IN off PER BLOCCO uvc
                	if ((++ucBlkCnt)>=20)
                        ucBlkCnt = 1;
                }        
                uFLAG0.B.f13BlkOld = uFLAG1.B.f28FastBlinkOn;
            }

            // blink fast 10 times and then stops for 10
            if (uFLAG1.B.f28FastBlinkOn & (ucBlkCnt<=5)) {
                vCtrlSetOUT1(UVA_REDC_LEVEL);
            }

            // 3-Check the transitions
            //---------------------------------------------
            // Push button exit from SUSPENSION state
            // RG: per uscire dallo stato di STOP si esce solo con la pressione simultanea dei due pulsanti
            // if (uFLAG1.B.f20ForceSusp) {
            if (uFLAG1.B.f24ButtProg || uFLAG2.B.f32ButtSusp) {
                // Reset blink counter for next states
                ucBlkCnt = 0;
                // Goto ECO_MODE
                ucDevMode = DEVICE_ECO;
                ulSTimer = (uint32_t)((TIME_ECO)/(TIME_CALL));
            }
            // Alarm state
            else if (uALM0.B.a00GenAlarm) {
                ucDevMode = DEVICE_ALARM;
            }

            break;    

        // ------------------------------------------------
        // AUTOTEST MODE - initial autotest
        // ------------------------------------------------
        case DEVICE_TEST:
            // 1-Set UVA LEVEL PWM
            //---------------------------------------------
            vCtrlSetOUT1(0);
            vCtrlSetOUT2(0);
            
            // 2-Check blinking
            //---------------------------------------------

            // 3-Check the transitions
            //---------------------------------------------

            break;    
            
    }
      

}

//---------------------------------------------------------------------------
//
// FUNCTION
//      void vCtrlSetOUT1(uint8_t uiPerc) 
//
// DESCRIPTION  
//      Set global UVA level to a specific percentage 
//
// INPUTS
//      All database general informations
//                                  
// OUTPUTS      
//      Sensor PWM output
//
// NOTES      
//		Must be called once every 50ms for UVA blinking procedure
//
//---------------------------------------------------------------------------
void vCtrlSetOUT1(uint8_t uiPerc) 
{
#ifdef SINGLE_UVA
    sPwm.pwm2Perc = uiPerc;
#endif

#ifdef SINGLE_UVC
    sPwm.pwm1Perc = uiPerc;
    sPwm.pwm3Perc = uiPerc;
#endif

}

//---------------------------------------------------------------------------
//
// FUNCTION
//      void vCtrlSetOUT1(uint8_t uiPerc) 
//
// DESCRIPTION  
//      Set global UVA level to a specific percentage 
//
// INPUTS
//      All database general informations
//                                  
// OUTPUTS      
//      Sensor PWM output
//
// NOTES      
//		Must be called once every 50ms for UVA blinking procedure
//
//---------------------------------------------------------------------------
void vCtrlSetOUT2(uint8_t uiPerc) 
{
#ifdef SINGLE_UVA
    sPwm.pwm1Perc = uiPerc;
    sPwm.pwm3Perc = uiPerc;
#endif

#ifdef SINGLE_UVC
    sPwm.pwm2Perc = uiPerc;
#endif
}


//---------------------------------------------------------------------------
//
// FUNCTION
//      void vCtrlOutUpdate() 
//
// DESCRIPTION  
//      Updates sensor outputs using also logic flags and alarm to 
//      detect abnormal conditions
//
// INPUTS
//      All database general informations
//                                  
// OUTPUTS      
//      Sensor PWM output
//
// NOTES      
//		Must be called once every 50ms for UVA blinking procedure
//
//---------------------------------------------------------------------------
void vCtrlOutUpdate(void)
{
// Local variables
uint16_t uiTmp;
uint32_t ulTmp;
// Calculated pulse value
uint32_t pwm1_pulse=0;
uint32_t pwm2_pulse=0;
uint32_t pwm3_pulse=0;
uint32_t pwm4_pulse=0;
//static uint8_t ucUVACnt;	// Counter for UVA blinking

#define BLINK_CNTPERIOD	20	// Alarm blinking period (100*50ms = 5s)
#define BLINK_CNTDUR	10	// Alarm blinking duration (10*50ms=500ms)


	if (uFLAG0.B.f00PowOnDelay) {

		// MODBUS PWM update - be aware that the PWM is inverted with the MOSFET
		// -------------------------------------------------------------------------------------
		// When set the MASTER MODE the Modbus completely control the UNIT
        // THe internal logic is no more responsible to activate the Output!
        if (uSTAT1.B.s31ModBusMM) {
			// A command has been received on modbus -> update the PWM
			if (stDataINT.D04ForcePWM1 <=100)
				pwm1_pulse = (PWM_PERIOD+1) - (uint16_t)((uint32_t)(stDataINT.D04ForcePWM1*PWM_PERIOD)/100);
			else
				pwm1_pulse = 0;
			// Calculate PWM2 pulse
			if (stDataINT.D05ForcePWM2<=100)
				pwm2_pulse = (PWM_PERIOD+1) - (uint16_t)((uint32_t)(stDataINT.D05ForcePWM2*PWM_PERIOD)/100);
			else
				pwm2_pulse = 0;
			// Calculate PWM3 pulse
			if (stDataINT.D06ForcePWM3<=100)
				pwm3_pulse = (PWM_PERIOD+1) - (uint16_t)((uint32_t)(stDataINT.D06ForcePWM3*PWM_PERIOD)/100);
			else
				pwm3_pulse = 0;

		}
		// MANUAL PWM update - be aware that the PWM is inverted with the MOSFET
		// -------------------------------------------------------------------------------------
		else {
            // uses pwm pulse as output from state automa
			if (sPwm.pwm1Perc <=100)
				pwm1_pulse = (PWM_PERIOD+1) - (uint16_t)((uint32_t)(sPwm.pwm1Perc*PWM_PERIOD)/100);
			else
				pwm1_pulse = 0;
			// Calculate PWM2 pulse
			if (sPwm.pwm2Perc<=100)
				pwm2_pulse = (PWM_PERIOD+1) - (uint16_t)((uint32_t)(sPwm.pwm2Perc*PWM_PERIOD)/100);
			else
				pwm2_pulse = 0;
			// Calculate PWM3 pulse
			if (sPwm.pwm3Perc<=100)
				pwm3_pulse = (PWM_PERIOD+1) - (uint16_t)((uint32_t)(sPwm.pwm3Perc*PWM_PERIOD)/100);
			else
				pwm3_pulse = 0;
        
        }


		// FAN Control procedure
		// -------------------------------------------------------------------------------------
//#define INVERTED_FAN	1

#ifdef INVERTED_FAN
		// Se temperatura entro i limiti -> FAN OFF
		if (stMeas.M00IntTemp<TEMP_FAN_ON) {
			pwm4_pulse= 0;
		}
		else if (stMeas.M00IntTemp<TEMP_FAN_MAX) {
			ulTmp = (stMeas.M00IntTemp - TEMP_FAN_ON) * PWM_PERIOD;
			uiTmp = (uint16_t)(ulTmp /(TEMP_FAN_MAX-TEMP_FAN_ON));
			pwm4_pulse= uiTmp;
		}
		else {
			pwm4_pulse= PWM_PERIOD+1;
		}
#else
		// Se temperatura entro i limiti -> FAN OFF
		if (stMeas.M00IntTemp<TEMP_FAN_ON) {
			pwm4_pulse= PWM_PERIOD+1;
		}
		else if (stMeas.M00IntTemp<TEMP_FAN_MAX) {
			ulTmp = (stMeas.M00IntTemp - TEMP_FAN_ON) * PWM_PERIOD;
			uiTmp = (uint16_t)(ulTmp /(TEMP_FAN_MAX-TEMP_FAN_ON));
			pwm4_pulse= (PWM_PERIOD-uiTmp);
		}
		else {
			pwm4_pulse= 0;
		}        
#endif

		// PWM related state - update procedure
		// -------------------------------------------------------------------------------------
		// Check the PWM states based on PWM value
		if (pwm1_pulse < PWM_PERIOD)
			uSTAT0.B.s01Free=1;
		else
			uSTAT0.B.s01Free=0;

		if (pwm2_pulse < PWM_PERIOD)
			uSTAT0.B.s02Free=1;
		else
			uSTAT0.B.s02Free=0;

		if (pwm3_pulse < PWM_PERIOD)
			uSTAT0.B.s03Free=1;
		else
			uSTAT0.B.s03Free=0;

		// UVC - central strip
#ifdef SINGLE_UVA       
		uSTAT0.B.s04Free = uSTAT0.B.s01Free || uSTAT0.B.s02Free;
#endif

#ifdef SINGLE_UVC
		uSTAT0.B.s04Free = uSTAT0.B.s02Free;
#endif
		// FAN ON
		if (pwm4_pulse < (PWM_PERIOD+1)) {
			uSTAT0.B.s09FanON=1;
            stMeas.M05FanRPM = (uint16_t)((uint32_t)((PWM_PERIOD-pwm4_pulse)*FAN_MAX_SPEED/PWM_PERIOD));
            if (stMeas.M05FanRPM<600) 
                stMeas.M05FanRPM=600;    
        }
        else
        {
			uSTAT0.B.s09FanON=0;
            stMeas.M05FanRPM = 0;
        }

		// PWM1 - update
		// -------------------------------------------------------------------------------------
		if (uFLAG0.B.f04PWM1Inited) {
			vCtrl_PWM_ChanelUpdate(htim_p, pwm1_pulse, TIM_CHANNEL_1);
		}
		else {
			sConfigOC_p->Pulse = pwm1_pulse;
			HAL_TIM_PWM_ConfigChannel(htim_p, sConfigOC_p, TIM_CHANNEL_1);
			HAL_TIM_PWM_Start(htim_p, TIM_CHANNEL_1);
			uFLAG0.B.f04PWM1Inited=1;
		}

		// PWM2 - update
		if (uFLAG0.B.f05PWM2Inited) {
			vCtrl_PWM_ChanelUpdate(htim_p, pwm2_pulse, TIM_CHANNEL_2);
		}
		else {
			sConfigOC_p->Pulse = pwm2_pulse;
			HAL_TIM_PWM_ConfigChannel(htim_p, sConfigOC_p, TIM_CHANNEL_2);
			HAL_TIM_PWM_Start(htim_p, TIM_CHANNEL_2);
			uFLAG0.B.f05PWM2Inited=1;
		}

		// PWM3 - update
		if (uFLAG0.B.f06PWM3Inited) {
			vCtrl_PWM_ChanelUpdate(htim_p, pwm3_pulse, TIM_CHANNEL_3);
		}
		else {
			sConfigOC_p->Pulse = pwm3_pulse;
			HAL_TIM_PWM_ConfigChannel(htim_p, sConfigOC_p, TIM_CHANNEL_3);
			HAL_TIM_PWM_Start(htim_p, TIM_CHANNEL_3);
			uFLAG0.B.f06PWM3Inited=1;
		}

		// PWM4 - update
		if (uFLAG0.B.f07PWM4Inited) {
			vCtrl_PWM_ChanelUpdate(htim_p, pwm4_pulse, TIM_CHANNEL_4);
		}
		else {
			sConfigOC_p->Pulse = pwm4_pulse;
			HAL_TIM_PWM_ConfigChannel(htim_p, sConfigOC_p, TIM_CHANNEL_4);
			HAL_TIM_PWM_Start(htim_p, TIM_CHANNEL_4);
			uFLAG0.B.f07PWM4Inited=1;
		}

	}
} 



//---------------------------------------------------------------------------
//
// FUNCTION
//      void vMSInit(void) 
//
// DESCRIPTION  
//      This function initialises all the monostable timers 
//      The init values must be:
//      - set to the maximum value for the RS type monostables
//      - set to zero for the RD type monostables
//
// INPUTS
//                                  
// OUTPUTS      
//
// NOTES      
//
//---------------------------------------------------------------------------
void vMSInit(void)
{
    INT8U i;
    
    for (i=0;i<MSTIMERNUM ;i++)
    {
        if (Mono[i].ucType == RS_TYPE)  
            auiMsTimersTable[(Mono[i].ucTimer)] = (Mono[i].uiDelay);
        else
            auiMsTimersTable[(Mono[i].ucTimer)] = 0;
    }
}



//---------------------------------------------------------------------------
//
// FUNCTION
//      bCtrlMonoStable (BOOLEAN in, INT8U monoind) 
//      - Monostable set and check
//
// DESCRIPTION  
//      This function performs the monostable simulation.   
//
// INPUTS
//      BOOLEAN IN: the input signal
//      INT8U monoind: monostable index (there could be a limited number)
//                                  
// OUTPUTS      
//      BOOLEAN: the output of the monostable  
//
// NOTES      
//
//---------------------------------------------------------------------------
BOOLEAN bMonoStable (BOOLEAN in, INT8U monoind)
{
    BOOLEAN out;
    
    if (Mono[monoind].ucType == RS_TYPE)
    {
        // RS TYPE MONOSTABLE
        // The output is set to high after a delay since the input goes high
        if(in)
        {
           if (END_MSTIMERS(Mono[monoind].ucTimer))
              out=1;
           else
              out=0;
        }
        else
        {
           out=0;
           auiMsTimersTable[(Mono[monoind].ucTimer)] = (Mono[monoind].uiDelay);
        }
    }
    else
    {
        // RD TYPE MONOSTABLE
        // The output is reset to zero after a delay since the input goes low
        if (in)
        {
           auiMsTimersTable[(Mono[monoind].ucTimer)] = (Mono[monoind].uiDelay);
           out= 1;
        }
        else
        {
           if (END_MSTIMERS(Mono[monoind].ucTimer))
              out=0;
           else
              out=1;
        }
    }    

return (out);
}


//---------------------------------------------------------------------------
//
// FUNCTION
//      BOOLEAN bCtrlFFRsS (boolean s, boolean r, boolean q_s) 
//      - RS flip flop SET prevailing 
//
// DESCRIPTION  
//      This function performs the flip-flop simulation.   
//      If both SET and RESET signals are present the SET 
//      has the prevalence.
//
// INPUTS
//      boolean S: SET signal
//      boolean R: RESET signal
//      boolean Q_S: memorized value of the flip-flop
//                                  
// OUTPUTS      
//      boolean Q: the output of the flip-flop  
//
// NOTES      
//
//---------------------------------------------------------------------------
BOOLEAN bFFRsS (BOOLEAN s, BOOLEAN r, BOOLEAN q_s)
{
   if (r)
      q_s= 0;
   if (s)
      q_s= 1;

   return (q_s);
}

//---------------------------------------------------------------------------
//
// FUNCTION
//      BOOLEAN bCtrlFFRsR (boolean s, boolean r, boolean q_r) 
//      - RS flip flop RESET prevailing 
//
// DESCRIPTION  
//      This function performs the flip-flop simulation.   
//      If both SET and RESET signals are present the RESET 
//      has the prevalence.
//
// INPUTS
//      boolean S: SET signal
//      boolean R: RESET signal
//      boolean Q_R: memorized value of the flip-flop
//                                  
// OUTPUTS      
//      boolean Q: the output of the flip-flop  
//
// NOTES      
//
//---------------------------------------------------------------------------
BOOLEAN bFFRsR (BOOLEAN s, BOOLEAN r, BOOLEAN q_r)
{
   if (s)
      q_r= 1;
   if (r)
      q_r= 0;

   return (q_r);
}


/**
  * @brief  Initializes the TIM PWM  channels according to the specified
  *         parameters in the TIM_OC_InitTypeDef.
  * @param  htim TIM PWM handle
  * @param  sConfig TIM PWM configuration structure
  * @param  Channel TIM Channels to be configured
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  *            @arg TIM_CHANNEL_3: TIM Channel 3 selected
  *            @arg TIM_CHANNEL_4: TIM Channel 4 selected
  * @retval HAL status
  */
uint8_t vCtrl_PWM_ChanelUpdate(TIM_HandleTypeDef *htim, uint32_t Pulse, uint32_t Channel)
{

	  switch (Channel)
	  {
	    case TIM_CHANNEL_1:
	    {
	      /* Check the parameters */
	      assert_param(IS_TIM_CC1_INSTANCE(htim->Instance));
	      /* Set the Pulse register */
	      htim->Instance->CCR1 = Pulse;
	      break;
	    }

	    case TIM_CHANNEL_2:
	    {
	      /* Check the parameters */
	      assert_param(IS_TIM_CC2_INSTANCE(htim->Instance));
	      /* Set the Pulse register */
	      htim->Instance->CCR2 = Pulse;
	      break;
	    }

	    case TIM_CHANNEL_3:
	    {
	      /* Check the parameters */
	      assert_param(IS_TIM_CC3_INSTANCE(htim->Instance));
	      /* Set the Pulse register */
	      htim->Instance->CCR3 = Pulse;
	      break;
	    }

	    case TIM_CHANNEL_4:
	    {
	      /* Check the parameters */
	      assert_param(IS_TIM_CC4_INSTANCE(htim->Instance));
	      /* Set the Pulse register */
	      htim->Instance->CCR4 = Pulse;
	      break;
	    }

	    default:
	      break;
	  }

	  return HAL_OK;
}
