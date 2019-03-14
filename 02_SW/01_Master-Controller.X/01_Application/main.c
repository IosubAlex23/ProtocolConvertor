/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.65.2
        Device            :  PIC18F26K83
        Driver Version    :  2.00
 */

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
 */
#include "../mcc_generated_files/mcc.h"
#include "../02_MCAL/GPIO/GPIO.h"
#include "../02_MCAL/TIMER0/Timer0.h"
#include "TimeoutModule/TimeoutModule.h"
#include "../02_MCAL/I2C/I2C.h"
#include "../02_MCAL/CAN/CAN.h"

/*
                         Main application
 */
void main(void)
{
    // Initialize the device
    SYSTEM_Initialize();
    TimeoutModule_vInit();
    I2C_vInit();
    CAN_vInit();

    uint8_t cnt;

    //    I2C_vMasterRead(0x08, 0x35, 1, &cnt);
    //    __delay_us(400);
    //    I2C_vMasterTransmit(0x08, 0x36, cnt);

    //    GPIO_vSetPinDirection(0xB3, GPIO_OUTPUT_PIN);
    //    RB3PPS = 0x25;
    //    uint8_t flag = 0;
    //    STD_LogicLevel state = STD_LOW;
    //    I2C_vJoinAsSlave(0x08);
    uint8_t state[8] = "Sal Alex";
    CAN_Frame frame;
    
    CAN_vFrameSetData(&frame, state, 8);
    frame.Frame_Identifier = 0x64;
    frame.Frame_RTR = false;


    CAN_vTransmitFrame(frame);
    while (1)
    {
       /* if(TXB0CON == 0x18)
        {
            CANCON |= 0x10;
        }*/
        //        val = I2C_vSlaveMainFunction();
        //        if ((0x10 > val) && val != 0x00)
        //        {
        //            GPIO_vSetPinLevel(0xA4, STD_HIGH);
        //        }
        //        else
        //        {
        //            GPIO_vSetPinLevel(0xA4, STD_LOW);
        //        }
        //        if ((GPIO_ui8GetPinLevel(0xA4) == STD_HIGH) || (TimeoutModule_uiSetTimeout(TIMEOUT_uS, 600) == TIMEOUT_REACHED))
        //        {
        //            if (state == STD_LOW)
        //            {
        //                state = STD_HIGH;
        //            }
        //            else if (state == STD_HIGH)
        //            {
        //                state = STD_LOW;
        //            }
        //            GPIO_vSetPinLevel(0xA5, state);
        //
        //        }
    }
}

/**
 End of File
 */