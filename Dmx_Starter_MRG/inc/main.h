//----------------------------------------------------------------------------------------------------------------//                                                                
/*
?  ██████╗ ███████╗███████╗██╗███╗   ██╗███████╗███████╗    ██████╗ ███╗   ███╗██╗  ██╗
?  ██╔══██╗██╔════╝██╔════╝██║████╗  ██║██╔════╝██╔════╝    ██╔══██╗████╗ ████║╚██╗██╔╝
?  ██║  ██║█████╗  █████╗  ██║██╔██╗ ██║█████╗  ███████╗    ██║  ██║██╔████╔██║ ╚███╔╝
?  ██║  ██║██╔══╝  ██╔══╝  ██║██║╚██╗██║██╔══╝  ╚════██║    ██║  ██║██║╚██╔╝██║ ██╔██╗
?  ██████╔╝███████╗██║     ██║██║ ╚████║███████╗███████║    ██████╔╝██║ ╚═╝ ██║██╔╝ ██╗
?  ╚═════╝ ╚══════╝╚═╝     ╚═╝╚═╝  ╚═══╝╚══════╝╚══════╝    ╚═════╝ ╚═╝     ╚═╝╚═╝  ╚═╝
*/
//----------------------------------------------------------------------------------------------------------------//

#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>
#include <stdbool.h>



/**=======================================================================================================================
 *                                                     Main variables
 *=======================================================================================================================**/

typedef struct {
   uint8_t TimeError    ;
   uint8_t DataError    ;  
   uint8_t GenericError ;     
    
}ErrorStat;




/**=======================================================================================================================
 *                                                   Definizioni del DMX
 *=======================================================================================================================**/
typedef struct {
  bool is_rdm;
  uint32_t time_Break;

// ---- define total packets -------------------------------------------------------------
  #define	tempoScadenzaTimeout   1500000   // In questo tempo ho perso 6 pacchetti 
  #define	tild_BAUD			      	  270000   // Velocità dei pacchetti 
  #define SlotData_Size               11  
  #define DMX_sipPck                  26   // Per il System Information Packet
  #define DMX_size                   512   // Massimale di pacchetti inviati o ricevuti

// ---- define timer packets -------------------------------------------------------------
  #define Start_Address               44  

  #define Start_Code                   4  // tempo singolo pacchetto  
  #define Channel_Data                40  

  #define Mark_afterBreak              6  // ne servono 2 
  #define Break_Time		              88  // time between packets
  #define	RDM_Break_Time           	 176  // RDM time between packets

  #define total_time_garanted        110  //tempo per inviare tutti i pacchetti  

} timeDefiners;

/**=======================================================================================================================
 *                                                    Valore degli Slots
 *=======================================================================================================================**/
typedef struct {
 
  uint32_t value_Break;

  #define value_dmxStCode           0x00
  #define value_rdmStCode           0xCC
  #define value_testsCode           0x55

  #define value_MAB                 0x03

}value_define;

// ---- define board options -------------------------------------------------------------


// ---- define dmx options sends ---------------------------------------------------------






#endif