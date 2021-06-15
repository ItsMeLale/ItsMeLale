//---------------------------------------------------------------------------
//
// MODULE           :   ModBus.H
// DESCRIPTION      :   interfaccia del modulo ModBus.c
//                              
//---------------------------------------------------------------------------

/****************************************************************************
 * definitions
 ***************************************************************************/
#ifdef MODBUS_GLOBAL
    #define MODBUS_EXT   
#else 
    #define MODBUS_EXT extern
#endif                                             

/* errori modbus */
#define ERR_NONE  (0)
#define ERR_CMD   (1)
#define ERR_ADDR  (2)
#define ERR_CCRC  (3)
#define ERR_NSUP  (4)
#define ERR_LEN   (5)

#define ERR_SEED   (ERR_NSUP)
#define ERR_WRITEL (ERR_NSUP)
#define ERR_WRITEI (ERR_NSUP)
#define ERR_FLASHW (ERR_NSUP)

#define ERR_RECEIVE_OK     ( 0)
#define ERR_REQUEST_OK     ( 0)
#define ERR_RXLOOP_OK      ( 0)
#define ERR_SEND_OK        ( 0)

/* valore convenzionale per indicare una posizione di mappa non gestita */
#define VALORE_NULL (0xff)      

/* valore convenzionale per indicare una posizione di mappa gestita ma fissa */
#define VALORE_ZERO (0)

/* dimensione del buffer di ricezione (byte) */
#define MAX_WRITE_WORDS (64)
#define RX_BUFFER       ((MAX_WRITE_WORDS * 2) + 8)


/****************************************************************************
 * variables
 ***************************************************************************/
MODBUS_EXT BOOLEAN BWritePermitted;
MODBUS_EXT volatile BOOLEAN bRxIntrUartEnabled;
MODBUS_EXT volatile BOOLEAN bTxIntrUartEnabled;

/* interrupt di ricezione ON/OFF */
#define RXINT_ENABLE   {bRxIntrUartEnabled = 1;}
#define RXINT_DISABLE  {bRxIntrUartEnabled = 0;}
#define RXINT_ISENABLED (bRxIntrUartEnabled == 1)

/* interrupt di trasmissione ON/OFF */
#define TXINT_ENABLE   {bTxIntrUartEnabled = 1;}
#define TXINT_DISABLE  {bTxIntrUartEnabled = 0;}
#define TXINT_ISENABLED (bTxIntrUartEnabled == 1)

#define UART_DIR485_TX {}   // Setta il driver in modalit� TX
#define UART_DIR485_RX {}   // Setta il driver in modalit� RX

/****************************************************************************
 * prototypes
 ***************************************************************************/
void vModBusInit(UART_HandleTypeDef *p_huart);
void vModBusPoll(void);
//void vModBusRxInt(INT8U rx_char_ser0);
//void vModBusTxInt(void);
void vModBusCheckCommunication(void);
              


