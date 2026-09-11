/*
 * encoder.c
 *
 *  Created on: 30-May-2023
 *      Author: Nilesh,Rohan,Santhosh N,Dhiman.
 */


#include <msp430.h>
#include <encoder.h>
#include <oled.h>
#include "buttons.h"
#include <rs232.h>

void OLED_fnCommand(char i);
void OLED_fnData(char i);
void OLED_fnInit();
void OLED_fnStrDisp(char *str);
void OLED_fnLeftEncoder(void);
void OLED_fnRightEncoder(void);
void OLED_fnReverse(char *str, int len);
int  OLED_fnIntToStr(int x, char str[], int d);


char str00[4];

unsigned int state1 = 0U;     //Left-Encoder state.
unsigned int state2 = 0U;     //right encoder state.
unsigned int mode1 = 0;
unsigned int mode2 = 0;
unsigned int uiOVPsetValtx = 0;
unsigned int uiOVPsetValrx = 0;
unsigned int uiVrefScaledtx = 0;
unsigned int uiVrefScaledrx = 0;
unsigned int uiIrefScaledtx = 0;
unsigned int uiIrefScaledrx = 0;

int uiOVPset;
int uiVrefSet;
int uiIrefSet;


void OLED_fnLeftEncoder()                            //left encoder starts
{
    if (uiPageNo == 4U)
    {
        if((P2IN & 0x30) == 0x00)  //if pulse A,B --> 0000 0000
        {
            if(state1 == 2)
            {
                if(mode1 == 1)
                {
                    uiOVPset -= 2;
                }
                else
                {
                    uiOVPset -= 10;
                }
            }
            else if(state1 == 4)
            {
                 if(mode1 == 1)
                 {
                     uiOVPset += 2;
                 }
                 else
                 {
                     uiOVPset += 10;
                 }
            }
            else
            {
                // Do Nothing
            }
                                              ////          AB
            state1 = 1;                       // State 1: - 00

         }
         else if((P2IN & 0x30) == 0x20) // if pulse A,B --> 0010 0000
         {
            if(state1 == 3)
            {

                if(mode1 == 1)
                {
                    uiOVPset -= 2;
                }
                else
                {
                    uiOVPset -= 10;
                }
            }

            else if(state1 == 1)
            {

                if(mode1 == 1)
                {
                    uiOVPset += 2;
                }
                else
                {
                    uiOVPset += 10;
                }
            }
            else
            {
                // Do Nothing
            }
                                                ////          AB
            state1 = 2;                         // State 1--> 01
         }
         else if((P2IN & 0x30) == 0x30) // if pulse A,B --> 0011 0000
         {
             if(state1 == 4)
             {

                 if(mode1 == 1)
                 {
                     uiOVPset -= 2;
                 }
                 else
                 {
                     uiOVPset -= 10;
                 }
             }
             else if(state1 == 2)
             {

                 if(mode1 == 1)
                 {
                     uiOVPset += 2;
                 }
                 else
                 {
                     uiOVPset += 10;
                 }
             }
             else
             {
                 // Do Nothing
             }
                                                 ////          AB
             state1 = 3;                         // State 1--> 11
          }
         else if ((P2IN & 0x30) == 0x10) // if pulse A,B --> 0010 0000
         {
             if(state1 == 1)
             {
                 if(mode1 == 1)
                 {
                     uiOVPset -= 2;
                 }
                 else
                 {
                     uiOVPset -= 10;
                 }
             }
             else if(state1 == 3)
             {

                 if(mode1 == 1)
                 {
                     uiOVPset += 2;
                 }
                 else
                 {
                     uiOVPset += 10;
                 }
             }
             else
             {
                 // Do Nothing
             }
                                                  ////          AB
             state1 = 4;                          // State 1--> 10
          }

        if (uiOVPset > 5270)
        {
            uiOVPset = 5270;
        }
        else if (uiOVPset <= uiVrefSet)
        {
            uiOVPset = ((uiVrefSet) + ((int)(uiVrefSet * 0.1))); // Raise it to 10% above uiVrefSet
        }
        else if(uiOVPset < 0)
        {
            uiOVPset = 0;
        }
        else
        {

        }

        DisplayDataTx.uiOVP = uiOVPset;


    }
    else if(uiPageNo == 3U)
    {
        if((P2IN & 0x30) == 0x00)   //if pulse A,B --> 0000 0000
        {
            if(state1 == 2)
            {
                if(mode1 == 1)
                {
                    uiVrefSet -= 2;
                }
                else
                {
                    uiVrefSet -= 10;
                }
            }
            else if(state1 == 4)
            {
                 if(mode1 == 1)
                 {
                     uiVrefSet += 2;
                 }
                 else
                 {
                     uiVrefSet += 10;
                 }
            }
            else
            {
                // Do Nothing
            }
                                              ////          AB
            state1 = 1;                       // State 1: - 00

         }
         else if((P2IN & 0x30) == 0x20) // if pulse A,B --> 0010 0000
         {
            if(state1 == 3)
            {

                if(mode1 == 1)
                {
                    uiVrefSet -= 2;
                }
                else
                {
                    uiVrefSet -= 10;
                }
            }

            else if(state1 == 1)
            {

                if(mode1 == 1)
                {
                    uiVrefSet += 2;
                }
                else
                {
                    uiVrefSet += 10;
                }
            }
            else
            {
                // Do Nothing
            }
                                                ////          AB
            state1 = 2;                         // State 1--> 01
         }
         else if((P2IN & 0x30) == 0x30) // if pulse A,B --> 0011 0000
         {
             if(state1 == 4)
             {

                 if(mode1 == 1)
                 {
                     uiVrefSet -= 2;
                 }
                 else
                 {
                     uiVrefSet -= 10;
                 }
             }
             else if(state1 == 2)
             {

                 if(mode1 == 1)
                 {
                     uiVrefSet += 2;
                 }
                 else
                 {
                     uiVrefSet += 10;
                 }
             }
             else
             {
                 // Do Nothing
             }
                                                 ////          AB
             state1 = 3;                         // State 1--> 11
          }
         else if ((P2IN & 0x30) == 0x10) // if pulse A,B --> 0010 0000
         {
             if(state1 == 1)
             {
                 if(mode1 == 1)
                 {
                     uiVrefSet -= 2;
                 }
                 else
                 {
                     uiVrefSet -= 10;
                 }
             }
             else if(state1 == 3)
             {

                 if(mode1 == 1)
                 {
                     uiVrefSet += 2;
                 }
                 else
                 {
                     uiVrefSet += 10;
                 }
             }
             else
             {
                 // Do Nothing
             }
                                                  ////          AB
             state1 = 4;                          // State 1--> 10
          }



        if (uiVrefSet > uiOVPset)
        {
            uiVrefSet = (int)(uiOVPset * 0.95);
        }
        else if (uiVrefSet <= 0)
        {
            uiVrefSet = 0;
        }
    }

    if((P6IN & 0x01) == 0x00)                   //V Fine Button... button 1.
    {
        if(uiLeftModeChangeDelay >= 500U)
        {
            uiLeftModeChangeDelay = 0;
            mode1 ^= 1U;
            P4OUT ^= (0x08);                //LED 1 on/off.
        }
        else
        {
            uiLeftModeChangeDelay++;
        }
    }

     DisplayDataTx.uiVref = uiVrefSet;
}



void OLED_fnRightEncoder()                        // Encoder right starts
{
    if(((P4IN & 0x60) == 0x00) && (uiPageNo == 3U))  // State 1: - 00
     {
        if(state2 == 2)
        {

            if(mode2 == 1)
            {
                uiIrefSet -= 1;
            }
            else
            {
                uiIrefSet -= 10;
            }
        }
        else if(state2 == 4)
        {
             if(mode2 == 1)
             {
                 uiIrefSet += 1;
             }
             else
             {
                 uiIrefSet += 10;
             }
        }
        else
        {
            // Do Nothing
        }

        state2 = 1;

     }
    else if((P4IN & 0x60) == 0x40) // State 2: - 01
     {
        if(state2 == 3)
        {

            if(mode2 == 1)
            {
                uiIrefSet -= 1;
            }
            else
            {
                uiIrefSet -= 10;
            }
        }

        else if(state2 == 1)
        {

            if(mode2 == 1)
            {
                uiIrefSet += 1;
            }
            else
            {
                uiIrefSet += 10;
            }
        }
        else
        {
            // Do Nothing
        }

        state2 = 2;
     }
     else if((P4IN & 0x60) == 0x60) // State 3: - 11
     {
         if(state2 == 4)
         {

             if(mode2 == 1)
             {
                 uiIrefSet -= 1;
             }
             else
             {
                 uiIrefSet -= 10;
             }
         }
         else if(state2 == 2)
         {

             if(mode2 == 1)
             {
                 uiIrefSet += 1;
             }
             else
             {
                 uiIrefSet += 10;
             }
         }
         else
         {
             // Do Nothing
         }

         state2 = 3;
      }
     else if ((P4IN & 0x60) == 0x20) // State 4: - 10
     {
         if(state2 == 1)
         {
             if(mode2 == 1)
             {
                 uiIrefSet -= 1;
             }
             else
             {
                 uiIrefSet -= 10;
             }
         }
         else if(state2 == 3)
         {

             if(mode2 == 1)
             {
                 uiIrefSet += 1;
             }
             else
             {
                 uiIrefSet += 10;
             }
         }
         else
         {
             // Do Nothing
         }

         state2 = 4;
      }



    if (uiIrefSet > MAX_POS2)
    {
        uiIrefSet = MAX_POS2;
    }
    else if (uiIrefSet <= 0)
    {
        uiIrefSet = 0;
    }

    if((P6IN & 0x20) == 0x00)           //A Fine. button 6
    {
        if(uiRightModeChangeDelay >= 500U)
        {
            uiRightModeChangeDelay = 0;
            mode2 ^= 1U;
            P2OUT ^= (0x04);            //LED 6 on/off.
        }
        else
        {
            uiRightModeChangeDelay++;
        }
    }
    else
    {

    }

    DisplayDataTx.uiIref = uiIrefSet;

  }
