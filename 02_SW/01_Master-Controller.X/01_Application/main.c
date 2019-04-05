
#include <pic18.h>

#include "../mcc_generated_files/mcc.h"
#include "../02_MCAL/GPIO/GPIO.h"
#include "../02_MCAL/TIMER0/Timer0.h"
#include "TimeoutModule/TimeoutModule.h"
#include "../02_MCAL/UART/UART1.h"
#include "../02_MCAL/UART/UART2.h"
#include "../02_MCAL/RS232/RS232.h"
#include "../02_MCAL/SPI/SPI.h"

/*
                         Main application
 */
void main(void)
{
    // Initialize the device
    SYSTEM_Initialize();
    
    RS232_actualConfig.communicationBaudGenSpeed = HIGH_SPEED;
    RS232_actualConfig.communicationDesiredBaud = BAUD_19200;
    RS232_actualConfig.communicationUartMode = ASYNC_8BIT;
    RS232_actualConfig.communicationPolarity = NON_INVERTED;
    RS232_actualConfig.communicationStopBitMode = ONE_STOP_BIT;
    RS232_vInit(&RS232_actualConfig);
    SPI_vInit(MASTER_MODE);
    
    

    uint8_t i = 7;
    uint8_t j = 8;
    char a[11] = {'h','e','l','l','o',' ','t','h','e','r','e'};
    while (1)
    {

        SPI_uiExchangeXBytes(a,11);
    }
}








//#include <pic18.h>
//
//#include "../mcc_generated_files/mcc.h"
//#include "../02_MCAL/GPIO/GPIO.h"
////#include "../02_MCAL/TIMER0/Timer0.h"
//#include "TimeoutModule/TimeoutModule.h"
////#include "../02_MCAL/UART/UART1.h"
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
//    RS232_actualConfig.communicationBaudGenSpeed = HIGH_SPEED;
//    RS232_actualConfig.communicationDesiredBaud = BAUD_9600;
//    RS232_actualConfig.communicationUartMode = LIN_MASTER;
//    RS232_actualConfig.communicationPolarity = NON_INVERTED;
//    RS232_actualConfig.communicationStopBitMode = ONE_STOP_BIT;
//    RS232_vInit(&RS232_actualConfig);
//    
//    //LIN_vInit();
//
//    SPI_vInit(MASTER_MODE);
////    
////
////
//
//    uint8_t i = 0;
//    uint8_t j,k = 0;
//  //  U2P1L = 0x18;
//    char a[11] = {'h','e','l','l','o',' ','w','o','r','l','d'};
//    while (1)
//    {
//
//        for(i=0;i<11;i++)
//        {
//        
//            SPI_vMasterTransmit(a[i]); 
//            j = SPI_uiMasterReceive();
//            
//        __delay_ms(3);
//        }
//            
//        
//       
//
//    
//
//        //U2P1L = 0x21;
//  
//    
//
//    }
//}