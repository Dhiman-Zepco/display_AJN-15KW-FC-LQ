/*=============================================================================
Copyright Zepco Technologies Pvt. Ltd. All Rights Reserved.
All trademarks are owned by Zepco Technologies Pvt. Ltd.
============================================================================ */

/*==============================================================================
 @file  encoder.h
 @author Lenovo   
 @date Oct 27, 2023

 @brief Description
==============================================================================*/

#ifndef MODULES_ENCODER_INC_ENCODER_H_
#define MODULES_ENCODER_INC_ENCODER_H_

/*==============================================================================
 Includes
==============================================================================*/



/*==============================================================================
 Defines
==============================================================================*/
#define MAX_POS1 527
#define MAX_POS2 1000

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
extern void OLED_fnLeftEncoder(void);
extern void OLED_fnRightEncoder(void);


/*==============================================================================
 Extern/Public Variables
==============================================================================*/
extern unsigned int coarsePos1;
extern unsigned int coarsePos2;
extern unsigned int finePos1;
extern unsigned int finePos2;
extern unsigned int state1;     //Left-Encoder state.
extern unsigned int state2;     //right encoder state.
extern unsigned int mode1;      //Left encoder coarse to fine mode.
extern unsigned int mode2 ;     //Right encoder coarse to fine mode.
extern int uiOVPset;            //OVP reference voltage.
extern int uiVrefSet;           //Vref voltage.
extern int uiIrefSet;           //Iref voltage.
extern unsigned int uiOVPsetValtx;
extern unsigned int uiOVPsetValrx;
extern unsigned int uiVrefScaledtx;
extern unsigned int uiIrefScaledtx;

/*==============================================================================
 Extern/Public Constants
==============================================================================*/


#endif /* MODULES_ENCODER_INC_ENCODER_H_ */
