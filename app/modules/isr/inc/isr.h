/*=============================================================================
Copyright Zepco Technologies Pvt. Ltd. All Rights Reserved.
All trademarks are owned by Zepco Technologies Pvt. Ltd.
============================================================================ */

/*==============================================================================
 @file  isr.h
 @author Dhiman Roy
 @date Oct 27, 2023

 @brief Description
==============================================================================*/

#ifndef MODULES_ISR_INC_ISR_H_
#define MODULES_ISR_INC_ISR_H_

/*==============================================================================
 Includes
==============================================================================*/

/*==============================================================================
 Defines
==============================================================================*/
#define MAXPOSL (unsigned int)(uiEncOVPLVal10x * 0.90)  //Vref MAX
#define MINPOSL 0
#define MAXPOSR (unsigned int)(uiEncOCPLVal10x * 0.92)  //IRef MAX
#define MINPOSR 0
#define COARSE_STEP  5U
#define FINE_STEP    1U
#define COARSE_STEP_R  5U
#define FINE_STEP_R  1U
#define MAXOCP 1100U             //OCP MAX
#define MAXOVP 350U            //OVP MAX
#define MINOCP 0U
#define MINOVP 0U
/*==============================================================================
 Enums
==============================================================================*/

/*==============================================================================
 Structures
==============================================================================*/

/*==============================================================================
 Macros
==============================================================================*/

/*==============================================================================
 Extern/Public Function Prototypes
==============================================================================*/

extern __interrupt void Timer0_B0_ISR (void);
extern __interrupt void USCI_A0_ISR(void);
extern __interrupt void PORT2_VECTOR_ISR (void);


/*==============================================================================
 Extern/Public Variables
==============================================================================*/

extern unsigned int cr_received;
extern unsigned int uiProcRxPacket;
extern unsigned int rxbuffer_length;
extern unsigned int uiTxPacketIndex;
extern unsigned int uiTxWait;


extern unsigned int uiLCoarseValint;
extern unsigned int uiLCoarseValdec;
extern unsigned int uiRCoarseValint;
extern unsigned int uiRCoarseValdec;

extern unsigned int uiEncoderLVal100x;
extern unsigned int uiEncoderRVal100x;



extern unsigned int uiEncOVPLVal10x;
extern unsigned int uiLOVPCoarseVal_int;
extern unsigned int uiLOVPCoarseVal_dec;

extern unsigned int uiEncOCPLVal10x;
extern unsigned int uiROCPCoarseVal_int;
extern unsigned int uiROCPCoarseVal_dec;

extern unsigned int uiEncoderLVal1_1pc;
extern unsigned int uiEncoderRVal1_1pc;
/*==============================================================================
 Extern/Public Constants
==============================================================================*/


#endif /* MODULES_ISR_INC_ISR_H_ */
