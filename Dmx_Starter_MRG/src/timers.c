//----------------------------------------------------------------------------------------------------------------//
/*
?   ████████╗██╗███╗   ███╗███████╗██████╗ ███████╗    ██████╗ ███████╗███████╗
?   ╚══██╔══╝██║████╗ ████║██╔════╝██╔══██╗██╔════╝    ██╔══██╗██╔════╝██╔════╝
?      ██║   ██║██╔████╔██║█████╗  ██████╔╝███████╗    ██║  ██║█████╗  █████╗
?      ██║   ██║██║╚██╔╝██║██╔══╝  ██╔══██╗╚════██║    ██║  ██║██╔══╝  ██╔══╝
?      ██║   ██║██║ ╚═╝ ██║███████╗██║  ██║███████║    ██████╔╝███████╗██║
?      ╚═╝   ╚═╝╚═╝     ╚═╝╚══════╝╚═╝  ╚═╝╚══════╝    ╚═════╝ ╚══════╝╚═╝
*/  
//----------------------------------------------------------------------------------------------------------------//
#include <stdint.h>
#include "./inc/timers.h"

time_check TimerTime;
time_check CronoTime;

cronoOptions cronotip; 

uint16_t	app_cnt_1ms;
uint16_t 	start_1ms;
uint16_t	app_cnt_10ms;
uint16_t	app_cnt_25ms;
uint16_t	app_cnt_100ms;
uint16_t	app_cnt_125ms;
uint16_t    app_cnt_1000ms;

/**-----------------------------------------------------------------------------------------------------------------------
 * *                                                     INFO
 *   Tempistiche variabili per tempistiche determinate 
 *   Quando passa tot tempo, il timer viene marcato (settato a 1) 
 *   Il timer viene resettato e impostato a 1 il flag che torna a 0 dopo 1 secondo.
 *-----------------------------------------------------------------------------------------------------------------------**/
void TimerIsOn(void){
    
    TimerTime.app_tick_1ms	 =0;
	TimerTime.app_tick_10ms	 =0;
	TimerTime.app_tick_25ms	 =0;
	TimerTime.app_tick_100ms	 =0;
	TimerTime.app_tick_125ms	 =0;
	TimerTime.app_tick_1000ms =0;

	app_cnt_1ms++;
	
	if(app_cnt_1ms>=1){
		app_cnt_1ms		=0;
		TimerTime.app_tick_1ms	=1;	
	}

    app_cnt_10ms++;
	if(app_cnt_10ms>=10){
		app_cnt_10ms	=0;
		TimerTime.app_tick_10ms	=1;
	}

    app_cnt_25ms++;
	if(app_cnt_25ms>=25){
		app_cnt_25ms	=0;
		TimerTime.app_tick_25ms	=1;
	}

    app_cnt_100ms++;
	if(app_cnt_100ms>=100){
		app_cnt_100ms	=0;
		TimerTime.app_tick_100ms	=1;
	}
    app_cnt_125ms++;
	if(app_cnt_125ms>=125){
		app_cnt_125ms	=0;
		TimerTime.app_tick_125ms	=1;
	}
    app_cnt_1000ms++;
	if(app_cnt_1000ms>=1000){
		app_cnt_1000ms	=0;
		TimerTime.app_tick_1000ms=1;
	}

}

/**-----------------------------------------------------------------------------------------------------------------------
 * *                                                     INFO
 *   salva lo stato del Timer quando viene fermato 
 *   Resetta la variabile del timer 
 *   Setta ad ON lo stato resettato
 *-----------------------------------------------------------------------------------------------------------------------**/
void CronoIsOFF(void){ 
	// Salvataggio data
	cronotip.saveState=cronotip.timespent;
	// Resetto il tempo
	cronotip.timespent = 0;
	start_1ms = 0; 
	cronotip.resettingState = 1;
}



void CronoIsON(void){	
	//Se il cronometro è resettato e parto da 0	
	if (cronotip.resettingState=1){cronotip.resettingState = 0;}
	//Se non era resettato prima di partire lo resetto.
		else CronoIsOFF();
	start_1ms++;
	
	if(start_1ms>=1){
		start_1ms		= 0;
		TimerTime.app_tick_1ms	= 1;	
		cronotip.timespent ++;
	}

}