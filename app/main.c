/*=============================================================================
Copyright Zepco Technologies Pvt. Ltd. All Rights Reserved.
All trademarks are owned by Zepco Technologies Pvt. Ltd.
============================================================================ */

/*==============================================================================
 @file  main.c
 @author DhimanRoy   
 @date 27-Oct-2023

 @brief Description:

 It contains all the functions related to the front panel display for the
 Degauss display system.
==============================================================================*/

/*==============================================================================
 Includes
==============================================================================*/
#include <msp430.h>
#include "isr.h"
#include "init.h"
#include "oled.h"
#include "buttons.h"
#include "rs232.h"
#include "state.h"
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

/*==============================================================================
 Macros
==============================================================================*/

/*==============================================================================
 Local Variables
==============================================================================*/

/*==============================================================================
 Local Constants
==============================================================================*/

/*=============================================================================
 @brief It consists of all the functions related to the front panel display.
 system initialization, OLED initialization, oled welcome page.
 Next remaining functions trigger in a timed event as per timer ISR trigger in
 while loop.

 @param void
 @return int
 ============================================================================ */
int main(void)
{

    INIT_fnSystemInit();                            //system initialization...

    __delay_cycles(16000000);                       //1 second delay.

    OLED_fnInit();                          //OLED initialization configurations

    uiPageNo = 1U;

    OLED_fnWelcomePage();

    __delay_cycles(32000000);                            //2 seconds delay...

    while (1)
    {
        if (uiTrig10msEvt == 1)
        {
            uiTrig10msEvt = 0;    //resetting uiTrig10msEvt variable every 10ms.

            ui10msEvtIndex++;



            if (ui10msEvtIndex >= 3)  //switching each modules of the display.
            {
                ui10msEvtIndex = 0;
            }
            else
            {

            }

            if (ui10msEvtIndex == 0)
            {
                RS232_fnProcessMessage();  //After receiving, storage of data...
            }
            else if (ui10msEvtIndex == 1)
            {
                Tactile_fnButtonPress();   //Tactile Buttons press logics...
            }
            else if (ui10msEvtIndex == 2)
            {
                OLED_fnUpdateDisplay();
            }
        }
    }
}

/*==============================================================================
 End of File
==============================================================================*/
