/*
?   ███████╗████████╗ █████╗ ██████╗ ████████╗███████╗██████╗      ██████╗ ███████╗    ██████╗ ███╗   ███╗██╗  ██╗
?   ██╔════╝╚══██╔══╝██╔══██╗██╔══██╗╚══██╔══╝██╔════╝██╔══██╗    ██╔═══██╗██╔════╝    ██╔══██╗████╗ ████║╚██╗██╔╝
?   ███████╗   ██║   ███████║██████╔╝   ██║   █████╗  ██████╔╝    ██║   ██║█████╗      ██║  ██║██╔████╔██║ ╚███╔╝
?   ╚════██║   ██║   ██╔══██║██╔══██╗   ██║   ██╔══╝  ██╔══██╗    ██║   ██║██╔══╝      ██║  ██║██║╚██╔╝██║ ██╔██╗
?   ███████║   ██║   ██║  ██║██║  ██║   ██║   ███████╗██║  ██║    ╚██████╔╝██║         ██████╔╝██║ ╚═╝ ██║██╔╝ ██╗
?   ╚══════╝   ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝   ╚══════╝╚═╝  ╚═╝     ╚═════╝ ╚═╝         ╚═════╝ ╚═╝     ╚═╝╚═╝  ╚═╝
*/                                    


/**
 *? =======================================================================================================================
 *?                                                   INCLUDES
 *? =======================================================================================================================
**/
#include <stdint.h>
#include <stdbool.h>


#include "./inc/dmxgo.h"
#include "./inc/getty.h"
#include "./inc/timers.h"
#include "./use/utility.h"
#include "./use/fixtures.h"
/**
 *? =======================================================================================================================
 *?                                                   DEFININGS
 *? =======================================================================================================================
**/

// uint32_t DataChannel[channel][value];
uint32_t DataChannel[DMX_size][SlotData_Size];


/**
 *? =======================================================================================================================
 *?                                                    GLOBAL Functions
 *? =======================================================================================================================
**/

              //Da implementare   
void DMX_Send(uint8_t bufferToSend){
    
    //Si può richiedere da un file o un task esterno
     start_fixtures(normal);
     
    /*Dinamicamente in base alla Fixture modifica il valore del suo buffer 
      in questo modo riesco a selezionare il tipo di fixture anche con un switch o 
      forse anche un Applicazione  
    */
  

    for (int ch = 1; ch<= channel ; ch++){                  /*I CANALI sono intesi come il totale di pacchetti che si possono mandare su 512
                                                              in base al tipo di fixture che si è impostata vedi file FIXTURE_H  
                                                            */
            for (uint8_t va = 0; va <= value; va++){        //Il VALORE intende i canali tipo R G B  che vanno inseriti con valori
                DataChannel[ch][va]=bufferToSend;           //Bisogna capire come arrivano i dati se viene fornito anche il canale.
            }

    }    


}