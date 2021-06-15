/*
?  ███████╗██╗██╗  ██╗████████╗██╗   ██╗██████╗ ███████╗
?  ██╔════╝██║╚██╗██╔╝╚══██╔══╝██║   ██║██╔══██╗██╔════╝
?  █████╗  ██║ ╚███╔╝    ██║   ██║   ██║██████╔╝█████╗
?  ██╔══╝  ██║ ██╔██╗    ██║   ██║   ██║██╔══██╗██╔══╝
?  ██║     ██║██╔╝ ██╗   ██║   ╚██████╔╝██║  ██║███████╗
?  ╚═╝     ╚═╝╚═╝  ╚═╝   ╚═╝    ╚═════╝ ╚═╝  ╚═╝╚══════╝
*/

#ifndef FIXTURE_H
#define FIXTURE_H
#define Fixture_normal

    #ifdef Fixture_normal
        #define channel 128
        #define value   4
    #endif

    #ifdef Fixture_slider
        #define channel 102
        #define value   5
    #endif

    #ifdef Fixture_testeRotanti
        #define channel 85
        #define value   6
    #endif

    #ifdef Fixture_multiCH
        #define channel 64
        #define value   8
    #endif

    #ifdef Fixture_MultiTeste
        #define channel 51
        #define value   10
    #endif

#endif