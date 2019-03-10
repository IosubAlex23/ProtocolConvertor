
#include <pic18.h>

#include "../mcc_generated_files/mcc.h"
#include "../02_MCAL/GPIO/GPIO.h"
#include "../02_MCAL/TIMER0/Timer0.h"
#include "TimeoutModule/TimeoutModule.h"
//#include "../02_MCAL/UART/UART1.h"
#include "../02_MCAL/UART/UART2.h"
#include "../02_MCAL/RS232/RS232.h"

/*
                         Main application
 */
void main(void)
{
    // Initialize the device
    SYSTEM_Initialize();
    
    RS232_actualConfig.communicationBaudGenSpeed = HIGH_SPEED;
    RS232_actualConfig.communicationDesiredBaud = BAUD_9600;
    RS232_actualConfig.communicationUartMode = ASYNC_8BIT;
    RS232_actualConfig.communicationPolarity = NON_INVERTED;
    RS232_actualConfig.communicationStopBitMode = ONE_STOP_BIT;
    RS232_vInit(&RS232_actualConfig);
    
    UART2_vInit();
    
    

    uint8_t i = 7;
    uint8_t j = 8;

    while (1)
    {
        UART2_uiTransmitter(i);
        UART2_uiTransmitter('\r');
        RS232_vSendData(j);
        RS232_vSendData('\r');
        if(STD_HIGH == RS232_bReceiveDataReady())
        {
            i = RS232_uiReceiveData();
        }
        if(STD_HIGH == UART2_bRX_Ready())
        {
            j = UART2_uiReception();
        }        
    }
}
/**
 End of File
 */