/*
?  ███████╗██╗██╗  ██╗████████╗██╗   ██╗██████╗ ███████╗
?  ██╔════╝██║╚██╗██╔╝╚══██╔══╝██║   ██║██╔══██╗██╔════╝
?  █████╗  ██║ ╚███╔╝    ██║   ██║   ██║██████╔╝█████╗
?  ██╔══╝  ██║ ██╔██╗    ██║   ██║   ██║██╔══██╗██╔══╝
?  ██║     ██║██╔╝ ██╗   ██║   ╚██████╔╝██║  ██║███████╗
?  ╚═╝     ╚═╝╚═╝  ╚═╝   ╚═╝    ╚═════╝ ╚═╝  ╚═╝╚══════╝
*/

#include <stdint.h>
#ifndef FIXTURE_H
#define FIXTURE_H

extern uint8_t channel;
extern uint8_t value; 
uint8_t fixture_type;

typedef enum {
    normal = 1      ,                   //
    slider          , 
    testeRotanti    ,
    multiCH         ,
    multiTeste
}Fixture; 




void start_fixtures(Fixture x){


  
    switch (x) {
        case 1 : 
                channel = 128;
                value =  4   ;
        break; 

        case 2 : 
                channel = 102;
                value =  5   ;
        break; 
    
         case 3 : 
                channel = 85 ;
                value =  6   ;
        break; 

        case 4 : 
                channel = 64 ;
                value =  8   ; 
        break; 

        case 5 : 
                channel = 51 ;
                value = 10   ; 
        break; 

        default:
                channel=0;
                value=0; 
        break;
    } 

}
#endif



