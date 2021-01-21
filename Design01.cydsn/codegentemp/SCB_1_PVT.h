/***************************************************************************//**
* \file .h
* \version 4.0
*
* \brief
*  This private file provides constants and parameter values for the
*  SCB Component.
*  Please do not use this file or its content in your project.
*
* Note:
*
********************************************************************************
* \copyright
* Copyright 2013-2017, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_SCB_PVT_SCB_1_H)
#define CY_SCB_PVT_SCB_1_H

#include "SCB_1.h"


/***************************************
*     Private Function Prototypes
***************************************/

/* APIs to service INTR_I2C_EC register */
#define SCB_1_SetI2CExtClkInterruptMode(interruptMask) SCB_1_WRITE_INTR_I2C_EC_MASK(interruptMask)
#define SCB_1_ClearI2CExtClkInterruptSource(interruptMask) SCB_1_CLEAR_INTR_I2C_EC(interruptMask)
#define SCB_1_GetI2CExtClkInterruptSource()                (SCB_1_INTR_I2C_EC_REG)
#define SCB_1_GetI2CExtClkInterruptMode()                  (SCB_1_INTR_I2C_EC_MASK_REG)
#define SCB_1_GetI2CExtClkInterruptSourceMasked()          (SCB_1_INTR_I2C_EC_MASKED_REG)

#if (!SCB_1_CY_SCBIP_V1)
    /* APIs to service INTR_SPI_EC register */
    #define SCB_1_SetSpiExtClkInterruptMode(interruptMask) \
                                                                SCB_1_WRITE_INTR_SPI_EC_MASK(interruptMask)
    #define SCB_1_ClearSpiExtClkInterruptSource(interruptMask) \
                                                                SCB_1_CLEAR_INTR_SPI_EC(interruptMask)
    #define SCB_1_GetExtSpiClkInterruptSource()                 (SCB_1_INTR_SPI_EC_REG)
    #define SCB_1_GetExtSpiClkInterruptMode()                   (SCB_1_INTR_SPI_EC_MASK_REG)
    #define SCB_1_GetExtSpiClkInterruptSourceMasked()           (SCB_1_INTR_SPI_EC_MASKED_REG)
#endif /* (!SCB_1_CY_SCBIP_V1) */

#if(SCB_1_SCB_MODE_UNCONFIG_CONST_CFG)
    extern void SCB_1_SetPins(uint32 mode, uint32 subMode, uint32 uartEnableMask);
#endif /* (SCB_1_SCB_MODE_UNCONFIG_CONST_CFG) */


/***************************************
*     Vars with External Linkage
***************************************/

#if (SCB_1_SCB_IRQ_INTERNAL)
#if !defined (CY_REMOVE_SCB_1_CUSTOM_INTR_HANDLER)
    extern cyisraddress SCB_1_customIntrHandler;
#endif /* !defined (CY_REMOVE_SCB_1_CUSTOM_INTR_HANDLER) */
#endif /* (SCB_1_SCB_IRQ_INTERNAL) */

extern SCB_1_BACKUP_STRUCT SCB_1_backup;

#if(SCB_1_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Common configuration variables */
    extern uint8 SCB_1_scbMode;
    extern uint8 SCB_1_scbEnableWake;
    extern uint8 SCB_1_scbEnableIntr;

    /* I2C configuration variables */
    extern uint8 SCB_1_mode;
    extern uint8 SCB_1_acceptAddr;

    /* SPI/UART configuration variables */
    extern volatile uint8 * SCB_1_rxBuffer;
    extern uint8   SCB_1_rxDataBits;
    extern uint32  SCB_1_rxBufferSize;

    extern volatile uint8 * SCB_1_txBuffer;
    extern uint8   SCB_1_txDataBits;
    extern uint32  SCB_1_txBufferSize;

    /* EZI2C configuration variables */
    extern uint8 SCB_1_numberOfAddr;
    extern uint8 SCB_1_subAddrSize;
#endif /* (SCB_1_SCB_MODE_UNCONFIG_CONST_CFG) */

#if (! (SCB_1_SCB_MODE_I2C_CONST_CFG || \
        SCB_1_SCB_MODE_EZI2C_CONST_CFG))
    extern uint16 SCB_1_IntrTxMask;
#endif /* (! (SCB_1_SCB_MODE_I2C_CONST_CFG || \
              SCB_1_SCB_MODE_EZI2C_CONST_CFG)) */


/***************************************
*        Conditional Macro
****************************************/

#if(SCB_1_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Defines run time operation mode */
    #define SCB_1_SCB_MODE_I2C_RUNTM_CFG     (SCB_1_SCB_MODE_I2C      == SCB_1_scbMode)
    #define SCB_1_SCB_MODE_SPI_RUNTM_CFG     (SCB_1_SCB_MODE_SPI      == SCB_1_scbMode)
    #define SCB_1_SCB_MODE_UART_RUNTM_CFG    (SCB_1_SCB_MODE_UART     == SCB_1_scbMode)
    #define SCB_1_SCB_MODE_EZI2C_RUNTM_CFG   (SCB_1_SCB_MODE_EZI2C    == SCB_1_scbMode)
    #define SCB_1_SCB_MODE_UNCONFIG_RUNTM_CFG \
                                                        (SCB_1_SCB_MODE_UNCONFIG == SCB_1_scbMode)

    /* Defines wakeup enable */
    #define SCB_1_SCB_WAKE_ENABLE_CHECK       (0u != SCB_1_scbEnableWake)
#endif /* (SCB_1_SCB_MODE_UNCONFIG_CONST_CFG) */

/* Defines maximum number of SCB pins */
#if (!SCB_1_CY_SCBIP_V1)
    #define SCB_1_SCB_PINS_NUMBER    (7u)
#else
    #define SCB_1_SCB_PINS_NUMBER    (2u)
#endif /* (!SCB_1_CY_SCBIP_V1) */

#endif /* (CY_SCB_PVT_SCB_1_H) */


/* [] END OF FILE */
