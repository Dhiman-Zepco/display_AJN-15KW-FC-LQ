/*=============================================================================
Copyright Zepco Technologies Pvt. Ltd. All Rights Reserved.
All trademarks are owned by Zepco Technologies Pvt. Ltd.
============================================================================ */

/*==============================================================================
 @file  oled.h
 @author Lenovo   
 @date Oct 27, 2023

 @brief Description
==============================================================================*/

#ifndef MODULES_OLED_INC_OLED_H_
#define MODULES_OLED_INC_OLED_H_

/*==============================================================================
 Includes
==============================================================================*/


/*==============================================================================
 Defines
==============================================================================*/

/*==============================================================================
 Enums
==============================================================================*/
//typedef enum RS232_eRX_FAULT_STATES
//{
//    RS232_RX_OFF,
//    RS232_RX_ON,
//    RS232_RX_FAULT,
//
//} RS232_teRX_FAULT_STATES;

/*==============================================================================
 Structures
==============================================================================*/

typedef struct OLED_RzDISPLAY_DATA
{
    unsigned int SOF1;                      //Start of frame 1.
    unsigned int SOF2;                      //Start of frame 2.
    unsigned int FRAME_LEN;                 //Frame Length
    unsigned int CMDH;                      //Command High.
    unsigned int CHML;                      //Command Low.
    unsigned int uiRemoteProtect;       //Remote Control Protection Signal.
    unsigned int uiVoutInt;                 //Volt rms integer part.
    unsigned int uiVoutFrac;            //Volt rms decimal part.
    unsigned int uiSCRoutInt;                 //Volt rms integer part.
    unsigned int uiSCRoutFrac;            //Volt rms decimal part.
    unsigned int uiIoutInt;            //Total Channel Current int part.
    unsigned int uiIoutFrac;            //Total Channel Current dec part.
    unsigned int uiRPhVolts;           //R Phase voltage input.
    unsigned int uiYPhVolts;           //Y Phase voltage input.
    unsigned int uiBPhVolts;           //B Phase voltage input.
    unsigned int uiVrefInt;              //Vref integer part.
    unsigned int uiVrefFrac;              //Vref decimal part.
    unsigned int uiIrefInt;              //Iref integer part.
    unsigned int uiIrefFrac;              //Iref decimal part.
    unsigned int uiOVPInt;               //OVP integeer part.
    unsigned int uiOVPFrac;               //OVP decimal part.
    unsigned int uiTemp1Int;               //Internal Temp1. reading int part.
    unsigned int uiTemp1Dec;               //Internal Temp1. reading dec part.
    unsigned int uiTemp2Int;               //Internal Temp2. reading int part.
    unsigned int uiTemp2Dec;               //Internal Temp2. reading dec part.
    unsigned int uiNodeId;                 //Set Node_id recieved.
    unsigned int uiFaultReg;               //recvd fault status.
    unsigned int uiPulseState;             //recvd pulse status.
    unsigned int ui_op_mode;               //CV/CC modes.
    unsigned int uirxbuild_ver;             //Build version..
    unsigned int uirxbuild_ver_major;      //Build version major number.
    unsigned int uirxbuild_ver_minor;      //Build version minor number.
    unsigned int uirxservr_ip_oct1;         //rx_server ip octet 1.
    unsigned int uirxservr_ip_oct2;         //rx_server ip octet 2.
    unsigned int uirxservr_ip_oct3;         //rx_server ip octet 3.
    unsigned int uirxservr_ip_oct4;         //rx_server ip octet 4.
    unsigned int uirxclient_ip_oct1;        //rx_client ip octet 1.
    unsigned int uirxclient_ip_oct2;        //rx_client ip octet 2.
    unsigned int uirxclient_ip_oct3;        //rx_client ip octet 3.
    unsigned int uirxclient_ip_oct4;        //rx_client ip octet 4.
    unsigned int uiCrc;                    //CRC value.


} OLED_rzDISPLAY_DATA;



typedef struct OLED_TzDISPLAY_DATA
{
    unsigned int uiRemoteProtect;
    unsigned int uiNodeId;
    unsigned int uiOVP;
    unsigned int uiOCP;
    unsigned int uiVref;
    unsigned int uiIref;
    unsigned int uiOnOffSwitch;
    unsigned int ui_IP_reset_flag;
    unsigned int ui_tx_servr_ip_oct1;         //tx_server ip octet 1.
    unsigned int ui_tx_servr_ip_oct2;         //tx_server ip octet 2.
    unsigned int ui_tx_servr_ip_oct3;         //tx_server ip octet 3.
    unsigned int ui_tx_servr_ip_oct4;         //tx_server ip octet 4.
    unsigned int ui_tx_client_ip_oct1;        //tx_client ip octet 1.
    unsigned int ui_tx_client_ip_oct2;        //tx_client ip octet 2.
    unsigned int ui_tx_client_ip_oct3;        //tx_client ip octet 3.
    unsigned int ui_tx_client_ip_oct4;        //tx_client ip octet 4.

} OLED_tzDISPLAY_DATA;




//OLED_rzDISPLAY_DATA DisplayDataRx;
//OLED_tzDISPLAY_DATA DisplayDataTx;


typedef struct OLED_zFAULT_BITS
{
    unsigned int bt_Boost1_OverCurrent:1;  // Boost1 OverCurrent
    unsigned int bt_Boost2_OverCurrent:1;  // Boost2 OverCurrent
    unsigned int bt_PSFB_OverCurrent:1;     // PSFB OverCurrent
    unsigned int bt_BoostOverVoltage:1;    //Boost Over Voltage
    unsigned int bt_BoostUnderVoltage:1;   //Boost Under Voltage.
    unsigned int bt_PSFBOverVoltage:1;     //PSFB_OverVoltage.
    unsigned int bt_PSFBUnderVoltage:1;    //PSFB Under Voltage.
    unsigned int bt_OTPFault:1;            //Over temperature trip fault.
    unsigned int bt_EEPROMFault:1;         //EEPROM Fault.
    unsigned int bt_FCUnderVoltage:1;      //FCnderVoltage
    unsigned int bt_FCOverVoltage:1;       //FC_OverVoltage
    unsigned int Reserved12:1;             //Reserved bit.
} OLED_tzFAULT_BITS;



typedef union OLED_uFAULT_REG
{
    unsigned int all;
    OLED_tzFAULT_BITS bit;

} OLED_tuFAULT_REG;




/*==============================================================================
 Macros
==============================================================================*/

#define filler_zero 48
#define filler_space 32

/*==============================================================================
 Extern/Public Function Prototypes
==============================================================================*/

extern void OLED_fnUpdateDisplay(void);
extern void OLED_fnNodeCount(void);
extern void OLED_fnInit(void);
extern void OLED_fnPrintPage(unsigned int uiPgNo);
extern void OLED_fnWelcomePage(void);

/*==============================================================================
 Extern/Public Variables
==============================================================================*/

extern OLED_tuFAULT_REG tuFaultReg;
extern OLED_rzDISPLAY_DATA DisplayDataRx;
extern OLED_tzDISPLAY_DATA DisplayDataTx;

extern unsigned int update_page;
extern unsigned int uiEditVarCnt;     //node id

extern unsigned int uiserver_ip_oct1;
extern unsigned int uiserver_ip_oct2;
extern unsigned int uiserver_ip_oct3;
extern unsigned int uiserver_ip_oct4;

extern unsigned int uiclient_ip_oct1;
extern unsigned int uiclient_ip_oct2;
extern unsigned int uiclient_ip_oct3;
extern unsigned int uiclient_ip_oct4;


extern char intstr[8];

extern int uiVrefScaled;
extern int uiIrefScaled;


extern unsigned int uiToggleState;

extern unsigned int uiEditLeftEncoderCoarse;
extern unsigned int uiEditLeftEncoderFine;
extern unsigned int uiEditRightEncoderCoarse;
extern unsigned int uiEditRightEncoderFine;

//extern unsigned int teRxFltState;
extern unsigned int uiRxFltState;
extern unsigned int uiCommError;
extern unsigned int uiAutoScrollEn;

/*==============================================================================
 Extern/Public Constants
==============================================================================*/


#endif /* MODULES_OLED_INC_OLED_H_ */
