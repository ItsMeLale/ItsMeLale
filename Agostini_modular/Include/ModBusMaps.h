//---------------------------------------------------------------------------
//
// MODULE           :   MODBUSMAPS.H
// DESCRIPTION      :   MODBUSMAPS modul header file
//                              
//
// HISTORY           :
//      AUTHOR       :  Luca AGOSTINI
//      DATE         :  28/11/2005   
//      DESCRIPTION  :  First release
//
//
// 
//---------------------------------------------------------------------------


/****************************************************************************
 * definitions
 ***************************************************************************/
#ifdef MODBUSMAPS_GLOBAL
    #define MODBUSMAPS_EXT   
#else 
    #define MODBUSMAPS_EXT extern
#endif                                             


// Enable the following define if the Modbus master function is enabled
#define MODBUS_MST      (1)        
// change this define depending on the call routin timer in main function
#define MBM_TIMER   (10 mS)

/* marker fine mappa modbus */
#define NULL_MAPPTR    ((t_PUNTATORE)0)

/* marker write non ammesso */
#define NULL_WRITE     ((t_WRITEWORD *)0)

/* comandi noti modbus */
#define CMD_READWORD   ( 3)
#define CMD_WRITE1WORD ( 6)
#define CMD_WRITEWORDS (16)

/****************************************************************************
 * constants
 ***************************************************************************/
enum {
MAP_ID=0,                   // IDENTIFIERS
MAP_STATES,                 // STATES
MAP_FLAGS,                  // FLAGS
MAP_ALARMS,                 // ALARMS
MAP_MEAS,                   // MEASUREMENTS
MAP_CYCLES,                 // CYCLEs STATE
MAP_POWER,                  // AVG POWER MAP 1'
MAP_DIAG,                   // DIAGNOSTIC MAP
MAP_DATETIME,               // DATE & TIME
MAP_SETT,                   // SETTING
MAP_PARM,                   // PARAMETERS
MAP_CMD,                    // COMMANDS
MAX_MAP_DESCR
};

/* stato di trasmissione */
enum
  {
  TX_SLAVE,
  TX_CMD,
  TX_DATA,
  TX_CRCL,
  TX_CRCH,
  TX_END
  };


/****************************************************************************
 * data types
 ***************************************************************************/
/* puntatore ad una variabile da inserire nelle mappe MODBUS    */
/* t_PUNTATORE � un puntatore a INT8U, se scrivo (t_PUNTATORE) */
/* con le parentesi � un puntatore con cast a INT8U            */
typedef INT8U *t_PUNTATORE;                  

/* mappa MODBUS definisce t_MAPPA_MODBUS come puntatore a INT8U */
typedef INT8U *T_MAPPA_MODBUS;

/* puntatore ad un elemento della mappa MODBUS */
typedef const t_PUNTATORE *t_PTRMAPPA_MODBUS; 

/* puntatore ad una funzione di scrittura di INT16U */
/* t_WRITEWORD � un puntatore a funz. con tre par. che ritorna una INT16U */
typedef INT16U (t_WRITEWORD)(void);    

/* elemento descrittore della mappa MODBUS */
typedef struct
{
   INT16U start;
   INT16U stop;
   t_PTRMAPPA_MODBUS inizio;
   t_WRITEWORD *write_funct;
   INT8U *write_permission;   
   INT8U *read_permission;
} t_DESCR_MAPPA_MODBUS;                       


/****************************************************************************
 * variables
 ***************************************************************************/
MODBUSMAPS_EXT BOOLEAN BRW_True;       // permission always true
//MODBUSMAPS_EXT BOOLEAN BW_CPerm;       // permission to write on internal variables
//MODBUSMAPS_EXT BOOLEAN BR_CPerm;       // permission to read the internal variables

// READ ONLY MAPS
// STATES
MODBUSMAPS_EXT const T_MAPPA_MODBUS states_Map[5]; 

// FLAGS
MODBUSMAPS_EXT const T_MAPPA_MODBUS flags_Map[9]; 

// ALARMS
MODBUSMAPS_EXT const T_MAPPA_MODBUS alarms_Map[5];

// MEASUREMENTS
MODBUSMAPS_EXT const T_MAPPA_MODBUS meas_Map[33];

// CYCLEs MAP
MODBUSMAPS_EXT const T_MAPPA_MODBUS cycle_Map[33];

// CYCLEs MAP
MODBUSMAPS_EXT const T_MAPPA_MODBUS power_Map[131];

// INTERNAL VARIABLES, DIAGNOSTIC
MODBUSMAPS_EXT const T_MAPPA_MODBUS diag_Map[33];

// DATE & TIME MAP
MODBUSMAPS_EXT const T_MAPPA_MODBUS datetime_Map[13]; 

// READ AND WRITE MAP
// IDENTIFIERS                                  
MODBUSMAPS_EXT const T_MAPPA_MODBUS ID_Map[54];

// SETTING
MODBUSMAPS_EXT const T_MAPPA_MODBUS sett_Map[33];

// PARAMETERS MAP
MODBUSMAPS_EXT const T_MAPPA_MODBUS param_Map[65];

/* COMMANDS */
MODBUSMAPS_EXT const T_MAPPA_MODBUS cmd_Map[17];


/* MODBUS MAPS */
MODBUSMAPS_EXT const t_DESCR_MAPPA_MODBUS descr_mappe[MAX_MAP_DESCR];

