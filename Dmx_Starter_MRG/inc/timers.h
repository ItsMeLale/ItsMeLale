//----------------------------------------------------------------------------------------------------------------//
/*
? ████████╗██╗███╗   ███╗███████╗██████╗ ███████╗    ███████╗███████╗ ██████╗████████╗██╗ ██████╗ ███╗   ██╗███████╗
? ╚══██╔══╝██║████╗ ████║██╔════╝██╔══██╗██╔════╝    ██╔════╝██╔════╝██╔════╝╚══██╔══╝██║██╔═══██╗████╗  ██║██╔════╝
?    ██║   ██║██╔████╔██║█████╗  ██████╔╝███████╗    ███████╗█████╗  ██║        ██║   ██║██║   ██║██╔██╗ ██║███████╗
?    ██║   ██║██║╚██╔╝██║██╔══╝  ██╔══██╗╚════██║    ╚════██║██╔══╝  ██║        ██║   ██║██║   ██║██║╚██╗██║╚════██║
?    ██║   ██║██║ ╚═╝ ██║███████╗██║  ██║███████║    ███████║███████╗╚██████╗   ██║   ██║╚██████╔╝██║ ╚████║███████║
?    ╚═╝   ╚═╝╚═╝     ╚═╝╚══════╝╚═╝  ╚═╝╚══════╝    ╚══════╝╚══════╝ ╚═════╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝╚══════╝
*/
//----------------------------------------------------------------------------------------------------------------//
#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

typedef struct 
{
    uint8_t app_tick_1ms	;
    uint8_t	app_tick_10ms	;
    uint8_t	app_tick_25ms	;
    uint8_t	app_tick_100ms	;
    uint8_t	app_tick_125ms	;
    uint8_t	app_tick_1000ms	;
}   time_check;

typedef struct 
{
    uint8_t timespent       ;
    uint8_t resettingState  ;
    uint8_t saveState       ;      

}   cronoOptions;


/**=======================================================================================================================
 *                                                    Funzioni globali
 * =======================================================================================================================
**/
void TimerIsOn();
void CronoIsON();
void CronoIsOFF();



#endif