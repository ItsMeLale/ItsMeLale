/*
?       ██████╗ ███████╗████████╗████████╗██╗   ██╗    ██████╗  █████╗ ████████╗ █████╗
?      ██╔════╝ ██╔════╝╚══██╔══╝╚══██╔══╝╚██╗ ██╔╝    ██╔══██╗██╔══██╗╚══██╔══╝██╔══██╗
?      ██║  ███╗█████╗     ██║      ██║    ╚████╔╝     ██║  ██║███████║   ██║   ███████║
?      ██║   ██║██╔══╝     ██║      ██║     ╚██╔╝      ██║  ██║██╔══██║   ██║   ██╔══██║
?      ╚██████╔╝███████╗   ██║      ██║      ██║       ██████╔╝██║  ██║   ██║   ██║  ██║
?       ╚═════╝ ╚══════╝   ╚═╝      ╚═╝      ╚═╝       ╚═════╝ ╚═╝  ╚═╝   ╚═╝   ╚═╝  ╚═╝
*/

/**-----------------------------------------------------------------------------------------------------------------------
 **                                                  -  INFO HEADER  -
 **   
 **     L'idea di questa TASK è di gestire la trasmissione dei dati da UART a un primo livello di controllo DATA
 **     e questo sarà eseguito tramite le funzioni che si susseguiranno in questo piccolo main che 
 **      - si impegna a :
 **         1) Gestire le tempistiche del DMX calcolando la frequenza dei pacchetti che si susseguono 
 **         2) Trovare errori nel pacchetto e gestirli (come il cambio del canale corretto se non viene forzato)
 **         3) Distribuire nel modo corretto invisibilmente i dati alle retrovie tramite funzioni modulari
 **     
 **     Nel main sarà rischiesto di eseguire la funzione - DMX_Starter() - che serve come motore per poter poi
 **     gestire nel modo appropriato le restanti funzioni.
 ** 
 *-----------------------------------------------------------------------------------------------------------------------**/

/**
 *? =======================================================================================================================
 *?                                                     INCLUDES
 *? =======================================================================================================================
**/
#include <stdint.h>
#include <stdbool.h>


#include "./inc/getty.h"
#include "./inc/timers.h"
#include "./use/utility.h"
#include "./use/fixtures.h"

/**
 *? =======================================================================================================================
 *?                                                     DEFININGS
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

/*------------------------------------------------------------------------------------------------*/
//* UTILITIEs

uint8_t Dmx_INIT = 0;
uint16_t static frequenzaIdealePacchetto = 0;


/**
 *? ======================================================================================================================
 *?                                                     CONTROLS DMX
 *?=======================================================================================================================
**/

void WaitforControl(void){
    //Controllo per vedere la tempistica del pacchetto 
    if((frequenzaIdealePacchetto>tild_BAUD)||(tempoScadenzaTimeout>frequenzaIdealePacchetto)){
        errorStat.TimeError = 1;
        }

    else if(frequenzaIdealePacchetto == 0){
        errorStat.TimeError = 1;
    }
    else {
        errorStat.TimeError = 0;
    }

}

/**
 * @brief Per poter gestire il pacchetto in modo più dinamico
 *        controllo il flusso per definire la frequenza  
 * @param lenght lunghezza dei dati (sono i canali)
 */
/*
? Val                __          __  _  _  _  _  _ _    _  _  _  _ _
?      _____________| |_________| |_||_||_||_||_||||___||_||_||_||||______________
? Tim     92-176     8     44       4
*/

void FreqVar (uint16_t lenght){
    uint8_t data1= (lenght*44);
    uint8_t data2= (lenght*value_MAB);

    frequenzaIdealePacchetto = 140+data1+data2+value_MAB;
    waitforControl();
}


/**
 *? ======================================================================================================================
 *?                                                     to get DATA
 *? ======================================================================================================================
 **/


//CHIEDIAMO il BUFFER e il TIPO di INVIO/RICEZIONE DMX
// RICEZIONE = 0
// INVIO = 1

void DMX_Starter(uint32_t *bufferD, uint8_t DMXType){
    uint16_t lengthofBuf = 0;
    uint8_t DmxType = DMXType;                    // Costruttore  
    
    //Parte la lettura del DMX
   // if(DmxType==0){
         lengthofBuf = strlen(bufferD);               //lunghezza del buffer
                                                      
         if (TimerTime.app_tick_25ms){                //* Se è già nell'interrupt del main allora posso lasciare stare sinceramente  
             myMemCpy(PacchettoDMX,bufferD,lengthofBuf);            
         }

        FreqVar(lengthofBuf);
        Dmx_INIT=1;
        DMX_Scanner(Dmx_INIT,lengthofBuf,* PacchettoDMX);
    //}

    //Inizia la scrittura del buffer
    if (DmxType == 1){
        DMX_Send(bufferD);
        
    }
}


