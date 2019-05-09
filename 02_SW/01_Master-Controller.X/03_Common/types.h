/* 
 * File:   types.h
 * Author: Alex
 *
 * Created on February 15, 2019, 8:25 PM
 */

#ifndef TYPES_H
#define	TYPES_H

/*----------------------------------------------------------------------------*/
/*                                  Includes                                  */
/*----------------------------------------------------------------------------*/
#include "../mcc_generated_files/mcc.h"
/*----------------------------------------------------------------------------*/
/*                             Defines and macros                             */
/*----------------------------------------------------------------------------*/
/* This is a mask used to clear the MS 4 bites of a byte */
#define MASK_4MSB_CLEAR                                  (0x0F)
#define MASK_8BIT_HALF                                   (4u)

#define MASK_16BIT_HALF                                  (8u)
#define MASK_8MSB_CLEAR                                  (0x00FF)

#define MASK_FOR_CLEARING_BIT(bitPos)                    (~(1u << bitPos))
#define MASK_FOR_SETTING_BIT(bitPos)                     (1u << bitPos)
/* This returns the MS 4 bites of a byte */
#define MASK_8BIT_GET_MSB_HALF(x)                       (x >> MASK_8BIT_HALF)
/* This returns the LS 4 bites of a byte */
#define MASK_8BIT_GET_LSB_HALF(x)                       (x & MASK_4MSB_CLEAR)
#define MASK_8BIT_CLEAR_BIT(reg, bitPosition)           (reg = reg & MASK_FOR_CLEARING_BIT(bitPosition))
#define MASK_8BIT_SET_BIT(reg, bitPosition)             (reg = reg | MASK_FOR_SETTING_BIT(bitPosition))
#define MASK_8BIT_GET_BIT(reg, bitPosition)             ((reg & MASK_FOR_SETTING_BIT(bitPosition)) >> bitPosition)

/* This returns the MS 8 bites of a byte */
#define MASK_16BIT_GET_MSB_HALF(x)                       (x >> MASK_16BIT_HALF)
/* This returns the LS 8 bites of a byte */
#define MASK_16BIT_GET_LSB_HALF(x)                       (x & MASK_8MSB_CLEAR)

#define MASK_16BIT_SET_MSB_HALF(reg, x)                  (reg = (x << MASK_16BIT_HALF) | MASK_16BIT_GET_LSB_HALF(reg))     

#define MASK_16BIT_MAX_VALUE                                 (65535u)
#define MASK_16BIT_MAX_COUNTS                                (65536u)

#define REGISTER_NUMBER_OF_BITS                           (8u)

#define RESET_VALUE                                       (0x00)
/*----------------------------------------------------------------------------*/
/*                                 Data types                                 */

/*----------------------------------------------------------------------------*/

typedef enum {
    STD_LOW = 0u,
    STD_HIGH,
    STD_UNKNOWN,
} STD_LogicLevel;

typedef enum {
    APP_PROTOCOL_CAN = 0u,
    APP_PROTOCOL_I2C,
    APP_PROTOCOL_LIN,
    APP_PROTOCOL_RS232,
    APP_PROTOCOL_UNKNOWN,
} MainAplication_Protocol;

/*----------------------------------------------------------------------------*/
/*                 External declaration of global RAM-Variables               */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                 External declaration of global ROM-Variables               */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                  External declaration of global functions                  */
/*----------------------------------------------------------------------------*/
/**
 * \brief     This function [...];
 * \param     None
 * \return    None 
 */

#endif	/* TYPES_H */

