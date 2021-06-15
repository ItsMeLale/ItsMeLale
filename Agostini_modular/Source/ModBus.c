//---------------------------------------------------------------------------
// 			    
// MODULE           :   ModBus.C
// DESCRIPTION      :   Funzioni associate all'uso della seriale 2 
//                              
//---------------------------------------------------------------------------
/****************************************************************************
 * MODBUS extern define exsclusion
 ***************************************************************************/
#define MODBUS_GLOBAL   1

/****************************************************************************
 * include
 ***************************************************************************/
#include <string.h>			 

//---------------------------------------------------------------------------
// 			    
// MODULE           :   ModBus.C
// DESCRIPTION      :   Funzioni associate all'uso della seriale 2
//                              
//---------------------------------------------------------------------------

/****************************************************************************
 * include
 ***************************************************************************/
#include "main.h"
#include "stm32l0xx_hal_uart.h"

#include "userdefs.h"
#include "database.h"  
#include "modbus.h"
#include "modbusmaps.h"
#include "usrtimers.h"

/****************************************************************************
 * define
 ***************************************************************************/
/* numero di nodo slave broadcast */
#define NODO_MODBUSBRC (0x00)

#define MODBUS_TIMEOUTMS  6	 // ms before timeout
#define TIMOUT_RELOAD    40  // express in ms*250 (20*250)=10s

static INT8U ucTimeOutBeforeReset = TIMOUT_RELOAD;


/****************************************************************************
 * typedef
 ***************************************************************************/


/****************************************************************************
 * variabili locali
 ***************************************************************************/
// Handler UART da driver
UART_HandleTypeDef *mdb_huart;
uint8_t ucRxBuff;
uint8_t ucTxBuff;


/* buffer di ricezione MODBUS */
static union
{
   INT8U array[RX_BUFFER];
   /* questa struttura rappresenta il pacchetto MODBUS */
   struct
   {
      INT8U slave;
      INT8U cmd;
      INT8U data[RX_BUFFER - 2];
   } packet;
} rx_buffer;

/* uiModBus_Crc16 di ricezione e trasmissione */
static union
{
   INT16U w;
   /* questa struttura tiene conto della
      rappresentazione little endian degli INT16U nel Kinetis */
   struct
   {
      INT8U l;
      INT8U h;
   } b;
} crc_buffer;

/* caratteri da trasmettere nel buffer MODBUS */
static volatile INT8U buffer_count;

/* caratteri ricevuti dalla seriale 2 */
static volatile INT16U rx_buffer_count=0;

/* vera lunghezza dei dati ricevuti (se la lungh. eccede viene generato l'errore di lunghezza) */
static volatile INT16U rx_data_lenght;

/* caratteri da trasmettere da mappa MODBUS */
static INT8U txdata_count;

/* puntatore alla prossima posizione in mappa MODBUS da trasmettere */
static t_PTRMAPPA_MODBUS txdata_pointer;

/* dummy: utile per generare una posizione gestita ma fissa nelle mappe MODBUS */
static INT16U zero_position;

/* dummy: utile per generare una posizione non gestita nelle mappe MODBUS */
static INT16U null_position;

static volatile INT8U tx_state = TX_END;

/****************************************************************************
 * prototipi
 ***************************************************************************/
static INT8U ucModBus_Command(void);
static void vModBus_AnsOK(INT8U, INT8U);
static void vModBus_Error(INT8U, INT8U, INT8U);
static INT16U uiModBus_Crc16(INT16U, INT8U);
static INT16U uiModBus_CheckCrc(INT8U *, INT16U);
//void vModBusRxInt(INT8U rx_char_ser0);
void vModBusTxInt(void);
void vModBusRxInt(void);

/*---------------------------------------------------------------------------+
|
| NOME
|   void vModBusInit (void)
|
| DESCRIZIONE
|   Inizializza il modbus per l'utilizzo in base ai due dip switch,
|   passate come variabili globali
|
| NOTE
|
+---------------------------------------------------------------------------*/


void vModBusInit(UART_HandleTypeDef *p_huart)
{
   //static INT8U mod;
   
   /* variabili di sistema */
   // Handler
	mdb_huart = p_huart;
	//HAL_UART_RegisterCallback(mdb_huart, HAL_UART_TX_COMPLETE_CB_ID, (pUART_CallbackTypeDef) vModBusTxInt);
	//HAL_UART_RegisterCallback(mdb_huart, HAL_UART_RX_COMPLETE_CB_ID , (pUART_CallbackTypeDef) vModBusRxInt);
	//p_huart->pRxBuffPtr = &ucRxBuff[0];
	//p_huart->RxXferSize= 1;

   // Baud rate, specificarlo eventualmente con i DIP-SW 
   // di default 9600 
   //mod = ((ConfigDip8 & 0x60)>>5) & 0x03;
   //tmp= Uart2_SetBaudRateMode(mod);

   // gestisce abilitazione modbus eventualmente con DIP-SW
   //mod = 1; // debug

   /* variabili MODBUS */
   buffer_count  =0;
   rx_buffer_count=0;
   rx_data_lenght=0;
   null_position=VALORE_NULL;
   zero_position=VALORE_ZERO;

   // inizializza i timeout
   SET_USRTIMERS(USRTIM6,5 mS);   
   // Timeout comunicazione
   ucTimeOutBeforeReset = TIMOUT_RELOAD;

   UART_DIR485_TX;                 // setta TX per un istante per prevenire il fault del driver
   UART_DIR485_RX;
   RXINT_ENABLE;

   // Enable UART interrupt routine
   HAL_UART_Receive_IT(mdb_huart, &ucRxBuff, 1);
   // Set R/W maps access
   BRW_True=1;

}



/*---------------------------------------------------------------------------+
|
| NOME
|   void vModBusRxInt(INT8U rx_char_ser0)
|
| DESCRIZIONE
|   Funzione chiamata dall'interrupt di ricezione da seriale 2.
|   Trasferisce i caratteri ricevuti in apposito buffer; ad ogni carattere
|   ricevuto ripristina un timer che, con il proprio scadere, riconosce il
|   termine di un pacchetto.
|
|   Il buffer di ricezione e' organizzato in modo conforme alle specifiche
|   del protocollo modbus. Poiche' non si dispone di memoria RAM in abbondanza,
|   non si accetta di memorizzare un pacchetto lungo complessivamente piu'
|   di 16 caratteri; per ottenere cio', trascorso il sedicesimo carattere i
|   rimanenti vengono memorizzati sovrapponendosi alle ultime due posizioni
|   del buffer (in modalita' FIFO). In questo modo, si ha la garanzia di poter
|   comunque accedere al campo CRC del pacchetto ricevuto.
|
|   Ogni carattere ricevuto viene comunque immesso nel campo CRC (in modalita'
|   FIFO) in modo che allo scadere del timeout e' gia' pronto il CRC ricevuto.
|
| NOTE
|   La variable buffer_count indica in ogni istante il numero di caratteri presenti
|   nel buffer (e quindi punta alla posizione successiva all'ultimo); quindi
|   la massima dimensione del buffer risulta essere pari al massimo valore
|   raggiungibile da buffer_count (es. 255 per rx_count dichiarata come INT8U).
|
|   In base alle specifiche MODBUS per il funzionamento della comunicazione
|   modbus, il termine di un pacchetto viene riconosciuto per timeout della
|   ricezione di caratteri. In corrispondenza all'inizio della ricezione di
|   un nuovo pacchetto, il valore di buffer_count viene azzerato per maggiore
|   sicurezza (ad es. per eliminare caratteri spuri in linea).
|
|
+---------------------------------------------------------------------------*/
//void vModBusRxInt(INT8U rx_char_ser0)
void vModBusRxInt(void)
{

INT8U rx_char_ser0 = 0;

    //Read data from RX Buffer
	rx_char_ser0 = ucRxBuff;

	if (RXINT_ISENABLED)
    {
       /*---------------------------------------------------------*/
       /* se si tratta di un nuovo pacchetto, azzera il conteggio */
       /*---------------------------------------------------------*/
       if (END_USRTIMERS(USRTIM6)) 
       {
          rx_buffer_count=0;
          rx_data_lenght=0;
       }

       /*--------------------------------------------------------------*/
       /* ogni volta che si riceve un carattere carica il timer per il */
       /* riconoscimento della fine del pacchetto                      */
       /*--------------------------------------------------------------*/
       SET_USRTIMERS(USRTIM6, MODBUS_TIMEOUTMS mS);

       if (rx_buffer_count >= sizeof(rx_buffer))
       {
          /*--------------------------------------------------------------------------*/
          /* gia' raggiunta la dimensione del buffer: push l'ultimo carattere sul     */
          /* penultimo e salva il carattere ricevuto sull'ultima posizione del buffer */
          /*--------------------------------------------------------------------------*/
          rx_buffer.array[sizeof(rx_buffer)-2]=rx_buffer.array[sizeof(rx_buffer)-1];
          rx_buffer.array[sizeof(rx_buffer)-1]=rx_char_ser0;
          rx_data_lenght++;
       }
       else
       {
          /*------------------------------------*/
          /* continua il riempimento del buffer */
          /*------------------------------------*/
          rx_buffer.array[rx_buffer_count++]=rx_char_ser0;
          rx_data_lenght++;
       }
    }
}



/*---------------------------------------------------------------------------+
|
| NOME
|   void vModBusTxInt(void)
|
| DESCRIZIONE
|   Funzione chiamata dall'interrupt di trasmissione da seriale 2.
|   Trasmette uno alla volta i caratteri fino al loro esaurimento.
|   La trasmissione avviene in tre fasi:
|
|   1) trasmissione della testata (slave e cmd); in realta' il primo carattere
|      (slave) viene trasmesso dalla funzione che inizia il processo di
|      trasmissione di un nuovo pacchetto.
|
|   2) trasmissione del campo dati: questa fase puo' avvenire in due modi.
|      - fino a che il contatore buffer_count e' diverso da zero, si
|        considera quale campo dati il contenuto del buffer di ricezione;
|      - al termine della eventuale fase precedente, fino a che il contatore
|        txdata_count e' diverso da zero, i caratteri da trasmettere vengono
|        prelevati a partire da *(*txdata_pointer), con doppia
|        de-referenziazione. Se pero' il puntatore *txdata_pointer
|        contiene NULL_MAPPTR allora viene trasmesso il carattere VALORE_NULL.
|
|    3) trasmissione del campo uiModBus_Crc16: il valore uiModBus_Crc16 viene accumulato
|       durante tutta la trasmissione, e trasmesso al termine.
|
|   Infine, pulisce ed abilita l'interrupt di ricezione.
|
| NOTE
|   ATTENZIONE: non utilizza register bank; controllare l'uso dei registri
|               in compilazione.
|
+---------------------------------------------------------------------------*/
void vModBusTxInt(void)
{
static INT8U count;
static INT8U to_send=0;

   if (TXINT_ISENABLED)
   { 
       /*----------------------*/
       /* fase di trasmissione */
       /*----------------------*/
       switch (tx_state)
       {
          case TX_SLAVE:
             to_send=rx_buffer.packet.slave;
             crc_buffer.w=uiModBus_Crc16(0xffff, to_send);
             tx_state=TX_CMD;
             break;

          case TX_CMD:
             to_send=rx_buffer.packet.cmd;
             count=0;
             crc_buffer.w=uiModBus_Crc16(crc_buffer.w, to_send);
             tx_state=TX_DATA;
             break;

          case TX_DATA:
             if (buffer_count)
             {
                to_send=rx_buffer.packet.data[count++];
                buffer_count--;
                crc_buffer.w=uiModBus_Crc16(crc_buffer.w, to_send);
                break;
             }
             else if (txdata_count)
             {
                if (*txdata_pointer == NULL_MAPPTR) // fine tabella
                {
                   to_send=VALORE_NULL;
                }
                else if (*txdata_pointer == (t_PUNTATORE)&null_position)
                {
                   to_send=VALORE_NULL;
                   txdata_pointer++;
                }
                else
                {
                   to_send=*(*txdata_pointer);
                   txdata_pointer++;
                }
                txdata_count--;
                crc_buffer.w=uiModBus_Crc16(crc_buffer.w, to_send);
                break;
             }
             else
             {
                tx_state=TX_CRCL;
             }

          case TX_CRCL:
             to_send=crc_buffer.b.l;
             tx_state=TX_CRCH;
             break;

          case TX_CRCH:
             to_send=crc_buffer.b.h;
             tx_state=TX_END;
             break;

          default:
          case TX_END:
             /*------------------------------------*/
             /* buffer esaurito: abilita ricezione */
             /*------------------------------------*/
             buffer_count=0;
             txdata_count=0;
             rx_buffer_count=0;
             rx_data_lenght=0;
             tx_state=TX_END;
             TXINT_DISABLE;				
             UART_DIR485_RX;
             RXINT_ENABLE;         
             break;
       }
    }
   // invia il il prossimo carattere
   ucTxBuff = to_send;
   if (TXINT_ISENABLED)
	   HAL_UART_Transmit_IT(mdb_huart,&ucTxBuff, 1);
}

/*---------------------------------------------------------------------------+
|
| NOME
|   void vModBusPoll(void)
|
| DESCRIZIONE
|   Funzione di elaborazione dei pacchetti modbus ricevuti.
|   1) controlla se risulta ricevuto un pacchetto completo (mediante timeout
|      sui caratteri ricevuti da seriale);
|   2) esegue un controllo di consistenza sul pacchetto (numero di nodo,
|      lunghezza massima ammessa, uiModBus_Crc16);
|   3) esegue il comando richiesto oppure invia un pacchetto di errore.
|
|
|   Il punto 3) comporta in genere l'invio di un pacchetto di risposta che
|   viene costruito all'interno della funzione stessa.
|
| NOTE
|   Questa funzione deve essere chiamata preferibilmente con cadenza superiore
|   al timeout fissato per il riconoscimento del termine di un pacchetto.
|   Non richiede regolarita' nella cadenza di chiamata.
|
+---------------------------------------------------------------------------*/
void vModBusPoll(void)
{
INT8U error;


   /*--------------------------------------------------*/
   /* esiste un pacchetto completo di uiModBus_Crc16 in attesa? */
   /*--------------------------------------------------*/
   if (RXINT_ISENABLED && (END_USRTIMERS(USRTIM6)) && (rx_buffer_count > sizeof(crc_buffer)))
   {
      /*------------------------------------------------------------*/
      /* blocca la ricezione mentre processi questo pacchetto:      */
      /* la ricezione sara' ri-abilitata solo al termine dell'invio */
      /* dell'eventuale risposta                                    */
      /*------------------------------------------------------------*/
      RXINT_DISABLE;

      /* ricostruisci il crc ricevuto */
      crc_buffer.b.l=rx_buffer.array[rx_buffer_count-2];
      crc_buffer.b.h=rx_buffer.array[rx_buffer_count-1];

      /* uiModBus_Crc16 corretto? */
      if (crc_buffer.w == uiModBus_CheckCrc(rx_buffer.array, rx_buffer_count - 2))
      {
         /* il numero di nodo corrisponde? */
         if ((rx_buffer.packet.slave == stSett.SET15SlvNum) || (rx_buffer.packet.slave == NODO_MODBUSBRC))
         {
        	uSTAT0.B.s15ModBusON = 1;
            // Setta timeout di comunicazione
        	ucTimeOutBeforeReset = TIMOUT_RELOAD;

            /* lunghezza accettabile? */
            if (rx_buffer_count <= sizeof(rx_buffer))
            {

               /* comando accettato? */
               error=ucModBus_Command();
               if (error == ERR_NONE) {
                  vModBus_AnsOK(rx_buffer.packet.slave, rx_buffer.packet.cmd);
               }
               else {
                  vModBus_Error(rx_buffer.packet.slave, error, rx_buffer.packet.cmd);
               }
               // LAG: [06/02/2006] - per evitare risposte ripetitive
               rx_buffer_count = 0;
            }
            else
            {
               /* lunghezza non accettabile */
               vModBus_Error(rx_buffer.packet.slave, ERR_LEN, rx_buffer.packet.cmd);
            }
         }
         else
         {
            /* il numero di nodo non corrisponde: ricevi il prossimo */
            rx_buffer_count=0;
            RXINT_ENABLE;
         }
      }
      else
      {
          /* uiModBus_Crc16 non corrisponde: ricevi il prossimo */
          rx_buffer_count=0;
          RXINT_ENABLE;
      }
   }
}


/*---------------------------------------------------------------------------+
|
| NOME
|   INT16U uiModBus_CheckCrc(INT16U *buffer, INT16U lenght)
|
| DESCRIZIONE
|   Elabora il calcolo del uiModBus_Crc16 su di un buffer di linghezza specificata
|   (INT8U a INT8U) restituendo il risultato a INT16U.
|
| NOTE
|   Questa funzione e' completamente sospensiva. Se il buffer specificato ha
|   lunghezza zero, viene restituito il valore di default 0xffff.
|
+---------------------------------------------------------------------------*/
static INT16U uiModBus_CheckCrc(INT8U *buffer, INT16U lenght)
{
INT16U result;

   result=0xffff;
   while (lenght)
   {
      result=uiModBus_Crc16(result, *buffer);
      lenght--;
      buffer++;
   }

return(result);                                                       
}



/*---------------------------------------------------------------------------+
|
| NOME
|   INT8U ucModBus_Command(void)
|
| DESCRIZIONE
|   Esamina il buffer di ricezione ed esegue il comando richiesto; se non
|   accettato, restituisce un codice di errore.
|
| NOTE
|
+---------------------------------------------------------------------------*/
static INT8U ucModBus_Command(void)
{

/*----------------------**
** ADDRESS access union **
**----------------------*/
union {
   INT16U w;
   /* questa struttura tiene conto della rappresentazione little endian degli INT16U nel Kinetis */
   struct {
      INT8U l;
      INT8U h;
      } b;
   } address;

/*---------------------**
** LENGHT access union **
**---------------------*/
union {
   INT16U w;
   /* questa struttura tiene conto della rappresentazione little endian degli INT16U nel Kinetis */
   struct {
	      INT8U l;
	      INT8U h;
      } b;
   } lenght;

/*--------------------**
** VALUE access union **
**--------------------*/
union {
   INT16U w;
   /* questa struttura tiene conto della rappresentazione little endian degli INT16U nel Kinetis */
   struct {
	      INT8U l;
	      INT8U h;
      } b;
   } value[MAX_WRITE_WORDS];

INT8U indice;
INT8U stop;
INT8U i;
static INT16U locVar;

  switch (rx_buffer.packet.cmd)
  {
      /*---------------------------------------**
      ** comando non riconosciuto / supportato **
      **---------------------------------------*/
      default:
         return(ERR_CMD);
         break;


      /*-----------------**
      ** leggi piu' INT16U **
      **-----------------*/
      case CMD_READWORD:
         address.b.h=rx_buffer.packet.data[0];
         address.b.l=rx_buffer.packet.data[1];
         lenght.b.h =rx_buffer.packet.data[2];
         lenght.b.l =rx_buffer.packet.data[3];

         /* lunghezza non ammessa */
         if (((unsigned long)lenght.w * 2) >= 256)
         {
             return(ERR_LEN);
         }

         /* memorizza info numero modulo e punta alla mappa Ctrl. (0x1XXX) */
         address.b.h=address.b.h & 0x0F;
         
         /* controlla l'indirizzo d'inizio lettura */
         indice=0;
         stop=0;
         while ((indice < MAX_MAP_DESCR) && !stop)
         {

           /*------------------------------------------------------------**
           **  verifica se l'indirizzo � compreso tra quello delle mappe **
           **------------------------------------------------------------*/
 
            if ((address.w >= (descr_mappe[indice].start)) && (address.w <= (descr_mappe[indice].stop)) && (*(descr_mappe[indice].read_permission) == TRUE))
            {

               stop=1;
               /* inizio nella mappa */
               txdata_pointer=descr_mappe[indice].inizio;
               /* offset nella mappa */
               address.w -= descr_mappe[indice].start;
               /* Per gestire anche la lettura singola o a pacchetti */
               if (address.w != 0)   address.w=address.w * 2;
               
            }
            else   
            {
               indice++;
            }
         }


         /* indirizzo non valido */
         if (!stop)   return(ERR_ADDR);


         /* cerca il primo INT8U da spedire, fermandoti comunque dove termina la mappa */
         while ((*txdata_pointer != NULL_MAPPTR) && address.w)
         {
            address.w--;
            txdata_pointer++;
         }

         /* sistema il resto del buffer */
         rx_buffer.packet.data[0]=lenght.w * 2;
         buffer_count=1;
         txdata_count=lenght.w * 2;
         break;


      /*----------------**
      ** scrivi un INT16U **
      **----------------*/
      case CMD_WRITE1WORD:
         address.b.h =rx_buffer.packet.data[0];
         address.b.l =rx_buffer.packet.data[1];
         value[0].b.h=rx_buffer.packet.data[2];
         value[0].b.l=rx_buffer.packet.data[3];


         /* controlla l'indirizzo d'inizio scrittura*/
         indice=0;
         stop=0;
         while ((indice < (sizeof(descr_mappe)/sizeof(descr_mappe[0]))) && !stop)
         {
            /* l'indirizzo di scrittura � compreso nelle mappe? */
            if ((address.w >= descr_mappe[indice].start) && (address.w <= descr_mappe[indice].stop))
            {
               /* --- eventuale controllo se la mappa � protetta in scrittura --- */
               /* .....                                                           */
               stop=1;
               /* inizio nella mappa */
               txdata_pointer=descr_mappe[indice].inizio;
               /* offset nella mappa */
               address.w -= descr_mappe[indice].start;
               if (address.w != 0)   address.w=address.w * 2;
            }
            else   indice++;
         }


         /* indirizzo assente, la scrittura � consentita solo nella mappa controller (0x1XXX) */
         if (!stop)   return(ERR_ADDR);

         /* cerca il primo INT8U da scrivere, fermandoti comunque dove termina la mappa */
         while ((*txdata_pointer != NULL_MAPPTR) && address.w)
         {
            address.w--;
            txdata_pointer++;
         }

         /* controlla se mappa scrivibile */
        if ((descr_mappe[indice].write_funct == NULL_WRITE) || (*(descr_mappe[indice].write_permission)==FALSE))  return(ERR_NSUP);

         /* tenta di scrivere un INT16U (little endian)*/
        *(*txdata_pointer++) = HIBYTE(value[0].w);
        *(*txdata_pointer  ) = LOBYTE(value[0].w);
        if(!(descr_mappe[indice].write_funct())) return(ERR_NSUP);
         
         /* sistema il buffer */
         buffer_count=4;
         txdata_count=0;
         break;


      /*------------------**
      ** scrivi piu' INT16U **
      **------------------*/
      case CMD_WRITEWORDS:
         address.b.h=rx_buffer.packet.data[0];
         address.b.l=rx_buffer.packet.data[1];
         
         indice=0;

         /* controlla se troppi dati */
         if (rx_buffer.packet.data[3] > (sizeof(value) / sizeof(value[0])))   return(ERR_NSUP);

         /* recupera i dati */
         for (indice=0; indice < rx_buffer.packet.data[3]; indice++)
         {
            value[indice].b.h=rx_buffer.packet.data[5 + indice * 2];
            value[indice].b.l=rx_buffer.packet.data[6 + indice * 2];

         }

         /* controlla l'indirizzo d'inizio scrittura*/
         indice=0;
         stop=0;
         while ((indice < (sizeof(descr_mappe)/sizeof(descr_mappe[0]))) && !stop)
         {
            if ((address.w >= descr_mappe[indice].start) && (address.w <= descr_mappe[indice].stop))
            {
               /* --- eventuale controllo se la mappa � protetta in scrittura --- */
               /* .....                                                           */
               stop=1;
               /* inizio nella mappa */
               txdata_pointer=descr_mappe[indice].inizio;
               /* offset nella mappa */
               address.w -= descr_mappe[indice].start;
               if (address.w != 0)   address.w=address.w * 2;
            }
            else   indice++;
         }


         /* indirizzo assente, la scrittura � consentita solo nella mappa controller (0x1XXX) */
         if (!stop)   return(ERR_ADDR);
         /* cerca il primo INT8U da scrivere, fermandoti comunque dove termina la mappa */
         while ((*txdata_pointer != NULL_MAPPTR) && address.w)
         {
            address.w--;
            txdata_pointer++;
         }

         /* controlla se mappa scrivibile */
         if ((descr_mappe[indice].write_funct == NULL_WRITE) || (*(descr_mappe[indice].write_permission)==FALSE))  return(ERR_NSUP);
         /* scrive byte per byte sulla mappa destinazione, salvo incontrare la fine anticipatemente */
         i = 0;
         while ((*txdata_pointer != NULL_MAPPTR) && (i<rx_buffer.packet.data[3]))
         {
            /* tenta di scrivere piu' INT16U (little endian)*/
                locVar = value[i].w;
                *(*txdata_pointer++ ) = HIBYTE(locVar);
                *(*txdata_pointer++ ) = LOBYTE(locVar);
                i++;
         }

         if(!(descr_mappe[indice].write_funct())) return(ERR_NSUP);

         /* sistema il buffer */
         buffer_count=4;
         txdata_count=0;
         break;
      }

/* comando positivo se sei arrivato fino a qui */
return(ERR_NONE);
}


/*---------------------------------------------------------------------------+
|
| NOME
|   void vModBus_AnsOK (INT8U slave, INT8U cmd)
|
| DESCRIZIONE
|   Completa ed avvia in trasmissione un pacchetto di risposta positiva.
|
| NOTE
|
+---------------------------------------------------------------------------*/
static void vModBus_AnsOK(INT8U slave, INT8U cmd)
{
   /*-----------------------*/
   /* testata del pacchetto */
   /*-----------------------*/
   rx_buffer.packet.slave=slave;
   rx_buffer.packet.cmd=cmd;

   /*-------------------------------------------------------------------------*/
   /* il campo dati deve essere gia' a posto (comprese le lunghezze relative) */
   /*-------------------------------------------------------------------------*/

   /*--------------------------------------------*/
   /* init calcolo uiModBus_Crc16 e invio primo carattere */
   /*--------------------------------------------*/
   crc_buffer.w=uiModBus_Crc16(0xffff, rx_buffer.packet.slave);
   tx_state=TX_CMD;
   TXINT_ENABLE;
   UART_DIR485_TX;
//   AS1_SendChar(rx_buffer.packet.slave);
   ucTxBuff = rx_buffer.packet.slave;
   HAL_UART_Transmit_IT(mdb_huart,&ucTxBuff, 1);
   
}


/*---------------------------------------------------------------------------+
|
| NOME
|   void vModBus_Error(INT8U slave, INT8U error, INT8U cmd)
|
| DESCRIZIONE
|   Costruisce e avvia un pacchetto di errore, da trasmettere
|   per il tipo di errore indicato dal secondo parametro.
|
| NOTE
|
+---------------------------------------------------------------------------*/
static void vModBus_Error(INT8U slave, INT8U error, INT8U cmd)
{
   /*-----------------------*/
   /* testata del pacchetto */
   /*-----------------------*/
   rx_buffer.packet.slave=slave;
   rx_buffer.packet.cmd=cmd + 128;

   /*------------*/
   /* campo dati */
   /*------------*/
   txdata_count=0;
   buffer_count=1;
   rx_buffer.packet.data[0]=error;

   /*--------------------------------------------*/
   /* init calcolo uiModBus_Crc16 e invio primo carattere */
   /*--------------------------------------------*/
   crc_buffer.w=uiModBus_Crc16(0xffff, rx_buffer.packet.slave);
   tx_state=TX_CMD;
   TXINT_ENABLE;
   UART_DIR485_TX;
 //  AS1_SendChar(rx_buffer.packet.slave);
   ucTxBuff=rx_buffer.packet.slave;
   HAL_UART_Transmit_IT(mdb_huart,&ucTxBuff, 1);

}

/*---------------------------------------------------------------------------+
|
| NOME
|   void vModBusCheckCommunication(void)
|
| DESCRIZIONE
|   la funzione inizializza il protocollo modbus quando non c'� comunicazione
|   con questo nodo.
|
| NOTE
|
+---------------------------------------------------------------------------*/
void vModBusCheckCommunication(void) {

    if (ucTimeOutBeforeReset) {
        ucTimeOutBeforeReset--;
    }
    else {
       uSTAT0.B.s15ModBusON = 0;
       vModBusInit(mdb_huart);
       ucTimeOutBeforeReset = TIMOUT_RELOAD;   // timeout reload
    }
}

/*---------------------------------------------------------------------------+
|
| NOME
|   INT16U uiModBus_Crc16(INT16U crc, INT8U nuovo)
|
| DESCRIZIONE
|   Accumulo del CRC-16 calcolato su INT8U, mediante chiamate consecutive.
|   In base alla definizione di calcolo del valore CRC-16, la prima chiamata
|   per una nuova sequenza di INT8U deve passare il valore FFFFh quale primo
|   parametro.
|
|   Accetta quali parametri il valore di partenza per il calcolo (accumulo del
|   CRC-16 su piu' INT8U) ed il nuovo INT8U da accumulare; restituisce il nuovo
|   valore di CRC-16.
|
| NOTE
|   ATTENZIONE: bisogna controllare bene la sintassi dei calcoli effettuati in
|               questa funzione, in particolare lo shift a destra ed il
|               recupero del carry!!!
|
+---------------------------------------------------------------------------*/
static INT16U uiModBus_Crc16(INT16U crc, INT8U newchar)
{
INT16U i;

   crc ^= newchar;
   for (i = 0; i < 8; i++)
   {
      /* carry? */
      if (crc & 0x0001)
      {
         crc >>= 1;
         crc ^= 0xa001;
      } 
      else 
      {
         crc >>= 1;
      }
   }

return(crc);
}

// -----------------------------------------------------------------------------------------------------------------
// Call back interrupt functions
// -----------------------------------------------------------------------------------------------------------------
// RX Interrupt
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	vModBusRxInt();
	HAL_UART_Receive_IT(huart, &ucRxBuff, 1);
}

// TX Interrupt
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	vModBusTxInt();
}

// ERR Interrupt
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
#define MAX_MODBUSHWERR		32
	if (stDataINT.D07ModBusHwErr++>MAX_MODBUSHWERR) {
		stDataINT.D07ModBusHwErr=0;
	}

}

