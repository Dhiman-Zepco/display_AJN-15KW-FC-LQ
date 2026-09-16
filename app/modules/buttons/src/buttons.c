/*
 * Tactile_ButtonPress.c
 *
 *  Created on: 31-Jul-2023
 *      Author: Dhiman Roy, Santhosh N
 *
 */

#include <msp430.h>
#include <oled.h>
#include <state.h>
#include <encoder.h>
#include <isr.h>
#include <buttons.h>

unsigned int uiKey1DebCnt = 0;
unsigned int uiKey2DebCnt = 0;
unsigned int uiKey3DebCnt = 0;
unsigned int uiKey4DebCnt = 0;
unsigned int uiKey5DebCnt = 0;
unsigned int uiKey6DebCnt = 0;
unsigned int uitestcount = 0;

unsigned int uiKey1State = 0;
unsigned int uiKey2State = 0;
unsigned int uiKey3State = 0;
unsigned int uiKey4State = 0;
unsigned int uiKey5State = 0;
unsigned int uiKey6State = 0;

unsigned int uiSaveDataPending = 0U;
unsigned int uiEditMode = 0;
char nodestr[6] = { 0, 0, 0, 0, 0, 0 };
unsigned int uiScrollBlankDelay = 0U;
unsigned int uiResetRemoteProtect;
unsigned int uiOnOffSwitch = 0U;
unsigned int uiCoarseFinetoggle;   //Coarse-fine mode for encoders

unsigned int uiPageNo;
unsigned int uiLastPage = 2U;    //last page is 6th now.

unsigned int left_mode = 0;
unsigned int right_mode = 0;
unsigned int uiTempDebugfrac_l = 0U;
unsigned int uiTempDebugfrac_r = 0U;

unsigned int uiENCKeyLState = 0;
unsigned int uiENCKeyRState = 0;

unsigned int uiLeftModeChangeDelay = 0;
unsigned int uiRightModeChangeDelay = 0;

unsigned int uiposition = 1U;

unsigned int uiserver_ip_oct1 = 0U;
unsigned int uiserver_ip_oct2 = 0U;
unsigned int uiserver_ip_oct3 = 0U;
unsigned int uiserver_ip_oct4 = 0U;

unsigned int uiclient_ip_oct1 = 0U;
unsigned int uiclient_ip_oct2 = 0U;
unsigned int uiclient_ip_oct3 = 0U;
unsigned int uiclient_ip_oct4 = 0U;




void Tactile_fnButtonPress(void);
void OLED_fnPrintPage(unsigned int uiPgNo);
int OLED_fnIntToStr(int x, char str[], int d);
void Button_fnLeftEncoder(void);
void Button_fnRightEncoder(void);

void Tactile_fnButtonPress()
{
    ////////////////////////Button_1 //////// as per schematic.
    /*
     * Button 1 -->  Page-UP/count++
     * Increment/Up Button
     *(P6.0)
     */

    if (((P6IN & 0x01) == 0x00) && (uiKey2State == 0))
    {
        if (uiKey2DebCnt >= 5U) //5U --> ( 5 unsigned * no. of times the function/event being called. )
        {
            uiKey2State = 1;                         //uiKey3DebCnt = 0;

            uiScrollBlankDelay = 1000U;

            if (uiEditMode == 1U)
            {
                if (uiPageNo == 2U)
                {
                    if (uiEditVarCnt >= 15)
                    {
                        uiEditVarCnt = 1U;
                    }
                    else
                    {
                        uiEditVarCnt++;                //Node increments here...
                    }
                }
                else if (uiPageNo == 3U)
                {
                    if (uiposition == 1U)
                    {
                        if(uiserver_ip_oct1 >= 255)
                        {
                            uiserver_ip_oct1 = 0U;
                        }
                        else
                        {
                            uiserver_ip_oct1++;
                        }
                    }
                    else if (uiposition == 2U)
                    {
                        if(uiserver_ip_oct2 >= 255)
                        {
                            uiserver_ip_oct2 = 0U;
                        }
                        else
                        {
                            uiserver_ip_oct2++;
                        }
                    }
                    else if(uiposition == 3U)
                    {
                        if(uiserver_ip_oct3 >= 255)
                        {
                            uiserver_ip_oct3 = 0U;
                        }
                        else
                        {
                            uiserver_ip_oct3++;
                        }
                    }
                    else if(uiposition == 4U)
                    {
                        if(uiserver_ip_oct4 >= 255)
                        {
                            uiserver_ip_oct4 = 0U;
                        }
                        else
                        {
                            uiserver_ip_oct4++;
                        }
                    }
                }
                else if (uiPageNo == 4U)
                {
                    if (uiposition == 1U)
                    {
                        if(uiclient_ip_oct1 >= 255)
                        {
                            uiclient_ip_oct1 = 0U;
                        }
                        else
                        {
                            uiclient_ip_oct1++;
                        }
                    }
                    else if (uiposition == 2U)
                    {
                        if(uiclient_ip_oct2 >= 255)
                        {
                            uiclient_ip_oct2 = 0U;
                        }
                        else
                        {
                            uiclient_ip_oct2++;
                        }
                    }
                    else if(uiposition == 3U)
                    {
                        if(uiclient_ip_oct3 >= 255)
                        {
                            uiclient_ip_oct3 = 0U;
                        }
                        else
                        {
                            uiclient_ip_oct3++;
                        }
                    }
                    else if(uiposition == 4U)
                    {
                        if(uiclient_ip_oct4 >= 255)
                        {
                            uiclient_ip_oct4 = 0U;
                        }
                        else
                        {
                            uiclient_ip_oct4++;
                        }
                    }
                }
                else
                {

                }

            }
            else
            {
                uiPageNo++;                             //Page increment here...

                if (uiPageNo > uiLastPage)
                {
                    uiPageNo = 1U;
                }
            }
        }
        else
        {
            uiKey2DebCnt++;
        }
    }
    else
    {
        uiKey2State = 0;
        uiKey2DebCnt = 0;
    }

/*Button_2 //////// as per schematic.
 * ( Decrement/Down Button - 2nd button)--->(P6.1)
 */

    if (((P6IN & 0x02) == 0x00) && (uiKey3State == 0))
    {
        if (uiKey3DebCnt >= 5U) //5U --> ( 5 unsigned * no. of times the function/event being called. )
        {
            uiKey3State = 1;

            uiScrollBlankDelay = 1000U;             //3-Seconds Delay.

            if (uiEditMode == 1U)
            {
                if (uiPageNo == 2U)
                {
                    if (uiEditVarCnt > 0)
                    {
                        uiEditVarCnt--;              //NodeID decrements here...
                    }
                    else
                    {
                        uiEditVarCnt = 15;
                    }
                }
                else if (uiPageNo == 3U)
                {
                    if (uiposition == 1U)
                    {
                        if(uiserver_ip_oct1 > 0U)
                        {
                            uiserver_ip_oct1--;
                        }
                        else
                        {
                            uiserver_ip_oct1 = 255U;
                        }
                    }
                    else if (uiposition == 2U)
                    {
                        if(uiserver_ip_oct2 > 0U)
                        {
                            uiserver_ip_oct2--;
                        }
                        else
                        {
                            uiserver_ip_oct2 = 255U;
                        }
                    }
                    else if(uiposition == 3U)
                    {
                        if(uiserver_ip_oct3 > 0U)
                        {
                            uiserver_ip_oct3--;
                        }
                        else
                        {
                            uiserver_ip_oct3 = 255U;
                        }
                    }
                    else if(uiposition == 4U)
                    {
                        if(uiserver_ip_oct4 > 0U)
                        {
                            uiserver_ip_oct4--;
                        }
                        else
                        {
                            uiserver_ip_oct4 = 255U;
                        }
                    }
                }
                else if (uiPageNo == 4U)
                {
                    if (uiposition == 1U)
                    {
                        if(uiclient_ip_oct1 > 0U)
                        {
                            uiclient_ip_oct1--;
                        }
                        else
                        {
                            uiclient_ip_oct1 = 255U;
                        }
                    }
                    else if (uiposition == 2U)
                    {
                        if(uiclient_ip_oct2 > 0U)
                        {
                            uiclient_ip_oct2--;
                        }
                        else
                        {
                            uiclient_ip_oct2 = 255U;
                        }
                    }
                    else if(uiposition == 3U)
                    {
                        if(uiclient_ip_oct3 > 0U)
                        {
                            uiclient_ip_oct3--;
                        }
                        else
                        {
                            uiclient_ip_oct3 = 255U;
                        }
                    }
                    else if(uiposition == 4U)
                    {
                        if(uiclient_ip_oct4 > 0U)
                        {
                            uiclient_ip_oct4--;
                        }
                        else
                        {
                            uiclient_ip_oct4 = 255U;
                        }
                    }
                }
            }
            else
            {
                if (uiPageNo > 0)
                {
                    uiPageNo--;                       //Page decrements here...
                }
                else
                {
                    uiPageNo = uiLastPage;
                }
            }
        }
        else
        {
            uiKey3DebCnt++;
        }
    }
    else
    {
        uiKey3State = 0;
        uiKey3DebCnt = 0;
    }

   /* Button 3 as per schematic --> P6.2
    * (NodeID/IP_Configuration/Edit button.
    *
    */
    if ((P6IN & 0x04) == 0x00)
    {
        if ((uiKey4DebCnt >= 30U) && (uiPageNo == 2U)) //50U --> ( 50 unsigned * no. of times the function/event being called. )
        {
            uiKey4DebCnt = 0;

            if (uiEditMode == 0)
            {
                uiEditVarCnt = DisplayDataRx.uiNodeId;
                uiEditMode = 1U;
            }
            else
            {
                DisplayDataTx.uiNodeId = uiEditVarCnt;
                uiEditMode = 0U;
                uiSaveDataPending = 1U;
            }
        }
        else if((uiKey4DebCnt >= 30U) && (uiPageNo == 3U))
        {
            uiKey4DebCnt = 0;

            if (uiEditMode == 0)
            {
                uiserver_ip_oct1 = DisplayDataRx.uirxservr_ip_oct1;
                uiserver_ip_oct2 = DisplayDataRx.uirxservr_ip_oct2;
                uiserver_ip_oct3 = DisplayDataRx.uirxservr_ip_oct3;
                uiserver_ip_oct4 = DisplayDataRx.uirxservr_ip_oct4;

                uiEditMode = 1U;
            }
            else if(uiEditMode == 1)
            {
                DisplayDataTx.ui_tx_servr_ip_oct1 = uiserver_ip_oct1;
                DisplayDataTx.ui_tx_servr_ip_oct2 = uiserver_ip_oct2;
                DisplayDataTx.ui_tx_servr_ip_oct3 = uiserver_ip_oct3;
                DisplayDataTx.ui_tx_servr_ip_oct4 = uiserver_ip_oct4;

                // Always mirror client side too, so TX packet is never zero on that side
                DisplayDataTx.ui_tx_client_ip_oct1 = uiclient_ip_oct1;
                DisplayDataTx.ui_tx_client_ip_oct2 = uiclient_ip_oct2;
                DisplayDataTx.ui_tx_client_ip_oct3 = uiclient_ip_oct3;
                DisplayDataTx.ui_tx_client_ip_oct4 = uiclient_ip_oct4;

                uiEditMode = 0U;
                uiSaveDataPending = 1U;
            }
            else
            {

            }
        }
        else if((uiKey4DebCnt >= 30U) && (uiPageNo == 4U))
        {
            uiKey4DebCnt = 0;

            if(uiEditMode == 0)
            {
                uiclient_ip_oct1 = DisplayDataRx.uirxclient_ip_oct1;
                uiclient_ip_oct2 = DisplayDataRx.uirxclient_ip_oct2;
                uiclient_ip_oct3 = DisplayDataRx.uirxclient_ip_oct3;
                uiclient_ip_oct4 = DisplayDataRx.uirxclient_ip_oct4;

                uiEditMode = 1U;
            }
            else if(uiEditMode == 1)
            {
                DisplayDataTx.ui_tx_client_ip_oct1 = uiclient_ip_oct1;
                DisplayDataTx.ui_tx_client_ip_oct2 = uiclient_ip_oct2;
                DisplayDataTx.ui_tx_client_ip_oct3 = uiclient_ip_oct3;
                DisplayDataTx.ui_tx_client_ip_oct4 = uiclient_ip_oct4;

                // Always mirror server side too
                DisplayDataTx.ui_tx_servr_ip_oct1 = uiserver_ip_oct1;
                DisplayDataTx.ui_tx_servr_ip_oct2 = uiserver_ip_oct2;
                DisplayDataTx.ui_tx_servr_ip_oct3 = uiserver_ip_oct3;
                DisplayDataTx.ui_tx_servr_ip_oct4 = uiserver_ip_oct4;

                uiEditMode = 0U;
                uiSaveDataPending = 1U;
            }
            else
            {

            }
        }
        else
        {
            uiKey4DebCnt++;
        }
    }
    else
    {
        uiKey4DebCnt = 0;
    }

    /***
     * Button - 4 (as per schematic...) -->(P6.3)
     *
     * Position switch button.
     * Default IP trigger switch.
     */

    if ((P6IN & 0x08) == 0x00)
    {
        if (uiKey5State == 0U)
        {
            if((uiPageNo == 3U) || (uiPageNo == 4U))
            {
                if ((uiKey5DebCnt >= 3U) && (uiEditMode == 1U))
                {
                    uiKey5DebCnt = 0U;
                    uiKey5State = 1U;

                    if (uiposition >= 4)
                    {
                        uiposition = 1U;
                    }
                    else
                    {
                        uiposition++;           //position toggling for values..
                    }
                }
                else
                {
                    uiKey5DebCnt++;
                }
            }
            else if((uiPageNo == 5U))
            {
                if ((uiKey5DebCnt >= 120U)) //~ near to 5 secs delay
                {
                    uiKey5DebCnt = 0U;
                    uiKey5State = 1U;  //Key pressed...

                    /*
                     * Copying of rx server ip address to tx server ip adress..
                     */

                    uiserver_ip_oct1 = DisplayDataRx.uirxservr_ip_oct1;
                    uiserver_ip_oct2 = DisplayDataRx.uirxservr_ip_oct2;
                    uiserver_ip_oct3 = DisplayDataRx.uirxservr_ip_oct3;
                    uiserver_ip_oct4 = DisplayDataRx.uirxservr_ip_oct4;

                    DisplayDataTx.ui_tx_servr_ip_oct1 = uiserver_ip_oct1 ;
                    DisplayDataTx.ui_tx_servr_ip_oct2 = uiserver_ip_oct2 ;
                    DisplayDataTx.ui_tx_servr_ip_oct3 = uiserver_ip_oct3 ;
                    DisplayDataTx.ui_tx_servr_ip_oct4 = uiserver_ip_oct4 ;

                    /*
                     * Copying of rx client ip address to tx client ip adress..
                     */

                    uiclient_ip_oct1 = DisplayDataRx.uirxclient_ip_oct1;
                    uiclient_ip_oct2 = DisplayDataRx.uirxclient_ip_oct2;
                    uiclient_ip_oct3 = DisplayDataRx.uirxclient_ip_oct3;
                    uiclient_ip_oct4 = DisplayDataRx.uirxclient_ip_oct4;

                    DisplayDataTx.ui_tx_client_ip_oct1 = uiclient_ip_oct1;
                    DisplayDataTx.ui_tx_client_ip_oct2 = uiclient_ip_oct2;
                    DisplayDataTx.ui_tx_client_ip_oct3 = uiclient_ip_oct3;
                    DisplayDataTx.ui_tx_client_ip_oct4 = uiclient_ip_oct4;

                    DisplayDataTx.ui_IP_reset_flag = 1U;  //set ip_reset flag

                    uiSaveDataPending = 1U;  //Trigger TX buffer...
                }
                else
                {
                    uiKey5DebCnt++;
                }


            }
        }
        else
        {

        }
    }
    else
    {
        uiKey5DebCnt = 0U;
        uiKey5State = 0U;
    }

/////////////////////////////////////////////////////////////////////////////////////////
//    /*
//     *
//     * On/Off Button pulse On/Off
//     *
//     */
//    //Button_6  ... as per schematic...185000000
//    //(P6.5)
//    if ((P6IN & 0x20) == 0x00)
//    {
//        if(uiKey6State == 0U)
//        {
//            if ((uiKey6DebCnt >= 3U) && (DisplayDataRx.uiPulseState == 2U))        //3U --> ( 3 unsigned * no. of times the function/event being called. )
//            {
//                uiKey6DebCnt = 0;
//                uiKey6State = 1U;   ///Key pressed.
//                DisplayDataTx.uiOnOffSwitch = 1U;
//
//            }
//            else if((uiKey6DebCnt >= 15U) && (DisplayDataRx.uiPulseState != 2U))   //15U --> ( 15 unsigned * no. of times the function/event being called. )
//            {
//                uiKey6DebCnt = 0;
//                uiKey6State = 1U;   ///Key pressed.
//                DisplayDataTx.uiOnOffSwitch = 1U;
//            }
//            else
//            {
//                uiKey6DebCnt++;
//            }
//        }
//        else
//        {
//            //do nothing
//        }
//    }
//    else
//    {
//        uiKey6DebCnt = 0U;
//        uiKey6State = 0U;
//
//    }

//    /**
//     * Remote Reset
//     */
//    if ((P6IN & 0x20) == 0x00)
//    {
//        if(uiKey6DebCnt >= 60U) //50U --> ( 50 unsigned * no. of times the function/event being called. )
//        {
//            DisplayDataTx.uiRemoteProtect = 1U;
//
//            uiKey6DebCnt = 0;
//
//        }
//        else
//        {
//            uiKey6DebCnt++;
//        }
//    }
//    else
//    {
//        DisplayDataTx.uiRemoteProtect = 0U;
//        uiKey6DebCnt = 0;
//    }

    /*
     *
     * BUTTON LED STATES...
     *
     */

    /*
     *
     * BUTTON 1 --> LED 1
     *
     */

//    if ((uiCoarseFinetoggle == 1U))
//    {
//        P4OUT |= 0x08;                            //LED 1 ON
//    }
//    else
//    {
//        P4OUT &= ~0x08;                         //LED 1 OFF
//    }
    /*
     *
     * BUTTON 2,3 --> LED 2,3
     *
     */

    if (uiEditMode == 1)
    {
        P4OUT |= (0x06);                //LED 2  ON // remote protect enable
    }
    else
    {
        P4OUT &= ~(0x06);                //LED 2 OFF // remote protect enable
    }

    /*
     *
     * BUTTON 4 --> LED 4
     *
     */

    if ((uiToggleState == 0) && (uiEditMode == 1))
    {
        P4OUT |= 0x01;                              //Add , Minus, Button LED
    }
    else
    {
        P4OUT &= ~(0x01);                             //Add , Minus, Button LED
    }

    /*
     *
     * BUTTON 5 --> LED 5
     * MENU / NodeID edit
     */

//    if ((uiEditMode == 1))
//    {
//        P2OUT |= 0x08;                                //MENU  Button LED
//    }
//    else
//    {
//        P2OUT &= ~(0x08);                        //MENU / NodeID edit Button LED
//    }
//
    /*
     * BUTTON 6 --> LED 6
     * Power ON/Off OUTPUT LED
     */

//    if (DisplayDataRx.uiPulseState == 2U) //P2.2 /// 0 - init, 1 - idle, 2 - live , 3 - fault...
//    {
//        P2OUT |= 0x04;                           //Power Button LED 6 turned ON.
//    }
//    else
//    {
//        P2OUT &= ~(0x04);                       //Power Button LED 6 turned OFF.
//    }
}

void Button_fnLeftEncoder()
{

    if (((P4IN & 0x80) == 0x00) && (uiENCKeyLState == 0U)) //V Fine Button... ENC_button .
    {
        if ((uiLeftModeChangeDelay >= 4U))
        {
            uiENCKeyLState = 1U;
            uiPageNo = 4U;
        }
        else
        {
            uiLeftModeChangeDelay++;
        }

    }
    else
    {
        uiENCKeyLState = 0U;
        uiLeftModeChangeDelay = 0U;
    }
}

void Button_fnRightEncoder()
{

    if (((P4IN & 0x10) == 0x00) && (uiENCKeyRState == 0U)) //I Fine Button... ENC_button .
    {
        if (uiRightModeChangeDelay >= 3U)
        {
            uiENCKeyRState = 1U;
            uiPageNo = 6U;
        }
        else
        {
            uiRightModeChangeDelay++;
        }

    }
    else
    {
        uiENCKeyRState = 0U;
        uiRightModeChangeDelay = 0U;
    }
}
