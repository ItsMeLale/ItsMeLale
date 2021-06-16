//----------------------------------------------------------------------------------------------------------------//
/*
?   ███████╗████████╗ █████╗ ██████╗ ████████╗███████╗██████╗      ██████╗ ███████╗    ██████╗ ███╗   ███╗██╗  ██╗
?   ██╔════╝╚══██╔══╝██╔══██╗██╔══██╗╚══██╔══╝██╔════╝██╔══██╗    ██╔═══██╗██╔════╝    ██╔══██╗████╗ ████║╚██╗██╔╝
?   ███████╗   ██║   ███████║██████╔╝   ██║   █████╗  ██████╔╝    ██║   ██║█████╗      ██║  ██║██╔████╔██║ ╚███╔╝
?   ╚════██║   ██║   ██╔══██║██╔══██╗   ██║   ██╔══╝  ██╔══██╗    ██║   ██║██╔══╝      ██║  ██║██║╚██╔╝██║ ██╔██╗
?   ███████║   ██║   ██║  ██║██║  ██║   ██║   ███████╗██║  ██║    ╚██████╔╝██║         ██████╔╝██║ ╚═╝ ██║██╔╝ ██╗
?   ╚══════╝   ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝   ╚══════╝╚═╝  ╚═╝     ╚═════╝ ╚═╝         ╚═════╝ ╚═╝     ╚═╝╚═╝  ╚═╝
*/                                    
//----------------------------------------------------------------------------------------------------------------//

/**
 *? =======================================================================================================================
 *?                                                   INCLUDES
 *? =======================================================================================================================
**/
#include <stdint.h>
#include <stdbool.h>


#include "./inc/main.h"
#include "./inc/timers.h"
#include "./use/Utility.h"
#include "./use/fixtures.h"
/**
 *? =======================================================================================================================
 *?                                                   DEFININGS
 *? =======================================================================================================================
**/

/*------------------------------------------------------------------------------------------------*/
//* FOREIGN uses
ErrorStat errorStat;
time_check TimerTime;
cronoOptions cronometro;
/*------------------------------------------------------------------------------------------------*/
//* BUFFERs

uint16_t PacchettoDMX[DMX_size];
uint32_t ContainerDMX[channel][value];
/*------------------------------------------------------------------------------------------------*/
//* UTILITIEs

uint8_t Dmx_INIT = 0;
uint16_t lengthofBUf = 0;
uint16_t static frequenzaIdealePacchetto = 0;

/**
 *? =======================================================================================================================
 *?                                                    GLOBAL Functions
 *? =======================================================================================================================
**/
void int_main(void){
    while (1)
    {
        /* code */
    }
}


/**
 *? =======================================================================================================================
 *?                                                     CONTROLS DMX
 *?=======================================================================================================================
**/

void WaitforControl(void){
    //Controllo per vedere la tempistica del pacchetto 
    if((frequenzaIdealePacchetto>tild_BAUD)||(total_time_garanted>frequenzaIdealePacchetto)){
        errorStat.TimeError = 1;
        }

    else if(frequenzaIdealePacchetto == 0){
        errorStat.TimeError = 1;
    }
    else {
        DMX_Scanner();
    }

}

/**
 * @brief Per poter gestire il pacchetto in modo più dinamico
 *        controllo il flusso per definire la frequenza  
 * @param lenght lunghezza dei dati (sono i canali)
 */

void FreqVar (uint16_t lenght){
    uint8_t data1= (lenght*44);
    uint8_t data2= (lenght*value_MAB);

    frequenzaIdealePacchetto = 140+data1+data2+value_MAB;
    waitforControl();
}



/**
 *? =======================================================================================================================
 *?                                                     DMX ON THE GO
 *?=======================================================================================================================
**/


/**-----------------------------------------------------------------------------------------------
 * *                                         CODA di ESECUZIONE
 * *  1  DMX_Starter
 * *  2  FreqVar
 * *  3  waitforControl
 * *  4  DMX_Scanner
 *---------------------------------------------------------------------------------------------**/ 

//CHIEDIAMO il BUFFER e il TIPO di INVIO/RICEZIONE DMX
// RICEZIONE = 0
// INVIO = 1

void DMX_Starter(uint32_t bufferD[], uint8_t DMXType){
    uint8_t DmxType = DMXType;                    // Costruttore  
    TimerIsOn();                                  // Timer per segnalare il tempo al programma

    //Parte la lettura del DMX
    if(DmxType==0){
         lengthofBUf = strlen(bufferD);               //lunghezza del buffer
                                                      //Inizia a fare la lettura e salvataggio dei pacchetti 
         if (TimerTime.app_tick_25ms){                ///* Se è già nell'interrupt del main allora posso lasciare stare sinceramente  

             CronoIsON();                             // Cronometro per definire (tramite calcolo) l'entità dei pacchetti
             
             for (uint8_t i=0; i<lengthofBUf;i++){   
                  PacchettoDMX[i]= bufferD[i];   
             } 
         }
    
        FreqVar(lengthofBUf);
        CronoIsOFF();
        Dmx_INIT=1;
    }

    //Inizia la scrittura del buffer
    if (DmxType == 1){
        
        
    }
}


/*
? Val                __          __  _  _  _  _  _ _    _  _  _  _ _
?      _____________| |_________| |_||_||_||_||_||||___||_||_||_||||______________
? Tim     92-176     8     44       4
*/

void DMX_Scanner(void){
    uint8_t ChannelData=0;
    uint8_t timeHasGone = 0;
    uint16_t QtaSlotInviabili = 0;


    if (Dmx_INIT){
        Dmx_INIT = 0;
        
        if(errorStat.TimeError){
           //FIXME WARNING DA IMPLEMENTARE      
           
        }
        
        
        if (TimerTime.app_tick_25ms)
        {

            do{
             

               ChannelData++; //Incremento Canale         
            }
            while (ChannelData<=lengthofBUf);
           
            /* code */
        }        
    }    
}