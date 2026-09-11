/*=============================================================================
Copyright Zepco Technologies Pvt. Ltd. All Rights Reserved.
All trademarks are owned by Zepco Technologies Pvt. Ltd.
============================================================================ */

/*==============================================================================
 @file  buttons.h
 @author Dhiman, Santhosh N.
 @date Oct 27, 2023

 @brief Description
==============================================================================*/

#ifndef MODULES_BUTTONS_INC_BUTTONS_H_
#define MODULES_BUTTONS_INC_BUTTONS_H_

/*==============================================================================
 Includes
==============================================================================*/

/*==============================================================================
 Defines
==============================================================================*/

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

extern void Tactile_fnButtonPress();
extern void Button_fnLeftEncoder();
extern void Button_fnRightEncoder();
/*==============================================================================
 Extern/Public Variables
==============================================================================*/
extern unsigned int uiSaveDataPending;
extern unsigned int uiEditMode;
extern unsigned int uiPageNo;
extern unsigned int uiScrollBlankDelay;
extern unsigned int uiLastPage;
extern unsigned int left_mode;
extern unsigned int right_mode;
extern unsigned int uiCoarseVal;
extern unsigned int uiLeftModeChangeDelay;
extern unsigned int uiRightModeChangeDelay;
extern unsigned int uiResetRemoteProtect;
extern unsigned int uiOnOffSwitch;
extern unsigned int uiCoarseFinetoggle;   //Coarse/fine mode for right enc
extern unsigned int uiposition;
extern unsigned int uiKey5DebCnt;

/*==============================================================================
 Extern/Public Constants
==============================================================================*/


#endif /* MODULES_BUTTONS_INC_BUTTONS_H_ */
