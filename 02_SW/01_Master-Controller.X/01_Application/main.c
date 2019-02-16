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

#include <pic18.h>

#include "../mcc_generated_files/mcc.h"
#include "../02_MCAL/GPIO/GPIO.h"
#include "../02_MCAL/TIMER0/Timer0.h"
#include "TimeoutModule/TimeoutModule.h"

/*
                         Main application
 */
void main(void)
{
    // Initialize the device
    SYSTEM_Initialize();
    // TimeoutModule_vInit();
    // I2C_vInit();

    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global Interrupts
    // Use the following macros to:

    // Enable the Global Interrupts
    //INTERRUPT_GlobalInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();  
    //i2c1_writeNBytesRegister(0x08, 0x61);
    //I2C_vWriteDataOnBus(0x08, 0x61);
    
    GPIO_vSetPinDirection(0xA4, GPIO_OUTPUT_PIN);
    GPIO_vSetPinDirection(0xA5, GPIO_INPUT_PIN);
    GPIO_vSetPinLevel(0xA4, STD_HIGH);
    GPIO_vSetPinDirection(0xB3, GPIO_OUTPUT_PIN);
    RB3PPS = 0x25;
    TimeoutModule_vInit();
    uint8_t flag = 0;
    STD_LogicLevel state = STD_LOW;
    while (1)
    {
        if ((GPIO_ui8GetPinLevel(0xA5) == STD_HIGH) || (TimeoutModule_uiSetTimeout(TIMEOUT_mS, 6500) == TIMEOUT_REACHED))
        {
            if(state == STD_LOW)
            {
                state = STD_HIGH;
            }
            else if(state == STD_HIGH)
            {
                state = STD_LOW;
            }
            GPIO_vSetPinLevel(0xA4, state);
            flag = 1;
        }
        //i2c1_write1ByteRegister(0x08,0x35,0x36);
        /*uint8_t initial = TimeoutModule_ui16GetValue();
        while((1) || TimeoutModule_isTimeout_us(initial, 30));
        RC1 = ~RC1;*/
        //I2C_vMainFunction();
        //I2C_vWriteDataOnBus(0x08, 0x36);
        //I2C_vMainFunction();
        //I2C_ui8ReadDataFromBus(0x08);

    }
}
/**
 End of File
 */