/*=============================================================================
Copyright Zepco Technologies Pvt. Ltd. All Rights Reserved.
All trademarks are owned by Zepco Technologies Pvt. Ltd.
============================================================================ */

/*==============================================================================
 @file  isr.c
 @author Nilesh B, Santhosh N, Dhiman Roy
 @date Oct 27, 2023

 @brief Description
==============================================================================*/

/*==============================================================================
 Includes
==============================================================================*/

#include <msp430.h>
#include "isr.h"
#include "rs232.h"
#include "state.h"
#include "oled.h"
#include "buttons.h"
#include "encoder.h"

/*==============================================================================
 Defines
==============================================================================*/


/*==============================================================================
 Enums
==============================================================================*/

/*==============================================================================
 Structures
==============================================================================*/
OLED_tzDISPLAY_DATA DisplayDataTx;
/*==============================================================================
 Macros
==============================================================================*/

/*==============================================================================
 Local Function Prototypes
==============================================================================*/

/*==============================================================================
 Local Variables
==============================================================================*/

unsigned int uiProcRxPacket = 0U;
unsigned int cr_received = 0;
unsigned int rxbuffer_length = 0;
unsigned int uiTxPacketIndex = 0;
unsigned int ui10msTicks = 0;
unsigned int uiOLEDUpdateTicks = 0;
unsigned int uiTxWait = 0U;

unsigned int uiLCoarseValint;
unsigned int uiLCoarseValdec;
unsigned int uiRCoarseValint;
unsigned int uiRCoarseValdec;

unsigned int uiEncoderLVal100x;
unsigned int uiEncoderLVal1_1pc;
unsigned int uiEncoderRVal100x;
unsigned int uiEncoderRVal1_1pc;



unsigned int uiEncOVPLVal10x = 0;
unsigned int uiLOVPCoarseVal_int = 0;
unsigned int uiLOVPCoarseVal_dec = 0;

unsigned int uiEncOCPLVal10x = 0;
unsigned int uiROCPCoarseVal_int = 0;
unsigned int uiROCPCoarseVal_dec = 0;


/*==============================================================================
 Local Constants

 VREF setting here...
==============================================================================*/

// External Port Pin interrupt service routine for left Encoder.

#pragma vector=PORT2_VECTOR
__interrupt void PORT2_VECTOR_ISR (void)
{
    if(((P2IN & 0x20) == 0x00) && (uiPageNo == 4) && (uiResetRemoteProtect == 0U))   //Left_ENC Rotating clockwise...
    {
        if(uiCoarseFinetoggle == 0)  //checking if its in coarse mode...
        {
            if(uiEncoderLVal100x < MAXPOSL)
            {
                uiEncoderLVal100x += COARSE_STEP;  //coarse increment...
            }
            else if (uiEncoderLVal100x >= MAXPOSL)
            {
                uiEncoderLVal100x = MAXPOSL; //Bounding to 1500U
            }
            else
            {

            }

        }
        else if(uiCoarseFinetoggle == 1)  //checking if its in fine mode... // 1
        {
            if(uiEncoderLVal100x < MAXPOSL)
            {
                uiEncoderLVal100x += FINE_STEP;  //fine increment...
            }
            else if(uiEncoderLVal100x >= MAXPOSL)
            {
                uiEncoderLVal100x = MAXPOSL; //Limiting to 1500U.
            }
            else
            {

            }
        }

        uiLCoarseValint = (unsigned int)(uiEncoderLVal100x / 10);    //left enc int val...   //Vref int
        uiLCoarseValdec = (unsigned int)(uiEncoderLVal100x % 10);     //left enc dec val...  //Vref dec

        uiEncoderLVal1_1pc = (unsigned int)(uiEncoderLVal100x * 1.1);

    }
    else if((uiPageNo == 4) && (uiResetRemoteProtect == 0U))    // Rotating on left to decrease here.
    {
        if(uiCoarseFinetoggle == 0)  //checking if its in coarse mode...
        {
            if(uiEncoderLVal100x >= COARSE_STEP)
            {
                uiEncoderLVal100x -= COARSE_STEP;   //coarse decrement...
            }
            else if (uiEncoderLVal100x <= COARSE_STEP)
            {
                uiEncoderLVal100x = MINPOSL;
            }
            else
            {

            }
        }
        else if(uiCoarseFinetoggle == 1) //checking if its in fine mode...
        {
            if(uiEncoderLVal100x >= FINE_STEP)
            {
                uiEncoderLVal100x -= FINE_STEP;   //fine decrement...
            }
            else if (uiEncoderLVal100x <= FINE_STEP)
            {
                uiEncoderLVal100x = MINPOSL;
            }
            else
            {

            }
        }

        uiLCoarseValint = (unsigned int)(uiEncoderLVal100x / 10);    //left enc int val...
        uiLCoarseValdec = (unsigned int)(uiEncoderLVal100x % 10);     //left enc dec val...

        uiEncoderLVal1_1pc = (unsigned int)(uiEncoderLVal100x * 1.1);

    }
    else
    {

    }


/////////////////////////////////////////////////////////////////////////////
/*
 *
 * OVP Left Encoder logic here...
 *
 */
////////////////////////////////////////////////////////////////////////////
    if(((P2IN & 0x20) == 0x00) && (uiPageNo == 3) && (uiResetRemoteProtect == 0U) && (DisplayDataRx.uiPulseState != 2))   //OVP reference change by ENC_L in page 4
    {
        if(uiCoarseFinetoggle == 0)  //checking if its in coarse mode...
        {
            if(uiEncOVPLVal10x < MAXOVP)
            {
                uiEncOVPLVal10x += COARSE_STEP;  //coarse increment...
            }
            else if (uiEncOVPLVal10x >= MAXOVP)
            {
                uiEncOVPLVal10x = MAXOVP; //Bounding to 1500U
            }
            else
            {

            }

        }
        else if(uiCoarseFinetoggle == 1)  //checking if its in fine mode... // 1
        {
            if(uiEncOVPLVal10x <= MAXOVP)
            {
                uiEncOVPLVal10x += FINE_STEP;  //fine increment...
            }
            else if(uiEncOVPLVal10x >= MAXOVP)
            {
                uiEncOVPLVal10x = MAXOVP; //Limiting to 1500U.
            }
            else
            {

            }
        }

        uiLOVPCoarseVal_int = (unsigned int)(uiEncOVPLVal10x / 10);    //left enc int val...OVP int
        uiLOVPCoarseVal_dec = (unsigned int)(uiEncOVPLVal10x % 10);     //left enc dec val... OVP dec

    }
    else if((uiPageNo == 3) && ((uiResetRemoteProtect == 0U)) && (DisplayDataRx.uiPulseState != 2))   // Rotating on left to decrease here.
    {
        if(uiCoarseFinetoggle == 0)  //checking if its in coarse mode...
        {
            if(uiEncOVPLVal10x >= COARSE_STEP)
            {
                uiEncOVPLVal10x -= COARSE_STEP;   //coarse decrement...

                if(uiEncOVPLVal10x <= uiEncoderLVal1_1pc)
                {
                    uiEncOVPLVal10x = uiEncoderLVal1_1pc;
                }
            }
            else if (uiEncOVPLVal10x < COARSE_STEP)
            {
                uiEncOVPLVal10x = MINOVP;
            }
            else
            {

            }
        }
        else if(uiCoarseFinetoggle == 1) //checking if its in fine mode...
        {
            if(uiEncOVPLVal10x >= FINE_STEP)
            {
                uiEncOVPLVal10x -= FINE_STEP;   //fine decrement...

                if(uiEncOVPLVal10x <= uiEncoderLVal1_1pc)
                {
                    uiEncOVPLVal10x = uiEncoderLVal1_1pc;
                }
            }
            else if (uiEncOVPLVal10x < FINE_STEP)
            {
                uiEncOVPLVal10x = MINOVP;
            }
            else
            {

            }
        }

        uiLOVPCoarseVal_int = (unsigned int)(uiEncOVPLVal10x / 10);    //left enc int val...
        uiLOVPCoarseVal_dec = (unsigned int)(uiEncOVPLVal10x % 10);     //left enc dec val...

    }

    P2IFG &= ~(0xFF);                         // Clear P2 IFG

}

/*
 *
 * Iref Encoder :-
 *
 */
// External Port Pin interrupt service routine for right Encoder.
#pragma vector=PORT4_VECTOR
__interrupt void PORT4_VECTOR_ISR (void)
{
    if (((P4IN & 0x40) == 0x00) && (uiPageNo == 4) && ((uiResetRemoteProtect == 0U)))       // Right ENC rotating clockwise...
    {
        if (uiCoarseFinetoggle == 0) //checking if its in coarse mode...
        {
            if(uiEncoderRVal100x < MAXPOSR)
            {
                uiEncoderRVal100x += COARSE_STEP_R;  //coarse increment...
            }
            else if(uiEncoderRVal100x >= MAXPOSR)
            {
                uiEncoderRVal100x = MAXPOSR;
            }
            else
            {

            }
        }
        else if(uiCoarseFinetoggle == 1) //checking if its in fine mode... // [1]
        {
            if(uiEncoderRVal100x < MAXPOSR)
            {
                uiEncoderRVal100x += FINE_STEP_R; //fine increment
            }
            else if(uiEncoderRVal100x >= MAXPOSR)
            {
                uiEncoderRVal100x = MAXPOSR; //Limiting to 700U.
            }
            else
            {

            }
        }
        else
        {

        }

        uiRCoarseValint = (unsigned int)(uiEncoderRVal100x / 10);   //right enc int val...
        uiRCoarseValdec = (unsigned int)(uiEncoderRVal100x % 10);     //right enc dec val...

        uiEncoderRVal1_1pc = (unsigned int)(uiEncoderRVal100x * 1.1);
    }
    else if ((uiPageNo == 4) && ((uiResetRemoteProtect == 0U)))    // Iref ENC --> Rotating on left to decrease here.
    {
        if (uiCoarseFinetoggle == 0) //checking if its in coarse mode...
        {
            if(uiEncoderRVal100x > COARSE_STEP_R)
            {
                uiEncoderRVal100x -= COARSE_STEP_R;  //coarse decrement...
            }
            else if(uiEncoderRVal100x <= COARSE_STEP_R)
            {
                uiEncoderRVal100x = MINPOSR;
            }
            else
            {

            }
        }
        else if(uiCoarseFinetoggle == 1) //checking if its in fine mode... // [1]
        {
            if(uiEncoderRVal100x > FINE_STEP_R)
            {
                uiEncoderRVal100x -= FINE_STEP_R; //fine increment
            }
            else if(uiEncoderRVal100x <= FINE_STEP_R)
            {
                uiEncoderRVal100x = MINPOSR; // resetting to Zero.
            }
            else
            {

            }
        }
        else
        {

        }


        uiRCoarseValint = (unsigned int)(uiEncoderRVal100x / 10);   //right enc int val...
        uiRCoarseValdec = (unsigned int)(uiEncoderRVal100x % 10);     //right enc dec val...

        uiEncoderRVal1_1pc = (unsigned int)(uiEncoderRVal100x * 1.1);

    }
    else  if (((P4IN & 0x40) == 0x00) && (uiPageNo == 3) && (uiResetRemoteProtect == 0U) && (DisplayDataRx.uiPulseState != 2))  ///// OCP increasing clockwise /////
    {
        if (uiCoarseFinetoggle == 0) //checking if its in coarse mode...
        {
            if(uiEncOCPLVal10x < MAXOCP)
            {
                uiEncOCPLVal10x += COARSE_STEP_R;  //coarse increment...
            }
            else if(uiEncOCPLVal10x >= MAXOCP)
            {
                uiEncOCPLVal10x = MAXOCP;
            }
            else
            {

            }
        }
        else if(uiCoarseFinetoggle == 1) //checking if its in fine mode... // [1]
        {
            if(uiEncOCPLVal10x < MAXOCP)
            {
                uiEncOCPLVal10x += FINE_STEP_R;   //fine increment
            }
            else if(uiEncOCPLVal10x >= MAXOCP)
            {
                uiEncOCPLVal10x = MAXOCP;        //Limiting to 700U.
            }
            else
            {

            }
        }
        else
        {

        }

        uiROCPCoarseVal_int = (unsigned int)(uiEncOCPLVal10x / 10);     //right enc int val...
        uiROCPCoarseVal_dec = (unsigned int)(uiEncOCPLVal10x % 10);     //right enc dec val...

    }
    else if ((uiPageNo == 3) && ((uiResetRemoteProtect == 0U)) && (DisplayDataRx.uiPulseState != 2))  //// Decreasing OCP here.. ////
    {
        if (uiCoarseFinetoggle == 0) //checking if its in coarse mode...
        {
            if(uiEncOCPLVal10x >= COARSE_STEP_R)
            {
                uiEncOCPLVal10x -= COARSE_STEP_R;  //coarse decrement...

                if(uiEncOCPLVal10x <= uiEncoderRVal1_1pc)
                {
                    uiEncOCPLVal10x = uiEncoderRVal1_1pc;
                }
            }
            else if(uiEncOCPLVal10x < COARSE_STEP_R)
            {
                uiEncOCPLVal10x = MINOCP;
            }
            else
            {

            }
        }
        else if(uiCoarseFinetoggle == 1) //checking if its in fine mode... // [1]
        {
            if(uiEncOCPLVal10x >= FINE_STEP_R)
            {
                uiEncOCPLVal10x -= FINE_STEP_R; //fine decrement

                if(uiEncOCPLVal10x <= uiEncoderRVal1_1pc)
                {
                    uiEncOCPLVal10x = uiEncoderRVal1_1pc;
                }
            }
            else if(uiEncOCPLVal10x < FINE_STEP_R)
            {
                uiEncOCPLVal10x = MINOCP;
            }
            else
            {

            }
        }
        else
        {

        }

        uiROCPCoarseVal_int = (unsigned int)(uiEncOCPLVal10x / 10);   //right enc int val...
        uiROCPCoarseVal_dec = (unsigned int)(uiEncOCPLVal10x % 10);     //right enc dec val...
    }

    /*
     * Interrupt Flag Clearing...
     */

    P4IFG &= ~(0xFF);                         // Clearing P4 IFG flag.
}

/*=============================================================================
 UART RS232 ISR- whenever any data is received or put in the UART0 RX buffer, it services this
 interrupt routine, by receiving it.
 @param void
 @return void
 ============================================================================ */
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{    switch (__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG))
    {
    case USCI_NONE:
        break;
    case USCI_UART_UCRXIFG:

        RS232_fnReceiveFSM(UCA0RXBUF);

        __no_operation();
        break;
    case USCI_UART_UCTXIFG:
        break;
    case USCI_UART_UCSTTIFG:
        break;
    case USCI_UART_UCTXCPTIFG:
        break;
    default:
        break;
    }
}

/*=============================================================================
 Timer ISR controlling toggling of encoder values and DisplayData in OLED Display and
 transmitting every 10ms the transmit buffer[15].

 @param void
 @return void
 ============================================================================ */

#pragma vector = TIMER0_B0_VECTOR
__interrupt void Timer0_B0_ISR (void)
{

    /**
     * Scheduler Ticks
     */
    ui10msTicks++;

    if (ui10msTicks >= 10)
    {
        ui10msTicks = 0;
        uiTrig10msEvt = 1;

    }
    else
    {

    }

    uiOLEDUpdateTicks++;

    if(uiOLEDUpdateTicks >= 500)
    {
        uiOLEDUpdateTicks = 0;
        uiTrigOLEDUpdateEvt = 1;
    }

    if(uiTxPacket == 1)
    {
        if(uiTxPacketLength == 0)
        {
            if(--uiTxWait == 0U)
            {
                uiTxPacketIndex = 0;
                uiTxPacket = 0;
            }
        }
        else
        {
            while (!(UCA0IFG&UCTXIFG));             // USCI_A0 TX buffer ready?

            UCA0TXBUF = uiTxBuff[uiTxPacketIndex++];
            uiTxPacketLength--;
//            __delay_cycles(100);

        }
    }
    else
    {

    }


}


/*==============================================================================
 End of File
==============================================================================*/
