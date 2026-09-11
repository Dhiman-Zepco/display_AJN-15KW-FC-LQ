/*
 * init.c
 * Created on: 30-May-2023
 * Author: Dhiman, Santhosh, Nilesh.
 */
/**
 * @brief INIT_fnSystemInit()
 * @param void
 * @return void
 *
 * 1. WatchDog reset.
 * 2. FRAM Controller Control Register.
 * 3. MSP430FR2153 Clock frequency setting.
 * 4. Port and Pin directions settings.
 * 5. Interrupt Pin configurations.
 * 6. LED's inside the tactile switches.
 * 7. Disable the GPIO power-on default high-impedance mode to activate previously configured port settings.
 * 8. Timer Configuration - TBCCR0 interrupt enabled, interrupt flag disabled.
 * 9. Configuring UART pins.
 * 10.UART Baud rate selection, modulation control, UART interrupt.
 * 11.General MSP Intrinsics (General Interrupt Enable).
 */

#include <msp430.h>

void INIT_fnSystemInit()
{
    WDTCTL = WDTPW | WDTHOLD;                          // Stop watch-dog timer.

    FRCTL0 = FRCTLPW | NWAITS_1;

    __bis_SR_register(SCG0);                  // disable FLL
    CSCTL3 |= SELREF__REFOCLK;               // Set REFO as FLL reference source
    CSCTL0 = 0;                                   // clear DCO and MOD registers
    CSCTL1 &= ~(DCORSEL_7);             // Clear DCO frequency select bits first
    CSCTL1 |= DCORSEL_5;                                      // Set DCO = 16MHz
    CSCTL2 = FLLD_0 + 487;                                  // DCOCLKDIV = 16MHz
    __delay_cycles(3);
    __bic_SR_register(SCG0);                                       // enable FLL
    while(CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1));                     // FLL locked

    CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK;

    P3DIR |= 0xFF;          // OLED Display Data Pins in Output Direction
    P5DIR |= 0x07;          // OLED Control Pins in Output Direction

    P3OUT |= 0xFF;          // OLED Display Data Pins in Output Direction.
    P5OUT |= 0x07;          // OLED Display Control Pins in Output Direction.


    P6DIR &= ~(0x37);       // Tactile Switches active low states set.(button 1 to 6)as
                            // per schematic of rev. 185-000000 REV1.

    P4DIR |= (0x0F);       //Button_LED-1,2,3,4.
    P2DIR |= (0x0C);       //Button_LED-5,6.

    P2DIR &= ~(0x30);       //ENC_1A Pulse & ENC_1B Pulse for left encoder.
    P4DIR &= ~(0xF0);       //ENC_1 Switch Enable , ENC_2A Pulse, ENC_2B Pulse & ENC_2 Switch Enable for right encoder.
    P2IN |= ~(0x30);         //Input low pulse for ENC_1A Pulse & ENC_1B Pulse for left encoder.
    P4IN |= ~(0xF0);         //Input low pulse for ENC_1 Switch Enable , ENC_2A Pulse, ENC_2B Pulse & ENC_2 Switch Enable for right encoder.



    P2REN |= (0x30);                      // P2 pull-up register enable
    P2IES |= (0x10);                      // P2.5 pulse, Low to High edge.
    P2IFG = 0;                            // Clear all P2 interrupt flags.
    P2IE |= (0x10);                       // P2 interrupt enabled

    P4REN |= 0xF0;                      // P4 pull-up register enable
    P4IES |= (0x20);                      // P4 Low to High edge.
    P4IFG = 0;                          // Clear all P4 interrupt flags
    P4IE |= (0x20);                       // P4 interrupt enabled.

    P6IN |= ~(0x3F);              // Tactile Switches in Input low Direction.

    P4OUT &= 0x00;                 //P4 LED's inside the tactile switches //
    P2OUT &= 0x00;                 //P2 LED inside the tactile switches.  //

    PM5CTL0 &= ~LOCKLPM5;           // Disable the GPIO power-on default high-impedance mode
                                    // to activate previously configured port settings.

    TB0CCTL0 |= CCIE;                // TBCCR0 interrupt enabled, interrupt flag disabled.
    TB0CCR0 = 16000;

    TB0CTL = TBSSEL__SMCLK | MC__UP;                          // SMCLK, Timer in "UP" mode.

                                                        // Configure UART pins
    P1SEL0 |= BIT6 | BIT7;                              // set 2-UART pin as UART function...
    UCA0CTLW0 |= UCSWRST;                                      // setting up UART Register.
    UCA0CTLW0 |= UCSSEL__SMCLK;                                // UART Clock selection.
    UCA0CTLW0 |= UCSPB_1;                                      // with stop-bits as 2.

    UCA0BR0 = 104;                                      // 16000000/16/9600------>Baud-rate.
    UCA0BR1 = 0x00;                                    //Baud-rate control registers, flags.
    UCA0MCTLW = 0x0110 | UCBRF_2 | UCOS16;             //eUSCI_Ax Modulation Control Word Register.

    // Initialize eUSCI, Clearing UCSWRST releases the eUSCI_A for operation.
    UCA0CTLW0 &= ~UCSWRST;
    UCA0IE |= UCRXIE;                                       // Enable USCI_A0 RX interrupt

    __bis_SR_register(GIE);                         //General Interrupt Enable.

}
