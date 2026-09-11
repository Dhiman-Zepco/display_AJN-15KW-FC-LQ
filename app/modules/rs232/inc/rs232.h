/*=============================================================================
Copyright Zepco Technologies Pvt. Ltd. All Rights Reserved.
All trademarks are owned by Zepco Technologies Pvt. Ltd.
============================================================================ */

/*==============================================================================
 @file  rs232.h
 @author Lenovo   
 @date Oct 27, 2023

 @brief Description
==============================================================================*/

#ifndef MODULES_RS232_RS232_H_
#define MODULES_RS232_RS232_H_

/*==============================================================================
 Includes
==============================================================================*/

/*==============================================================================
 Defines
==============================================================================*/

/*==============================================================================
 Enums
==============================================================================*/

typedef enum RS232_eRX_STATES
{
    RS232_RX_IDLE,
    RS232_RX_LEN,
    RS232_RX_CMD,
    RS232_RX_RECV,
    RS232_RX_CRC,
    RS232_RX_WAIT
} RS232_teRX_STATES;

typedef enum RS232_eTX_STATES
{
    RS232_TX_IDLE,
    RS232_TX_SEND,
    RS232_TX_WAIT
} RS232_teTX_STATES;

typedef enum RS232_eRX_FAULT_STATES
{
    RS232_RX_OFF,
    RS232_RX_ON,
    RS232_RX_FAULT,

} RS232_teRX_FAULT_STATES;

/*==============================================================================
 Structures
==============================================================================*/

/*==============================================================================
 Macros
==============================================================================*/

#define SHOW_mSOF                       (0xA5)

/*==============================================================================
 Extern/Public Function Prototypes
==============================================================================*/

extern void RS232_fnProcessMessage(void);
extern void RS232_fnReceiveFSM(unsigned int uiShowRxByte);

/*==============================================================================
 Extern/Public Variables
==============================================================================*/

extern unsigned int uiRxBuff[40U];
extern unsigned int rxIndex;
extern unsigned int Fault;
extern unsigned int uiTxBuff[20U];
extern unsigned int uiTxPacketLength;
extern unsigned int uiTxPacket;
extern unsigned int uiCommError;
extern unsigned int uiActualTemp1int;
extern unsigned int uiActualTemp2int;


/*==============================================================================
 Extern/Public Constants
==============================================================================*/


#endif /* MODULES_RS232_RS232_H_ */
