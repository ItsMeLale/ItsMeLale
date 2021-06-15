//---------------------------------------------------------------------------
//
// MODULE           :   MEASCOMP.C
// DESCRIPTION      :   This module deals with measurement computation
//                      reading them by the sensors   
//                              
//
// HISTORY           :
//
//      AUTHOR       : Luca AGOSTINI 
//      DATE         : 5/01/2015    
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
 * MEASCOMP extern define exsclusion
 ***************************************************************************/
#define MEASCOMP_GLOBAL

/****************************************************************************
 * include
 ***************************************************************************/
// HAL Driver includes
#include "main.h"
#include "stm32l0xx_hal_adc.h"
// User includes
#include "userdefs.h"
#include "database.h"
#include "meascomp.h"

/****************************************************************************
 * definitions
 ***************************************************************************/
#ifdef DEBUG_CODE_EXP
#define MEAS_DEBUG
#endif

#define AD_MAXVAL          4096  // AD Val
#define TEMPTP               21  // Temperature Table Points 
   
/****************************************************************************
 * constants
 ***************************************************************************/
//---------------------------------------------------------------------------
// Temperature table
//                                    0�C,   5�C,  10�C,  15�C,  20�C,  25�C,  30�C,  35�C,  40�C,  45�C,   
//                                   50�C,  55�C,  60�C,  65�C,  70�C,  75�C,  80�C,  85�C,  90�C,  95�C,   
//                                  100�C
const INT16U ntc_adtable[TEMPTP] = { 1070,  1255,  1450,  1649,  1850,  2048,  2241,  2426,  2598,  2758,   
                                     2903,  3035,  3154,  3260,  3354,  3437,  3511,  3575,  3632,  3683, 
                                     3727};
const INT16U     ntc_temp[TEMPTP] = {0, 50, 100, 150, 200, 250, 300, 350, 400, 450, 500, 550, 600, 650, 700, 750, 800, 850, 900, 950, 1000};


/****************************************************************************
 * variables
 ***************************************************************************/
INT16U uiADVal[5];		// AD Istantaneous values

/****************************************************************************
 * internal prototypes
 ***************************************************************************/

/****************************************************************************
 * code
 ***************************************************************************/
//---------------------------------------------------------------------------
//
// FUNCTION
//      void vMeasCompute (void)
//
// DESCRIPTION
//      It reads the AD samples and calculate the filtered value for each
//      measurements
//      It controls the Meas ID has been properly changed
//
// INPUTS
//
//
// OUTPUTS
//      - stMeas.M01
//      - stMeas.M02
//      - stMeas.M03
//      - stMeas.M04
//
// NOTES
//
//---------------------------------------------------------------------------
#ifdef NODEF
void HAL_ADC_ConvCpltCallback(void)
{
}
#endif

//---------------------------------------------------------------------------
//
// FUNCTION
//      void vMeasADPoll (void)
//
// DESCRIPTION  
//      It reads the AD samples and calculate the filtered value for each 
//      measurements   
//      It controls the Meas ID has been properly changed     
//                                                                                                   
// INPUTS                                                                                             
//      
//
// OUTPUTS      
//
// NOTES  
//
//---------------------------------------------------------------------------
void vMeasADPoll(ADC_HandleTypeDef *hadc_p)
{
	// Private variables
	static uint8_t ADState;
	static uint8_t ch;
	uint16_t uiTmp;
	static uint16_t uiFiltCh[5];  // filtered channels

	switch (ADState)
    {
        case 0:
        	if ((uFLAG0.B.f01ADCStart == 1) || (ch !=0)) {
            	HAL_ADC_Start(hadc_p);
    	        ADState++;
        	}
            break;
        case 1:
            HAL_ADC_PollForConversion(hadc_p, 10);
	        ADState++;
            break;
        case 2:
            uiTmp = HAL_ADC_GetValue(hadc_p);
            uiFiltCh[ch] = (uiFiltCh[ch] - (uiFiltCh[ch]>>3)) + uiTmp;
            uiADVal[ch] = uiFiltCh[ch]>>3;
	        ADState++;
            break;                
        case 3:
            HAL_ADC_Stop(hadc_p);
	        ADState++;
            break;
        case 4:
	        ADState=0;
	        if (ch++>=4) {
	        	ch = 0;
	        	uFLAG0.B.f02ADCEos = 1;
	        }
	        // select the current channel on ADC
	        switch (ch) {
            case 0: default:
    	        hadc_p->Instance->CHSELR = 0x0001;
            	break;
            case 1:
    	        hadc_p->Instance->CHSELR = 0x0002;
            	break;
            case 2:
    	        hadc_p->Instance->CHSELR = 0x0004;
            	break;
            case 3:
    	        hadc_p->Instance->CHSELR = 0x0008;
            	break;
            case 4:
    	        hadc_p->Instance->CHSELR = 0x0010;
            	break;
            }
            break;    
    }
}

//---------------------------------------------------------------------------
//
// FUNCTION
//      void vMeasCompute (void)
//
// DESCRIPTION  
//      It reads the AD samples and calculate the filtered value for each 
//      measurements   
//      It controls the Meas ID has been properly changed     
//                                                                                                   
// INPUTS                                                                                             
//      
//
// OUTPUTS      
//      - stMeas.M01
//      - stMeas.M02
//      - stMeas.M03
//      - stMeas.M04
//
// NOTES  
//
//---------------------------------------------------------------------------
void vMeasCompute(void)                        
{                                           
#define RDIM_TP		80600		// dimming resistor top side
#define RDIM_BT		12000		// dimming resistor low side
#define R26			33000		// 12V sensing resistor top side
#define R27			10000		// 12V sensing resistor low side
#define AD_EOS		330			// AD EOS [V/10]
#define AD_MAX		4096	    // AD MAX read value

	static INT16U uiM00IntTemp;
	INT16U uiM01ADTemp, uiTmp;
	INT8U i, f;
    INT32U ulTmp;

	// Temperature (0 - 100°C) filtered value
	// for some temperature values (step of 5�C) the ADC corresponding values
	// are listed in the table above; for the other point the values are obtained
	// by interpolation of the two closer values
	// Save AD Value
	uiM01ADTemp = uiADVal[0];
	// Look for the closer table value
	for (i=0, f=0; i<21; i++ )
	{
		if (uiM01ADTemp >= ntc_adtable[i])
				f = i;
		else
			break;
	}

	if (f==0)
		uiTmp = 0;
	else if (f>= (TEMPTP-1))
		uiTmp = ntc_temp[(TEMPTP-1)];
	else {
		ulTmp = ((INT32U)(ntc_temp[(f+1)] - ntc_temp[f])*(uiM01ADTemp - ntc_adtable[f]));
		uiTmp = (INT16U)(ulTmp / (ntc_adtable[(f+1)] - ntc_adtable[f]));
		uiTmp += ntc_temp[f];
	}
	uiM00IntTemp = uiM00IntTemp - (uiM00IntTemp>>4) + uiTmp;
	stMeas.M00IntTemp =  (INT16U)(uiM00IntTemp>>4);

	// Dimming values [V/100]
	uiTmp = (uint16_t)(((uint32_t)uiADVal[1] * AD_EOS)/AD_MAX);
	ulTmp = uiTmp * (RDIM_TP+RDIM_BT);
	uiTmp = (uint16_t)(ulTmp / RDIM_BT);
	stMeas.M01Out1Val = uiTmp;
	uiTmp = (uint16_t)(((uint32_t)uiADVal[2] * AD_EOS)/AD_MAX);
	ulTmp = uiTmp * (RDIM_TP+RDIM_BT);
	uiTmp = (uint16_t)(ulTmp / RDIM_BT);
	stMeas.M02Out2Val = uiTmp;
	uiTmp = (uint16_t)(((uint32_t)uiADVal[3] * AD_EOS)/AD_MAX);
	ulTmp = uiTmp * (RDIM_TP+RDIM_BT);
	uiTmp = (uint16_t)(ulTmp / RDIM_BT);
	stMeas.M03Out3Val= uiTmp;

	// 12V measure [V/100]
	uiTmp = (uint16_t)(((uint32_t)uiADVal[4] * AD_EOS)/AD_MAX);
	ulTmp = uiTmp * (R26+R27);
	uiTmp = (uint16_t)(ulTmp/R27);
	stMeas.M04VINVal = uiTmp;
    
}


