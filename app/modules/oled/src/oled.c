/*=============================================================================
 Copyright Zepco Technologies Pvt. Ltd. All Rights Reserved.
 All trademarks are owned by Zepco Technologies Pvt. Ltd.
 ============================================================================ */

/*==============================================================================
 @file  oled.c
 @author DhimanRoy , Santhosh N
 @date 27-Oct-2023

 @brief Description:

 It contains all the functions related to the OLED display auto-toggle feature,
 positioning of values and labels in the pages used.
 ==============================================================================*/

/*==============================================================================
 Includes
 ==============================================================================*/
#include <msp430.h>
#include "oled.h"
#include "isr.h"
#include "rs232.h"
#include "buttons.h"
#include "state.h"
#include <encoder.h>
#include <stdint.h>

/*==============================================================================
 Defines
 ==============================================================================*/

/*==============================================================================
 Enums
 ==============================================================================*/

/*==============================================================================
 Structures Variables.
 ==============================================================================*/
OLED_tuFAULT_REG tuFaultReg;
OLED_rzDISPLAY_DATA DisplayDataRx;
OLED_tzDISPLAY_DATA DisplayDataTx;

/*==============================================================================
 Macros
 ==============================================================================*/

/*==============================================================================
 Local Function Prototypes
 ==============================================================================*/
void OLED_fnCommand(char i);
void OLED_fnData(char i);
void OLED_fnStrDisp(const char *str);
void OLED_fnReverse(char *str, int len);
void OLED_fnNodeCount(void);
void OLED_fnUpdateDisplay(void);
void OLED_fnPrintPage(unsigned int uiPgNo);
int OLED_fnIntToStr_v2(int x, char str[], int d, char filler); //whole number part with "zero" or "blank-space" print function...
int OLED_fnFracToStr_v2(int x, char str[], int d, int mult); //decimal part with no trailing zeroes print function...
static const char *fault_lookup(void);

/*==============================================================================
 Local Variables
 ==============================================================================*/

/*==============================================================================
 Local Constants
 ==============================================================================*/
unsigned int uiBuildNo = 1U;
unsigned int update_page = 1;
unsigned int uiAutoScrollDelay = 0U;
unsigned int uiAutoScroll = 0U;
unsigned int uiAutoScrollEn = 0U;
unsigned int uiCommError;
unsigned int uiToggleCnt = 0U;
unsigned int uiToggleState = 0U;
char intstr[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
unsigned int uiEditVarCnt;  //nodeID variable...
int uiVrefScaled = 0;
int uiIrefScaled = 0;

/**
 * @brief void OLED_fnCommand(char i)
 * @param char i
 * @return void
 *
 * 1.Set control & data pins for OLED.
 * 2.
 * 3.
 */

void OLED_fnCommand(char i)
{
    // Setting up the command
    P3OUT = i;              // data on port for command function
    P5OUT &= ~(0x01);       // data/command select LOW – command
    P5OUT &= ~(0x02);       // read/write select LOW – write

    // Adding a small delay to allow any noise to settle
    __delay_cycles(50);     // Short delay to filter out noise

    // Latching the command
    P5OUT |= 0x04;          // enable HIGH
    __delay_cycles(200);    // Ensure timing meets datasheet specs
    P5OUT &= ~(0x04);       // enable LOW – data latched
    __delay_cycles(200);    // Ensure timing meets datasheet specs

    // Optional: Redundant write to ensure command is correctly latched
//        P5OUT |= 0x04;          // enable HIGH again for redundancy
    __delay_cycles(200);    // Short delay
    P5OUT &= ~(0x04);       // enable LOW
}

/**
 * @brief OLED_fnData(char i)
 * @param char i
 * @return void
 *
 * 1.
 * 2.
 * 3.
 */

void OLED_fnData(char i)
{
    // Setting up the data
    P3OUT = i;              // data on port for data function
    P5OUT |= 0x01;          // data/command select HIGH – data
    P5OUT &= ~(0x02);       // read/write select LOW – write

    // Adding a small delay to allow any noise to settle
    __delay_cycles(50);     // Short delay to filter out noise

    // Latching the data
    P5OUT |= 0x04;          // enable HIGH
    __delay_cycles(200);    // Ensure timing meets datasheet specs
    P5OUT &= ~(0x04);       // enable LOW – data latched
    __delay_cycles(300);    // Ensure timing meets datasheet specs

    //    // Optional: Redundant write to ensure data is correctly latched
    //    P5OUT |= 0x04;          // enable HIGH again for redundancy
    __delay_cycles(200);    // Short delay
    P5OUT &= ~(0x04);       // enable LOW
}

/**
 * @brief OLED_fnInit()
 * @param void
 * @return void
 *
 * 1.delay for segment updates.
 * 2.display cursor on off, clock setting.
 * 3.COM SEG direction.
 */

void OLED_fnInit()
{
    P2OUT |= 0x10;           //reset HIGH – inactive (Normal Operation)
    /**
     * Changing delay for stable display init from 200
     */
    __delay_cycles(200);  //delay for updating values.

    OLED_fnCommand(0x2A); //function set (extended OLED_fnCommand set)
    OLED_fnCommand(0x71); //function selection A

    OLED_fnData(0x5C); // OLED_fnData(0x00)-- disable internal VDD regulator (2.8V I/O); data(0x5C) = enable regulator (5V I/O)

    OLED_fnCommand(0x28); //function set (fundamental OLED_fnCommand set)
    OLED_fnCommand(0x0C); //display on, cursor on, blink on.
    OLED_fnCommand(0x2A); //(extended OLED_fnCommand set)--> Function Set
    OLED_fnCommand(0x79); //In extended OLED_fnCommand set --> (OLED OLED_fnCommand set enabled)
    OLED_fnCommand(0xD5); //set display clock divide ratio/oscillator frequency
    OLED_fnCommand(0x70); //set display clock divide ratio/oscillator frequency
    OLED_fnCommand(0x78); ////In extended OLED_fnCommand set --> (OLED OLED_fnCommand set disabled)
    OLED_fnCommand(0x08); //extended function set (2-lines)
    OLED_fnCommand(0x06); //COM SEG direction
    OLED_fnCommand(0x72); //function selection B

    OLED_fnData(0xFA);    //ROM CGRAM selection

    OLED_fnCommand(0x2A); //function set (extended OLED_fnCommand set)
    OLED_fnCommand(0x79); //OLED OLED_fnCommand set enabled
    OLED_fnCommand(0xDA); //set SEG pins hardware configuration
    OLED_fnCommand(0x10); //set SEG pins hardware configuration
    OLED_fnCommand(0xDC); //function selection C
    OLED_fnCommand(0x00); //function selection C
    OLED_fnCommand(0x81); //set contrast control
    OLED_fnCommand(0x7F); //set contrast control
    OLED_fnCommand(0xD9); //set phase length
    OLED_fnCommand(0xF1); //set phase length
    OLED_fnCommand(0xDB); //set VCOMH deselect level
    OLED_fnCommand(0x40); //set VCOMH deselect level
    OLED_fnCommand(0x78); //OLED OLED_fnCommand set disabled
    OLED_fnCommand(0x28); //function set (fundamental OLED_fnCommand set)
    OLED_fnCommand(0x01); //clear display
    OLED_fnCommand(0x80); //set DDRAM address to 0x00
    OLED_fnCommand(0x0C); //display ON

    /**
     * Increasing Delay for stable display init from 200 to 400
     */

    __delay_cycles(200);  //delay
}

/**
 * @brief OLED_fnUpdateDisplay()
 * @param void
 * @return void
 *
 * 1. AutoScrolling
 * 2. Update Page No
 * 3. If any fault occur, latch display to fault page.
 * 4.Auto scrolling of pages.
 */
void OLED_fnUpdateDisplay()
{
    /**
     * 15U = 150ms
     */
    if (uiToggleCnt >= 10U)
    {
        uiToggleCnt = 0;
        uiToggleState ^= 1;
    }
    else
    {
        uiToggleCnt++;
    }

    if (uiScrollBlankDelay > 0)
    {
        uiAutoScroll = 0;                 //Auto Scroll Stopping.
        uiScrollBlankDelay--;             //Auto Scroll Stopping time decrement.
    }
    else
    {
        uiAutoScroll = 0;                   //Auto Scroll disabling.
    }

    if (tuFaultReg.all > 0)   /// Fault page scroll lock...
    {
        if (uiAutoScrollEn == 0)
        {
            uiAutoScrollEn = 0;
            uiPageNo = 3U;   /// Fault page button scroll lock...
        }
        else
        {

        }
    }
    else
    {
        OLED_fnPrintPage(uiPageNo);          //System Page Display "switches" here..
    }

    /*
     *
     * Auto-scroll logic here.
     *
     */
    if ((uiAutoScroll == 1) && (uiEditMode == 0) && (uiAutoScrollEn == 0))
    {
        if (uiAutoScrollDelay >= 100U)
        {
            uiAutoScrollDelay = 0;

            uiPageNo++;

            if (uiPageNo > 6)
            {
                uiPageNo = 1;
            }
        }
        else
        {
            uiAutoScrollDelay++;
        }
    }

    OLED_fnPrintPage(uiPageNo);          //System Page Display "switches" here..
}

void OLED_fnPrintPage(unsigned int uiPgNo)
{
    if (uiCommError == 0)
    {
        switch (uiPgNo)
        {
        case 1:

            /**
             * 0123456789ABCDEF
             * ----------------
             * xxx.xV  xx.xA
             * Status: XXXXXXXX
             */

            OLED_fnCommand(0x80);
            OLED_fnStrDisp("    ");
            OLED_fnIntToStr_v2(DisplayDataRx.uiVoutInt, intstr, 3,
            filler_space);
            OLED_fnStrDisp(intstr);
            OLED_fnStrDisp(".");
            OLED_fnFracToStr_v2(DisplayDataRx.uiVoutFrac, intstr, 2, 2);
            OLED_fnStrDisp(intstr);
            OLED_fnStrDisp(" V    ");
//            OLED_fnIntToStr_v2(DisplayDataRx.uiIoutInt, intstr, 2,
//            filler_space);
//            OLED_fnStrDisp(intstr);
//            OLED_fnStrDisp(".");
//            OLED_fnFracToStr_v2(DisplayDataRx.uiIoutFrac, intstr, 2, 2);
//            OLED_fnStrDisp(intstr);
//            OLED_fnStrDisp(" ");
//            OLED_fnStrDisp("A");

            OLED_fnCommand(0xC0);
            OLED_fnStrDisp("STATUS: ");

            if (DisplayDataRx.uiPulseState == 4U)
            {
                OLED_fnStrDisp("FAULT   ");
            }
            else
            {
                if (DisplayDataRx.uiPulseState == 3)
                {
                    OLED_fnStrDisp("LIVE    ");
                }
                else if(DisplayDataRx.uiPulseState == 2)
                {
                    OLED_fnStrDisp("IDLE    ");
                }
                else if (DisplayDataRx.uiPulseState == 1)
                {
                    OLED_fnStrDisp("PRCHARGE");
                }
                else if (DisplayDataRx.uiPulseState == 0)
                {
                    OLED_fnStrDisp("INIT    ");
                }
                else
                {

                }
            }

            break;
        case 2:

            /**
             * 0123456789ABCDEF
             * ----------------
             * A: xxx.xV xx.xxA
             * Node ID:  XX
             *
             */
            OLED_fnCommand(0x80);

            OLED_fnStrDisp("FLT:");      //4

            OLED_fnStrDisp(fault_lookup());


            OLED_fnCommand(0xC0);
            OLED_fnStrDisp("                ");


            OLED_fnCommand(0x80);
            OLED_fnStrDisp("Node ID: ");

            if (uiEditMode == 1)
            {
                if (uiToggleState == 0)
                {
                    OLED_fnStrDisp("       ");
                }
                else
                {
                    OLED_fnIntToStr_v2(uiEditVarCnt, intstr, 2, filler_space);
                    OLED_fnStrDisp(intstr);
                    OLED_fnStrDisp("     ");
                }
            }
            else
            {
                OLED_fnIntToStr_v2(DisplayDataRx.uiNodeId, intstr, 2,
                filler_space);
                OLED_fnStrDisp(intstr);
                OLED_fnStrDisp("     ");
            }

            OLED_fnCommand(0xC0);

            OLED_fnStrDisp("FLT:");      //6

            OLED_fnStrDisp(fault_lookup());




            break;
        case 3:


            /**
             * 0123456789ABCDEF
             * ----------------
             * A: xxx.xV xx.xxA
             * Fault:NO FAULT
             *
             */

            OLED_fnCommand(0x80);
            OLED_fnIntToStr_v2(DisplayDataRx.uiVoutInt, intstr, 2,
            filler_space);
            OLED_fnStrDisp(intstr);
            OLED_fnStrDisp(".");
            OLED_fnFracToStr_v2(DisplayDataRx.uiVoutFrac, intstr, 2, 2);
            OLED_fnStrDisp(intstr);
            OLED_fnStrDisp(" V ");
            OLED_fnIntToStr_v2(DisplayDataRx.uiIoutInt, intstr, 3,
            filler_space);
            OLED_fnStrDisp(intstr);
            OLED_fnStrDisp(".");
            OLED_fnFracToStr_v2(DisplayDataRx.uiIoutFrac, intstr, 2, 2);
            OLED_fnStrDisp(intstr);
            OLED_fnStrDisp(" ");
            OLED_fnStrDisp("A");

            OLED_fnCommand(0xC0);

            OLED_fnStrDisp("FLT:");      //6

            OLED_fnStrDisp(fault_lookup());

            break;
        case 4:
            OLED_fnCommand(0x80);
            OLED_fnStrDisp("ServerIP_setting");

            if (uiEditMode == 1)
            {
                /* Octet 1 */
                OLED_fnCommand(0xC0);
                if (uiposition == 1U && uiToggleState == 0)
                    OLED_fnStrDisp("   ");
                else
                {
                    OLED_fnIntToStr_v2(uiserver_ip_oct1, intstr, 3, filler_space);
                    OLED_fnStrDisp(intstr);
                }
                OLED_fnStrDisp(".");

                /* Octet 2 */
                OLED_fnCommand(0xC4);
                if (uiposition == 2U && uiToggleState == 0)
                    OLED_fnStrDisp("   ");
                else
                {
                    OLED_fnIntToStr_v2(uiserver_ip_oct2, intstr, 3, filler_space);
                    OLED_fnStrDisp(intstr);
                }
                OLED_fnStrDisp(".");

                /* Octet 3 */
                OLED_fnCommand(0xC8);
                if (uiposition == 3U && uiToggleState == 0)
                    OLED_fnStrDisp("   ");
                else
                {
                    OLED_fnIntToStr_v2(uiserver_ip_oct3, intstr, 3, filler_space);
                    OLED_fnStrDisp(intstr);
                }
                OLED_fnStrDisp(".");

                /* Octet 4 */
                OLED_fnCommand(0xCC);
                if (uiposition == 4U && uiToggleState == 0)
                    OLED_fnStrDisp("   ");
                else
                {
                    OLED_fnIntToStr_v2(uiserver_ip_oct4, intstr, 3, filler_space);
                    OLED_fnStrDisp(intstr);
                }
                OLED_fnStrDisp(" ");   /* pad last column */
            }
            else
            {
                /* Non-edit: display received server IP */
                OLED_fnCommand(0xC0);
                OLED_fnIntToStr_v2(DisplayDataRx.uirxservr_ip_oct1, intstr, 3, filler_space);
                OLED_fnStrDisp(intstr);
                OLED_fnStrDisp(".");
                OLED_fnIntToStr_v2(DisplayDataRx.uirxservr_ip_oct2, intstr, 3, filler_space);
                OLED_fnStrDisp(intstr);
                OLED_fnStrDisp(".");
                OLED_fnIntToStr_v2(DisplayDataRx.uirxservr_ip_oct3, intstr, 3, filler_space);
                OLED_fnStrDisp(intstr);
                OLED_fnStrDisp(".");
                OLED_fnIntToStr_v2(DisplayDataRx.uirxservr_ip_oct4, intstr, 3, filler_space);
                OLED_fnStrDisp(intstr);
                OLED_fnStrDisp(" ");
            }
            break;

        case 5:
            OLED_fnCommand(0x80);
            OLED_fnStrDisp("ClientIP_setting");

            if (uiEditMode == 1)
            {
                /* Octet 1 */
                OLED_fnCommand(0xC0);
                if (uiposition == 1U && uiToggleState == 0)
                    OLED_fnStrDisp("   ");
                else
                {
                    OLED_fnIntToStr_v2(uiclient_ip_oct1, intstr, 3, filler_space);
                    OLED_fnStrDisp(intstr);
                }
                OLED_fnStrDisp(".");

                /* Octet 2 */
                OLED_fnCommand(0xC4);
                if (uiposition == 2U && uiToggleState == 0)
                    OLED_fnStrDisp("   ");
                else
                {
                    OLED_fnIntToStr_v2(uiclient_ip_oct2, intstr, 3, filler_space);
                    OLED_fnStrDisp(intstr);
                }
                OLED_fnStrDisp(".");

                /* Octet 3 */
                OLED_fnCommand(0xC8);
                if (uiposition == 3U && uiToggleState == 0)
                    OLED_fnStrDisp("   ");
                else
                {
                    OLED_fnIntToStr_v2(uiclient_ip_oct3, intstr, 3, filler_space);
                    OLED_fnStrDisp(intstr);
                }
                OLED_fnStrDisp(".");

                /* Octet 4 */
                OLED_fnCommand(0xCC);
                if (uiposition == 4U && uiToggleState == 0)
                    OLED_fnStrDisp("   ");
                else
                {
                    OLED_fnIntToStr_v2(uiclient_ip_oct4, intstr, 3, filler_space);
                    OLED_fnStrDisp(intstr);
                }
                OLED_fnStrDisp(" ");   /* pad last column */
            }
            else
            {
                OLED_fnCommand(0xC0);
                OLED_fnIntToStr_v2(DisplayDataRx.uirxclient_ip_oct1, intstr, 3, filler_space);
                OLED_fnStrDisp(intstr);
                OLED_fnStrDisp(".");
                OLED_fnIntToStr_v2(DisplayDataRx.uirxclient_ip_oct2, intstr, 3, filler_space);
                OLED_fnStrDisp(intstr);
                OLED_fnStrDisp(".");
                OLED_fnIntToStr_v2(DisplayDataRx.uirxclient_ip_oct3, intstr, 3, filler_space);
                OLED_fnStrDisp(intstr);
                OLED_fnStrDisp(".");
                OLED_fnIntToStr_v2(DisplayDataRx.uirxclient_ip_oct4, intstr, 3, filler_space);
                OLED_fnStrDisp(intstr);
                OLED_fnStrDisp(" ");
            }
            break;
        case 6:

            /**
             * 0123456789ABCDEF
             * ----------------
             * Set Default IP:
             *             Set
             *
             */

            OLED_fnCommand(0x80);
            OLED_fnStrDisp("Set Default IP: ");

            OLED_fnCommand(0xC0);
            OLED_fnStrDisp("          ");



            if(DisplayDataTx.ui_IP_reset_flag == 1U)
            {
                OLED_fnStrDisp("Set ok");
            }
            else
            {
                if(uiToggleState == 0)
                {
                    OLED_fnStrDisp("      ");
                }
                else
                {
                    OLED_fnStrDisp("   Set");
                }
            }

            break;
        case 7:

            /**
             * 0123456789ABCDEF
             * ----------------
             * A: xxx.xV xx.xxA
             * Fault:NO FAULT
             *
             */

            OLED_fnCommand(0x80);
            OLED_fnIntToStr_v2(DisplayDataRx.uiVoutInt, intstr, 2,
            filler_space);
            OLED_fnStrDisp(intstr);
            OLED_fnStrDisp(".");
            OLED_fnFracToStr_v2(DisplayDataRx.uiVoutFrac, intstr, 2, 2);
            OLED_fnStrDisp(intstr);
            OLED_fnStrDisp(" V ");
            OLED_fnIntToStr_v2(DisplayDataRx.uiIoutInt, intstr, 3,
            filler_space);
            OLED_fnStrDisp(intstr);
            OLED_fnStrDisp(".");
            OLED_fnFracToStr_v2(DisplayDataRx.uiIoutFrac, intstr, 2, 2);
            OLED_fnStrDisp(intstr);
            OLED_fnStrDisp(" ");
            OLED_fnStrDisp("A");

            OLED_fnCommand(0xC0);

            OLED_fnStrDisp("Fault:");      //6

            OLED_fnStrDisp(fault_lookup());


            break;
        case 8:

            OLED_fnCommand(0x80);
            OLED_fnStrDisp("A:");
            OLED_fnIntToStr_v2(DisplayDataRx.uiVoutInt, intstr, 3,
            filler_space);
            OLED_fnStrDisp(intstr);
            OLED_fnStrDisp(".");
            OLED_fnFracToStr_v2(DisplayDataRx.uiVoutFrac, intstr, 1, 1);
            OLED_fnStrDisp(intstr);
            OLED_fnStrDisp(" V ");
            OLED_fnIntToStr_v2(DisplayDataRx.uiIoutInt, intstr, 2,
            filler_space);
            OLED_fnStrDisp(intstr);
            OLED_fnStrDisp(".");
            OLED_fnFracToStr_v2(DisplayDataRx.uiIoutFrac, intstr, 1, 1);
            OLED_fnStrDisp(intstr);
            OLED_fnStrDisp(" ");
            OLED_fnStrDisp("A");

            OLED_fnCommand(0xC0);
            OLED_fnStrDisp("T1:");
            OLED_fnIntToStr_v2(DisplayDataRx.uiTemp1Int, intstr, 3,
            filler_space);
            OLED_fnStrDisp(intstr);
            OLED_fnStrDisp("  ");
            OLED_fnStrDisp("T2:");
            OLED_fnIntToStr_v2(DisplayDataRx.uiTemp2Int, intstr, 3,
            filler_space);
            OLED_fnStrDisp(intstr);
            OLED_fnStrDisp("  ");
//            OLED_fnData(0xF2);      ////------->*C
//            OLED_fnStrDisp("C");
            break;
        default:
            break;
        }
    }
    else if (uiCommError == 1)               //error Page...
    {
        OLED_fnCommand(0x80);
        OLED_fnStrDisp(" Communication  ");
        OLED_fnCommand(0xC0);
        OLED_fnStrDisp("     Error!     ");
    }

}

void OLED_fnWelcomePage(void)
{
    OLED_fnCommand(0x80);
    OLED_fnStrDisp("15KW_DC-DC_FC-LQ");
    OLED_fnCommand(0xC0);
    OLED_fnStrDisp("   VER: ");
    OLED_fnIntToStr_v2(DisplayDataRx.uirxbuild_ver_major, intstr, 1,
    filler_space);
    OLED_fnStrDisp(intstr);
    OLED_fnStrDisp(".");
    OLED_fnIntToStr_v2(DisplayDataRx.uirxbuild_ver_minor, intstr, 1,
    filler_space);
    OLED_fnStrDisp(intstr);
    OLED_fnStrDisp(".");
    OLED_fnIntToStr_v2(DisplayDataRx.uirxbuild_ver, intstr, 1,
    filler_space);
    OLED_fnStrDisp(intstr);
    OLED_fnStrDisp("   ");
}

void OLED_fnStrDisp(const char *str)
{
    while (*str)
    {
        OLED_fnData(*str);
        str++;
    }
}

void OLED_fnReverse(char *str, int len)
{
    unsigned int i = 0, j = len - 1, temp;
    while (i < j)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

int OLED_fnIntToStr_v2(int x, char str[], int d, char filler)
{
    unsigned int i = 0;

    if (x == 0)
    {
        str[i++] = '0';
    }
    else
    {
        while (x)
        {
            str[i] = x % 10 + '0';
            i = i + 1;
            x = x / 10;
        }
    }

    while (i < d)
    {
        str[i++] = filler;
    }

    OLED_fnReverse(str, i);

    str[i] = '\0';

    return i;
}

int OLED_fnFracToStr_v2(int x, char str[], int d, int mult)
{
    unsigned int i = 0;

    if (x == 0)
    {
        str[i++] = '0';
    }
    else
    {
        while (x)
        {
            str[i] = x % 10 + '0';
            i = i + 1;
            x = x / 10;
        }

        while (i < mult)
        {
            str[i++] = '0';
        }

        OLED_fnReverse(str, i);
    }

    while (i < d)
    {
        str[i++] = ' ';
    }

    if (i > d)
    {
        str[d] = '\0';
    }
    else
    {
        str[i] = '\0';
    }

    return i;
}

/* --- Fault table: add more entries as needed --- */
typedef struct
{
    uint8_t   active;
    const char *text;   /* must be <=12 chars to fit " %12s" field */
} FaultEntry_t;

static const char *fault_lookup(void)
{
    static uint8_t  activeIdx[11];   /* holds indices of active faults */
    static uint8_t  activeCount = 0;
    static uint16_t blinkTimer  = 0;
    static uint8_t  showIdx     = 0;

    static const char *faultText[11] =
    {
        " B-CH1-OC   ",
        " B-CH1-OC   ",
        " PSFB-OC    ",
        " B-OV       ",
        " B-UV       ",
        " PSFB-OV    ",
        " PSFB-UV    ",
        " OTP        ",
        " EEPROM     ",
        " FC-UV      ",
        " FC-OV      ",
    };

    uint8_t bits[10];
    bits[0] = tuFaultReg.bit.bt_Boost1_OverCurrent;
    bits[1] = tuFaultReg.bit.bt_Boost2_OverCurrent;
    bits[2] = tuFaultReg.bit.bt_PSFB_OverCurrent;
    bits[3] = tuFaultReg.bit.bt_BoostOverVoltage;
    bits[4] = tuFaultReg.bit.bt_BoostUnderVoltage;
    bits[5] = tuFaultReg.bit.bt_PSFBOverVoltage;
    bits[6] = tuFaultReg.bit.bt_OTPFault;
    bits[7] = tuFaultReg.bit.bt_EEPROMFault;
    bits[8] = tuFaultReg.bit.bt_FCUnderVoltage;
    bits[9] = tuFaultReg.bit.bt_FCOverVoltage;

    /* Rebuild list of active faults every call (cheap, 10 checks) */
    activeCount = 0;
    uint8_t i;
    for (i = 0U; i < 10U; i++)
    {
        if (bits[i] == 1U)
        {
            activeIdx[activeCount] = i;
            activeCount++;
        }
    }

    if (activeCount == 0U)
    {
        return "   NO FLT   ";
    }

    /* Advance blink timer; this function assumed called ~every 100ms.
       BLINK_PERIOD = 5 -> toggle every 500ms. Adjust to your actual call rate. */
    blinkTimer++;
    if (blinkTimer >= 70U)
    {
        blinkTimer = 0U;
        showIdx++;
        if (showIdx >= activeCount)
        {
            showIdx = 0U;
        }
    }

    return faultText[activeIdx[showIdx]];
}

