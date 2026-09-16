/*
 * rs232.c
 *
 *  Created on: 11-Jul-2023
 *      Author: Nilesh, Santhosh .N, Dhiman Roy
 */
#include <msp430.h>
#include "oled.h"
#include "isr.h"
#include "rs232.h"
#include "buttons.h"
#include "encoder.h"

void RS232_fnReceiveFSM(unsigned int uiShowRxByte);

void RS232_fnProcessMessage();

unsigned int correct = 0;
unsigned int Fletcher16( unsigned int *data, int count );
unsigned int fletcher_calc = 0, data_valid = 0;
unsigned int fletcher_recvd = 0;
unsigned int Fault = 0;

unsigned int uiRxByteNow;
unsigned int uiCommError;
unsigned int rxIndex = 0;

unsigned int uiRxByteNow = 0;
unsigned int uiRxBytePrev = 0;
unsigned int uiRxCrcRecv = 0U;
unsigned int uiRxCrcCalc = 0U;
unsigned int uiRxCmdIndex = 0U;
unsigned int uiRxIndex = 0U;
unsigned int uiRxDataLen = 0U;
unsigned int uiRxCommand = 0U;
unsigned int uiRxBuff[40U] = {0};
unsigned int uiTxBuff[20U] = {0};
unsigned int uiTxPacketLength = 0;
unsigned int uiTxPacket = 0;
unsigned int uiTxCrcCalc = 0U;
int i = 0;
unsigned int uiErrCnt = 0U;

unsigned int uiActualTemp1int = 0U;
unsigned int uiActualTemp2int = 0U;
RS232_teRX_STATES teRxState = RS232_RX_IDLE;
RS232_teTX_STATES teTxState = RS232_TX_IDLE;

//RS232_teRX_FAULT_STATES teRxFltState = RS232_RX_OFF;
unsigned int uiRxFltState = 0;

unsigned int uiCnt = 0U;

unsigned int uiMasterDataUpdated = 0U;

unsigned int uiDoOnce = 1;

unsigned int uiDelayTxCnt = 0U;


/*=============================================================================
 @brief Type Function description here.

 Receiving Sequence finite state machine function by UART RS232.

 @param uiShowRxByte
 @return void
 ============================================================================ */

                        //Receive FSM goes here...

void RS232_fnReceiveFSM(unsigned int uiShowRxByte)
{

    uiRxByteNow = uiShowRxByte;


    switch (teRxState)
    {
    case RS232_RX_IDLE:                         //CASE 0

        if(uiRxByteNow == 0xA5)
        {
            if(uiRxBytePrev == 0xA5)
            {
                teRxState = RS232_RX_LEN;
                //Reset all variables
                uiRxCrcCalc = 0U;
                uiRxIndex = 0U;
                uiRxCmdIndex = 0U;
                uiRxDataLen = 0U;
                uiRxCommand = 0U;
            }
        }

        break;
    case RS232_RX_LEN:                          //CASE 1

        uiRxDataLen = uiRxByteNow;

        if(uiRxDataLen <= 16)           //check if Length is good???
        {

            uiRxBuff[uiRxIndex++] = uiRxByteNow;
            uiRxCrcCalc += uiRxByteNow;
            teRxState = RS232_RX_CMD;
        }
        else
        {
            teRxState = RS232_RX_IDLE;
        }

        break;

    case RS232_RX_CMD:                          //CASE 2

        uiRxBuff[uiRxIndex++] = uiRxByteNow;
        uiRxCrcCalc += uiRxByteNow;

        if(uiRxCmdIndex >= 1U)
        {
            uiRxCommand = (uiRxBytePrev << 8U) | (uiRxByteNow & 0x0F);
            teRxState = RS232_RX_RECV;
        }

        uiRxCmdIndex++;

        break;

    case RS232_RX_RECV:                         //CASE 3

        uiRxBuff[uiRxIndex++] = uiRxByteNow;
        uiRxCrcCalc += uiRxByteNow;

        if(uiRxIndex >= (uiRxDataLen))
        {
            teRxState = RS232_RX_CRC;
        }

        break;
    case RS232_RX_CRC:                          //CASE 4

        uiRxBuff[uiRxIndex++] = uiRxByteNow;
        uiRxCrcRecv = uiRxByteNow;

        if(uiRxCrcRecv == (uiRxCrcCalc & 0xFF))
        {
            teRxState = RS232_RX_WAIT;
            uiProcRxPacket =  1U;                   //if Complete buffer receive successful...
        }
        else
        {
            teRxState = RS232_RX_IDLE;
            //Error Count can be increased here...
        }

        break;
    case RS232_RX_WAIT:                         //CASE 5

        break;
    default:
        break;
    }

    uiRxBytePrev = uiRxByteNow;

}


void RS232_fnProcessMessage()
{
    unsigned int uiTempVal = 0U;

    if (uiProcRxPacket == 1)
    {

        if(uiErrCnt > 0)
        {
            uiErrCnt = 0;
            uiCommError = 0U;
        }
        else
        {

        }

        DisplayDataRx.FRAME_LEN = uiRxBuff[0];

        DisplayDataRx.CHML = uiRxBuff[1];

        DisplayDataRx.CMDH = uiRxBuff[2];

        ///////////////////////Vout display//////////////////////////////////////////

        uiTempVal = (uiRxBuff[3] << 8) | uiRxBuff[4];

        DisplayDataRx.uiVoutFrac = (unsigned int) (uiTempVal % 100);     //Vrms fractional part.

        DisplayDataRx.uiVoutInt = (unsigned int) (uiTempVal / 100);      //Vrms integer part.

        ///////////////////////Iout Display/////////////////////////////////////////////

        uiTempVal = (uiRxBuff[5] << 8) | uiRxBuff[6];

        DisplayDataRx.uiIoutFrac = (unsigned int) (uiTempVal % 100);    //Irms fractional part.

        DisplayDataRx.uiIoutInt = (unsigned int) (uiTempVal / 100);     //Irms integer part.

        /////////////Node-ID////////////////////////////////////////////

        DisplayDataRx.uiNodeId = uiRxBuff[7];

        //////////////////////Fault-Registers///////////////////////////////////

        tuFaultReg.all = ((uiRxBuff[8] << 8) | (uiRxBuff[9] & 0xFF));

        //////////////pulse_State///////////////////////////////////////////

        DisplayDataRx.uiPulseState = uiRxBuff[10];  /// 0 - init, 1 - idle, 2 - live , 3 - fault...

        ///////////////////// major version////////////////////////////////////

        uiTempVal = (uiRxBuff[11] & 0xFF);

        DisplayDataRx.uirxbuild_ver = (unsigned int) (uiTempVal);

        //////////////////// minor version////////////////////////////////////

        uiTempVal = (uiRxBuff[12] & 0xFF);

        DisplayDataRx.uirxbuild_ver_major = (unsigned int) (uiTempVal);

        //////////////////// build version////////////////////////////////////

        uiTempVal = (uiRxBuff[13] & 0xFF);

        DisplayDataRx.uirxbuild_ver_minor = (unsigned int) (uiTempVal);


//
//        //////////////////Vref Set//////////////////////////////////////
//
//        uiTempVal = (uiRxBuff[5] << 8) | uiRxBuff[6];
//
//        if((uiDoOnce == 1) || (uiResetRemoteProtect == 1))
//        {
//            uiEncoderLVal100x = uiTempVal / 10;
//        }
//
//        ////////////////////Iref Set//////////////////////////////////////////
//
//        uiTempVal = (uiRxBuff[7] << 8) | uiRxBuff[8];
//
//        if((uiDoOnce == 1) || (uiResetRemoteProtect == 1))
//        {
//            uiEncoderRVal100x = uiTempVal / 10;
//        }
//
//
//        ////////////////////////// OVP Value receive //////////////////////////////
//
//        uiTempVal = (uiRxBuff[9] << 8) | uiRxBuff[10];
//
//        if((uiDoOnce == 1) || (uiResetRemoteProtect == 1))
//        {
//            uiEncOVPLVal10x = uiTempVal /10;
//        }
//
//        ////////////////////////// OCP Value receive //////////////////////////////
//
//        uiTempVal = (uiRxBuff[11] << 8) | uiRxBuff[12];
//
//        if((uiDoOnce == 1) || (uiResetRemoteProtect == 1))
//        {
//            uiEncOCPLVal10x = uiTempVal / 10;
//        }
//
//        //////////////////R-Phase////////////////////////////////////////////////////////////////
//
//        uiTempVal = (uiRxBuff[17] << 8) | uiRxBuff[18];
//
//        DisplayDataRx.uiRPhVolts = (unsigned int) (uiTempVal / 100 );
//
//        //////////////////Y-Phase////////////////////////////////////////////////////////////////
//
//        uiTempVal = (uiRxBuff[19] << 8) | uiRxBuff[20];
//
//        DisplayDataRx.uiYPhVolts = (unsigned int) (uiTempVal / 100);
//
//        //////////////////B-Phase//////////////////////////////////////////////////////////////////
//
//        uiTempVal = (uiRxBuff[21] << 8) | uiRxBuff[22];
//
//        DisplayDataRx.uiBPhVolts = (unsigned int) (uiTempVal / 100);

        //        /*
//         * Server IP address receive here...
//         */
//
//        DisplayDataRx.uirxservr_ip_oct1 = uiRxBuff[28];   //server_ip_oct1
//
//
//        DisplayDataRx.uirxservr_ip_oct2 = uiRxBuff[29];   //server_ip_oct2
//
//
//        DisplayDataRx.uirxservr_ip_oct3 = uiRxBuff[30];   //server_ip_oct3
//
//
//        DisplayDataRx.uirxservr_ip_oct4 = uiRxBuff[31];   //server_ip_oct4
//
//
//        /*
//         * Client IP address receive here...
//         */
//
//        DisplayDataRx.uirxclient_ip_oct1 = uiRxBuff[32];  //client_ip_oct1
//
//
//        DisplayDataRx.uirxclient_ip_oct2 = uiRxBuff[33];  //client_ip_oct2
//
//
//        DisplayDataRx.uirxclient_ip_oct3 = uiRxBuff[34];  //client_ip_oct3
//
//
//        DisplayDataRx.uirxclient_ip_oct4 = uiRxBuff[35];  //client_ip_oct4
//
//
        DisplayDataRx.uiCrc = uiRxBuff[14];  // rx-crc
//
//        if((uiEditMode == 0U))
//        {
//            //to copy ip addresses to local variables once.
//
//            uiserver_ip_oct1 = DisplayDataRx.uirxservr_ip_oct1;
//            uiserver_ip_oct2 = DisplayDataRx.uirxservr_ip_oct2;
//            uiserver_ip_oct3 = DisplayDataRx.uirxservr_ip_oct3;
//            uiserver_ip_oct4 = DisplayDataRx.uirxservr_ip_oct4;
//
//
//            uiclient_ip_oct1 = DisplayDataRx.uirxclient_ip_oct1;
//            uiclient_ip_oct2 = DisplayDataRx.uirxclient_ip_oct2;
//            uiclient_ip_oct3 = DisplayDataRx.uirxclient_ip_oct3;
//            uiclient_ip_oct4 = DisplayDataRx.uirxclient_ip_oct4;
//
//        }
//        else
//        {
//
//        }

        uiProcRxPacket = 0;  //todo

        teRxState = RS232_RX_IDLE;

        /**
         * Transmit Packet
         */

//        if((uiDelayTxCnt >= 30U) && (uiSaveDataPending == 1U))  // uiDelayTxCnt == 30U --> 90 mili-secs
//        {
//            if ((uiTxPacket == 0U))                  //Tx -Packet Start.
//            {
//
//                uiTxCrcCalc = 0;
//
//                uiTxBuff[0] = 0xA5;                          //SOF1.
//                uiTxBuff[1] = 0xA5;                          //SOF2.
//
//                uiTxBuff[2] = 0x0D;                   //Length of the "data packet"-13U
//
//                uiTxBuff[3] = 0x00;                          //CMDH
//                uiTxBuff[4] = 0x00;                          //CMDL
//
//                uiTxBuff[5] = DisplayDataTx.uiNodeId; //Node_id Received value is being transmitted.
//
//                uiTxBuff[6] = DisplayDataTx.ui_tx_servr_ip_oct1;
//                uiTxBuff[7] = DisplayDataTx.ui_tx_servr_ip_oct2;
//
//                uiTxBuff[8] = DisplayDataTx.ui_tx_servr_ip_oct3;
//                uiTxBuff[9] = DisplayDataTx.ui_tx_servr_ip_oct4;
//
//                uiTxBuff[10] = DisplayDataTx.ui_tx_client_ip_oct1;
//                uiTxBuff[11] = DisplayDataTx.ui_tx_client_ip_oct2;
//
//                uiTxBuff[12] = DisplayDataTx.ui_tx_client_ip_oct3;
//                uiTxBuff[13] = DisplayDataTx.ui_tx_client_ip_oct4;
//
//                uiTxBuff[14] = DisplayDataTx.ui_IP_reset_flag;
//
//                for (i = 2; i <= 14; i++)
//                {
//                    uiTxCrcCalc += uiTxBuff[i];          //CRC calc...
//                }
//
//                uiTxBuff[15] = (uiTxCrcCalc & 0xFF);    //CRC sum value...
//
//                uiTxPacket = 1U;
//                uiTxPacketLength = 16U;
//                uiTxPacketIndex = 0;
//                uiTxWait = 5;
//
//                DisplayDataTx.ui_IP_reset_flag = 0U;  //reset ip_reset_trigger flag..
//                uiSaveDataPending = 0U;
//                uiDelayTxCnt = 0U;
//            }
//        }
//        else
//        {
//            uiDelayTxCnt++;
//        }
    }
    else
    {
        uiErrCnt++;

        if(uiErrCnt >= 256)
        {
//            uiCommError = 1U;
        }
    }
}


unsigned int Fletcher16( unsigned int *data, int count )
{
   unsigned int sum1 = 0;
   unsigned int sum2 = 0;
   int index;

   for ( index = 0; index < count; ++index )
   {
      sum1 = (sum1 + data[index]) % 255;
      sum2 = (sum2 + sum1) % 255;
   };

   return (sum2 << 8) | sum1;
}






