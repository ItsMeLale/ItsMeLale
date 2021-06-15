//---------------------------------------------------------------------------
//
// MODULE           :   DATABASE.H
// DESCRIPTION      :   DATABASE module header file
//                              
//
// HISTORY           :
//      AUTHOR       :  Luca AGOSTINI
//      DATE         :  29/04/2003   
//      DESCRIPTION  :  First release
//
//
//---------------------------------------------------------------------------

/****************************************************************************
 * definitions
 ***************************************************************************/
#ifdef DATABASE_GLOBAL
    #define DATABASE_EXT   
#else 
    #define DATABASE_EXT extern
#endif

//-----------------------------------------------------------------------------
// IDENTIFIERS DEFAULT      -0123456789012345+
//-----------------------------------------------------------------------------
#define ST_ID_NUM           001
#define ST_COMPANY_NAME     "DALCNET Esempio  "
#define ST_MODEL_NAME       "NOME PRODOTTO    "
#define ST_MODEL_ID         "Codice prodotto  "
#define ST_SN               "0000000001"
#define ST_FW_VERS          "00.01"
#define ST_FW_VERS_NUM      001
#define ST_PRODDATE         "06/03/21"

// Define provvisoria
#define SLAVE_NUM 			2

// Disable to avoid debug functions
//#define DEBUG_MODE			1

/****************************************************************************
 * constants
 ***************************************************************************/


/****************************************************************************
 * data types
 ***************************************************************************/

//-------------------------------------------------------------------
// INTERNAL DATA
// IDENTIFIERS
typedef struct
{
    unsigned short  ID00;
    char            ID01FWVers[5];          // FW Version
    char            ID04ModelName[16];
    char            ID12Manufacturer[16];
    char            ID20SN[16];             // Version string

} tdIdent;


// STATES
typedef union
{
    struct
    {
        INT16U S00;
    } W;    
    struct
    {
        unsigned int s00InitState   : 1;    // Init state - device addressing
        unsigned int s01Free        : 1;    // 
        unsigned int s02Free        : 1;    // 
        unsigned int s03Free        : 1;    // 
        unsigned int s04Free        : 1;    // 
        unsigned int s05Free        : 1;	// 
        unsigned int s06DeviceOFF   : 1;	// 
        unsigned int s07Dev1Enabled : 1;	// 
        unsigned int s08WaitPres   	: 1;    // 
        unsigned int s09FanON    	: 1;    // 
        unsigned int s10VInOk    	: 1;    // 
        unsigned int s11TempOk      : 1;    // 
        unsigned int s12DbgSensor   : 1;    // 
        unsigned int s13ModeOut1    : 1;	// 
        unsigned int s14ModeOut2    : 1;	// 
        unsigned int s15ModBusON    : 1;    // Modbus commuincation is ON
    } B;
} tdU_STAT0;        
                                        
typedef union
{
    struct
    {
        INT16U S01;
    } W;    
    struct
    {
        unsigned int s16Free        : 1;    // 
        unsigned int s17Free        : 1;    // 
        unsigned int s18Free        : 1;    // 
        unsigned int s19Free        : 1;    // 
        unsigned int s20Free        : 1;    // 
        unsigned int s21Free        : 1;	// 
        unsigned int s22Free        : 1;	// 
        unsigned int s23Free        : 1;	// 
        unsigned int s24Free        : 1;    // 
        unsigned int s25Free        : 1;    // 
        unsigned int s26Free        : 1;    // 
        unsigned int s27Free        : 1;    // 
        unsigned int s28Free        : 1;    // 
        unsigned int s29Free        : 1;	// 
        unsigned int s30Free        : 1;	// 
        unsigned int s31ModBusMM    : 1;    // Thiis mode is used to control completely the DEVICE by modbus
    } B;
} tdU_STAT1;        

// FLAGS
typedef union
{
    struct
    {
        INT16U F00;
    } W;    
    struct
    {
        unsigned int f00PowOnDelay  : 1;    // Startup initialization done
        unsigned int f01ADCStart    : 1;    // ADC Start flag
        unsigned int f02ADCEos      : 1;    // ADC End of Conversion flag
        unsigned int f03FanFBIN     : 1;    // FAN Feedback Input
        unsigned int f04PWM1Inited  : 1;    // PWM1 Initialized
        unsigned int f05PWM2Inited  : 1;    // PWM2 Initialized
        unsigned int f06PWM3Inited  : 1;    // PWM3 Initialized
        unsigned int f07PWM4Inited  : 1;    // PWM4 Initialized
        unsigned int f08TempOutLim  : 1;    // Temperature is Out of limits
        unsigned int f09TempInLim   : 1;    // Temperature is Within the limits
        unsigned int f10VInOutLim   : 1;    // Input Voltage is Out of limits
        unsigned int f11VInInLim    : 1;	// Input Voltage is Within the limits
        unsigned int f12BlkIncCnt   : 1;    // Blink Cnt increment number
        unsigned int f13BlkOld      : 1;    // Blink Old state
        unsigned int f14ModbusCmd   : 1;    // A Modbus Command has been received
        unsigned int f15ModbusOn    : 1;    // Modbus ON previous state

    } B;
} tdU_FLAG0;


typedef union
{
    struct
    {
        INT16U F01;
    } W;    
    struct
    {
        unsigned int f16SensorIN    : 1;    // Sensor IN pin state
        unsigned int f17SafeIN      : 1;    // Safe IN pin state
        unsigned int f18Conf1IN     : 1;    // Configuration IN1 pin
        unsigned int f19Conf2IN     : 1;	// Configuration IN2 pin
        unsigned int f20ForceSusp   : 1;    // Force Suspension
        unsigned int f21Free        : 1;    // 
        unsigned int f22ButtSetProg : 1;    // Button long duration enter Program Mode
        unsigned int f23ButtSetPOld : 1;    // Button long duration old state
        unsigned int f24ButtProg    : 1;    // Button std duration Program cycles
        unsigned int f25ButtProgOld : 1;    // Button std duration Program cycles old state
        unsigned int f26SetProg     : 1;    // Set program mode action
        unsigned int f27Prog        : 1;    // Program action 
        unsigned int f28FastBlinkOn : 1;    // Fast blink ON activation
        unsigned int f29NormalBlkOn : 1;    // Normal blink ON activation
        unsigned int f30SlowBlinkOn : 1;    // Slow blink ON activation
        unsigned int f31VrySlwBlkOn : 1;    // Very Slow blink ON activation
    } B;
} tdU_FLAG1;

typedef union
{
    struct
    {
        INT16U F02;
    } W;    
    struct
    {
        unsigned int f32ButtSusp    : 1;    // Button suspension activation
        unsigned int f33ButtSuspOld : 1;    // Button suspension activation old state
        unsigned int f34MBusFrcSusp : 1;    // Modbus command force suspension mode
        unsigned int f35MBusFrcUVC  : 1;    // Modbus command force UVC full mode
        unsigned int f36MBusExtSusp : 1;    // Modbus command force EXIT suspension 
        unsigned int f37MBusExtUVC  : 1;    // Modbus command force EXIT UVC full mode
        unsigned int f38FanFBINOld  : 1;    // FAN Feedback Input - OLD state
        unsigned int f39FanFBRecvd  : 1;    // FAN Feedback Input - has changed
        unsigned int f40CalcFANSpd  : 1;    // FAN Speed calculation
        unsigned int f41LongBlink   : 1;    // Long BLINK signalling
        unsigned int f42MBusExtALM  : 1;    // Modbus command force EXIT ALARM condition
        unsigned int f43Free        : 1;    // 
        unsigned int f44Free        : 1;    // 
        unsigned int f45Free        : 1;    // 
        unsigned int f46Free        : 1;    // 
        unsigned int f47Free        : 1;    // 
    } B;
} tdU_FLAG2;

// ALARMS
typedef union
{
    struct 
    {
        INT16U A00;
    }W;    
    struct
    {
        unsigned int a00GenAlarm    : 1;    // Generic alarm 
        unsigned int a01OverTemp    : 1;    // Over Temperature limits reached alarm
        unsigned int a02FanNotOk    : 1;    // FAN is not working properly
        unsigned int a03VInNotOK    : 1;    // Input voltage NOT OK
        unsigned int a04Free        : 1;    // 
        unsigned int a05Free        : 1;
        unsigned int a06Free        : 1;
        unsigned int a07Free        : 1;
        unsigned int a08Free        : 1;    //
        unsigned int a09Free        : 1;    //
        unsigned int a10Free        : 1;    //
        unsigned int a11Free        : 1;
        unsigned int a12Free        : 1;
        unsigned int a13Free        : 1;
        unsigned int a14ModBusHWErr : 1;    // Modbus HW Error
        unsigned int a15ModbusTO    : 1;    // Modbus Timeout Alarm
     } B;   
} tdU_ALM0;    



// MEASUREMENTS
typedef struct                          
{
   INT16U M00IntTemp               ; // Internal Temperature [°C/10]
   INT16U M01Out1Val               ; // Dimming 1 value * [V/100]
   INT16U M02Out2Val               ; // Dimming 2 value * [V/100]
   INT16U M03Out3Val               ; // Dimming 3 value * [V/100]
   INT16U M04VINVal                ; // Supply voltage value [V/100]
   INT16U M05FanRPM	               ; // FAN rpm measurements
   INT16U M06Free                  ; // 
   INT16U M07Free                  ; // 
   INT16U M08Free                  ; // 
   INT16U M09Free                  ; // 
   INT16U M10Free                  ; // 
   INT16U M11Free                  ; //
   INT16U M12Free                  ; //
   INT16U M13Free                  ; //
   INT16U M14Free                  ; //
   INT16U M15Free                  ; //
}  tdMeas;


// POWER 
typedef struct
{
    INT16U powIDX                 ;     // Current Power Index
    INT16U powVal[64]             ;     // Power value 

} tdPower;    




// SETTINGS
typedef struct                          
{
   INT16U SET00Free                ; //
   INT16U SET01Free                ; //
   INT16U SET02Free                ; //
   INT16U SET03Free                ; //
   INT16U SET04Free                ; //
   INT16U SET05Free                ; //
   INT16U SET06Free                ; //
   INT16U SET07Free                ; //
   INT16U SET08Free                ; //
   INT16U SET09Free                ; //
   INT16U SET10Free                ; //
   INT16U SET11Free                ; //
   INT16U SET12Free                ; //
   INT16U SET13Free                ; //
   INT16U SET14Free                ; //
   INT16U SET15SlvNum              ; // Modbus module slave number
}  tdSett;

// PARAMETERS
typedef struct
{
   INT16U  P00Free                 ;   //
   INT16U  P01Free                 ;   //
   INT16U  P02Free                 ;   //
   INT16U  P03Free                 ;   // 
   INT16U  P04Free                 ;   // 
   INT16U  P05Free                 ;   // 
   INT16U  P06Free                 ;   // 
   INT16U  P07Free                 ;   // 
   INT16U  P08Free                 ;   // 
   INT16U  P09Free                 ;   //
   INT16U  P10Free                 ;   // 
   INT16U  P11Free                 ;   // 
   INT16U  P12Free                 ;   // 
   INT16U  P13Free                 ;   // 
   INT16U  P14Free                 ;   // 
   INT16U  P15Free                 ;   // 
   INT16U  P16Free                 ;   // 
   INT16U  P17Free                 ;   // 
   INT16U  P18Free                 ;   // 
   INT16U  P19Free                 ;   // 
   INT16U  P20Free                 ;   // 
   INT16U  P21Free                 ;   // 
   INT16U  P22Free                 ;   // 
   INT16U  P23Free                 ;   // 
   INT16U  P24Free                 ;   // 
   INT16U  P25Free                 ;   // 
   INT16U  P26Free                 ;   // 
   INT16U  P27Free                 ;   // 
   INT16U  P28Free                 ;   // 
   INT16U  P29Free                 ;   // 
   INT16U  P30Free                 ;   // 
   INT16U  P31Free                 ;   //
} tdParam;


// COMMANDS
typedef struct
{
   INT16U C00CmdCode               ; // Codice comando
   INT16U C01CmdPar1               ; // Comando parametro 1
   INT16U C02CmdPar2               ; // Comando parametro 2
   INT16U C03CmdPar3               ; // Comando parametro 3
   INT16U C04CmdPar4               ; // Comando parametro 4
   INT16U C05CmdPar5               ; // Comando parametro 5
   INT16U C06CmdPar6               ; // Comando parametro 6
   INT16U C07CmdPar7               ; // Comando parametro 7
}  tdComm;


/****************************************************************************
 * variables
 ***************************************************************************/


//*******************************************************************
//                      PROTOTYPES
//*******************************************************************
void vDataBaseInit(void);

//*******************************************************************
//                      VARIABLES
//*******************************************************************
//-------------------------------------------------------------------
// PUBLIC INFORMATION 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// SOLAR INVERTER MAPS
//-------------------------------------------------------------------

// INTERNAL DATA

// Identifiers
DATABASE_EXT tdIdent stIdent;
// States
DATABASE_EXT tdU_STAT0 uSTAT0;
DATABASE_EXT tdU_STAT1 uSTAT1;

// Flags
DATABASE_EXT tdU_FLAG0 uFLAG0;
DATABASE_EXT tdU_FLAG1 uFLAG1;
DATABASE_EXT tdU_FLAG2 uFLAG2;

// Alarms
DATABASE_EXT tdU_ALM0 uALM0;
// Measurements
DATABASE_EXT tdMeas stMeas;
// Settings
DATABASE_EXT tdSett stSett;
// Parameters
DATABASE_EXT tdParam stParam;
// Commands
DATABASE_EXT tdComm stComm;
