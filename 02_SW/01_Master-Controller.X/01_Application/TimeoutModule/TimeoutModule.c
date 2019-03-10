///**
// * \file       TimeoutModule.c
// * \author     Iosub Mihai Alexandru
// * \brief      This module is used for timeouts.
// */
//
///*----------------------------------------------------------------------------*/
///*                                 Includes                                   */
///*----------------------------------------------------------------------------*/
//#include "TimeoutModule.h"
///*----------------------------------------------------------------------------*/
///*                               Local defines                                */
///*----------------------------------------------------------------------------*/
///* US = microseconds */
//#define US_CLK_SOURCE_VALUE         (0x03u)
//#define US_CLK_PRESCALER_POWER      (6u)
//#define MS_CLK_SOURCE_VALUE         (0x02u)
//#define MS_CLK_PRESCALER_POWER      (14u)
///*----------------------------------------------------------------------------*/
///*                              Local data types                              */
///*----------------------------------------------------------------------------*/
//
///*----------------------------------------------------------------------------*/
///*                             Global data at RAM                             */
///*----------------------------------------------------------------------------*/
//
///*----------------------------------------------------------------------------*/
///*                             Global data at ROM                             */
///*----------------------------------------------------------------------------*/
//
///*----------------------------------------------------------------------------*/
///*                             Local data at RAM                              */
///*----------------------------------------------------------------------------*/
//static TimeoutModule_State ModuleState;
//static uint8_t NumberOfOverflows;
//static Timer_Configuration timer_config;
///*----------------------------------------------------------------------------*/
///*                             Local data at ROM                              */
///*----------------------------------------------------------------------------*/
//
///*----------------------------------------------------------------------------*/
///*                       Declaration of local functions                       */
///*----------------------------------------------------------------------------*/
///**
// * \brief     This function puts the TimeoutModule on IDLE and stops TMR0
// * \param     None
// * \return    None 
// */
//void TimeoutModule_vStop(void);
///*----------------------------------------------------------------------------*/
///*                     Implementation of global functions                     */
//
///*----------------------------------------------------------------------------*/
//void TimeoutModule_vInit()
//{
//    Timer0_vInit();
//    NumberOfOverflows = 0;
//    ModuleState = TIMEOUT_IDLE;
//}
//
//TimeoutModule_State TimeoutModule_uiSetTimeout(TimeoutModule_MeasurementUnit unit, uint16_t timeAmount)
//{
//    TimeoutModule_State returnValue = TIMEOUT_ERROR;
//    if (TIMEOUT_IDLE == ModuleState)
//    {
//        if (TIMEOUT_uS == unit)
//        {
//            timer_config.clockSourceValue = US_CLK_SOURCE_VALUE; /* 64MHz */
//            timer_config.prescalerPower = US_CLK_PRESCALER_POWER; /* 2^6 => 1Mhz */
//            timer_config.postscalerValue = 1;
//
//        }
//        else if (TIMEOUT_mS == unit)
//        {
//            timer_config.clockSourceValue = MS_CLK_SOURCE_VALUE; /* 16MHz */
//            timer_config.prescalerPower = MS_CLK_PRESCALER_POWER; /* 2^14 => 976Hz */
//            timer_config.postscalerValue = 1; /* 976Hz (~1ms) */
//            NumberOfOverflows = 0;
//        }
//        timer_config.opMode = TIMER_16BIT_MODE;
//        timer_config.startValue = MASK_16BIT_MAX_VALUE - timeAmount;
//        Timer0_vStart(timer_config);
//        ModuleState = TIMEOUT_NOT_REACHED;
//        returnValue = TIMEOUT_NOT_REACHED;
//    }
//    else if (TIMEOUT_NOT_REACHED == ModuleState)
//    {
//        if (TIMEOUT_uS == unit)
//        {
//            if (true == Timer0_bWasOverflow())
//            {
//                returnValue = TIMEOUT_REACHED;
//                ModuleState = TIMEOUT_REACHED;
//            }
//        }
//        else if (TIMEOUT_mS == unit)
//        {
//            if (true == Timer0_bWasOverflow())
//            {
////                NumberOfOverflows++;
////                if (NumberOfOverflows >= timer_config.postscalerValue)
////                {
//                    ModuleState = TIMEOUT_REACHED;
//                    returnValue = TIMEOUT_REACHED;
////                }
//            }
//        }
//
//    }
//    else if (TIMEOUT_REACHED == ModuleState)
//    {
//        returnValue = TIMEOUT_IDLE;
//        TimeoutModule_vStop();
//    }
//    return returnValue;
//}
//
//TimeoutModule_State TimeoutModule_uiCancelTimeout(void)
//{
//    TimeoutModule_vStop();
//    return ModuleState;
//}
///*----------------------------------------------------------------------------*/
///*                     Implementation of local functions                      */
//
///*----------------------------------------------------------------------------*/
//void TimeoutModule_vStop(void)
//{
//    ModuleState = TIMEOUT_IDLE;
//    NumberOfOverflows = 0;
//    Timer0_vStop();
//}