
#include <pic18.h>
#include "../mcc_generated_files/mcc.h"
#include "../03_Common/types.h"
#include "../02_MCAL/GPIO/GPIO.h"
//#include "../02_MCAL/TIMER0/Timer0.h"
#include "TimeoutModule/TimeoutModule.h"
#include "../02_MCAL/UART/UART1.h"
#include "../02_MCAL/UART/UART2.h"
#include "../02_MCAL/RS232/RS232.h"
#include "../02_MCAL/LIN/LIN.h"
#include "../02_MCAL/SPI/SPI.h"

/*
                         Main application
 */

void INTERRUPT_Initialize(void)
{
    INTCON0bits.GIEH = 1; // Enable high priority interrupts
    INTCON0bits.GIEL = 1; // Enable low priority interrupts
    INTCON0bits.IPEN = 1; // Enable interrupt priority
}




void main(void)
{
    // Initialize the device

    SYSTEM_Initialize();
    INTERRUPT_Initialize();

    LIN_actualConfig.configBaudGeneratorSpeed = HIGH_SPEED;
    LIN_actualConfig.configBaudValue = BAUD_19200;
    LIN_actualConfig.configUartMode = LIN_MASTER;
    LIN_actualConfig.configTransmitPolarity = NON_INVERTED;
    LIN_actualConfig.configStopBitMode = ONE_STOP_BIT;
    LIN_actualConfig.config_ChecksumMode = ENHANCED;
    LIN_vInit(&LIN_actualConfig);

    uint8_t i = 5;
    uint8_t j, k = 0;
    uint8_t a[] = {0x43, 0xFE, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    uint8_t b[] = {0x7, 0x4, 0x3, 0x04, 0x05, 0x06, 0x07, 0x08};
    GPIO_vSetPinDirection(0xA2, GPIO_OUTPUT_PIN);
    GPIO_vSetPinLevel(0xA2, STD_HIGH);
    

    while (1)
    {
        //LIN_vTransmit(0x01,2,a);
        LIN_vTransmit(49,1,b);
        //__delay_ms(1);
    }
}































//#include <pic18.h>
//
//#include "../mcc_generated_files/mcc.h"
//#include "../02_MCAL/GPIO/GPIO.h"
////#include "../02_MCAL/TIMER0/Timer0.h"
//#include "TimeoutModule/TimeoutModule.h"
//#include "../02_MCAL/UART/UART1.h"
//#include "../02_MCAL/UART/UART2.h"    
//#include "../02_MCAL/RS232/RS232.h"
//#include "../02_MCAL/LIN/LIN.h"
//#include "../02_MCAL/SPI/SPI.h"
//
///*
//                         Main application
// */
//void main(void)
//{
//    // Initialize the device
//    SYSTEM_Initialize();
//
////    RS232_actualConfig.communicationBaudGenSpeed = HIGH_SPEED;
////    RS232_actualConfig.communicationDesiredBaud = BAUD_9600;
////    RS232_actualConfig.communicationUartMode = LIN_SLAVE;
////    RS232_actualConfig.communicationPolarity = NON_INVERTED;
////    RS232_actualConfig.communicationStopBitMode = ONE_STOP_BIT;
////    RS232_vInit(&RS232_actualConfig);
//    
//    LIN_vInit(LIN_SLAVE);
//
////    SPI_vInit(MASTER_MODE);
////    
////
////
//
//    uint8_t i = 0;
//    uint8_t j,k = 0;
//    
//   
//    while (1)
//    {
//        U2P1L = 0x06;
////        U2P2L = 0x01;
////        U2P3L = 0x01;
//
//        U2TXB =0x03;
//
//
//        
//        //i = U2P1L;
//
//    
//
//    }
//}

































//#include <pic18.h>
//
//#include "../mcc_generated_files/mcc.h"
//#include "../02_MCAL/GPIO/GPIO.h"
//#include "../02_MCAL/TIMER0/Timer0.h"
//#include "TimeoutModule/TimeoutModule.h"
//#include "../02_MCAL/UART/UART1.h"
//#include "../02_MCAL/UART/UART2.h"
//#include "../02_MCAL/RS232/RS232.h"
//#include "../02_MCAL/SPI/SPI.h"
//
///*
//                         Main application
// */
//void main(void)
//{
//    // Initialize the device
//    SYSTEM_Initialize();
//    
//    RS232_actualConfig.communicationBaudGenSpeed = HIGH_SPEED;
//    RS232_actualConfig.communicationDesiredBaud = BAUD_19200;
//    RS232_actualConfig.communicationUartMode = ASYNC_8BIT;
//    RS232_actualConfig.communicationPolarity = NON_INVERTED;
//    RS232_actualConfig.communicationStopBitMode = ONE_STOP_BIT;
//    RS232_vInit(&RS232_actualConfig);
//    SPI_vInit(SLAVE_MODE);
//    uint8_t state  = 0;
//    GPIO_vSetPinDirection(0xA0,GPIO_OUTPUT_PIN);
//    
//    
//    
//
//    uint8_t i = 7;
//    uint8_t j = 8;
//    char a[5] = {'s','a','l','u','t'};
//    while (1)
//    {
//
//        state = SPI_uiExchangeXBytes(a,1);
//        if(state == 0)
//        {
//            GPIO_vSetPinLevel(0xA0, STD_HIGH);
//        }else if(state == 1)
//        {
//            GPIO_vSetPinLevel(0xA0,STD_LOW);
//        }
//        else
//        {
//            GPIO_vSetPinLevel(0xA0, STD_HIGH);
//            __delay_ms(300);
//            GPIO_vSetPinLevel(0xA0, STD_LOW);
//            __delay_ms(300);
//        }
//        
//    }
//}