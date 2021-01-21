/***************************************************************************//**
* \file SCB_1_I2C.c
* \version 4.0
*
* \brief
*  This file provides the source code to the API for the SCB Component in
*  I2C mode.
*
* Note:
*
*******************************************************************************
* \copyright
* Copyright 2013-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "SCB_1_PVT.h"
#include "SCB_1_I2C_PVT.h"


/***************************************
*      I2C Private Vars
***************************************/

volatile uint8 SCB_1_state;  /* Current state of I2C FSM */

#if(SCB_1_SCB_MODE_UNCONFIG_CONST_CFG)

    /***************************************
    *  Configuration Structure Initialization
    ***************************************/

    /* Constant configuration of I2C */
    const SCB_1_I2C_INIT_STRUCT SCB_1_configI2C =
    {
        SCB_1_I2C_MODE,
        SCB_1_I2C_OVS_FACTOR_LOW,
        SCB_1_I2C_OVS_FACTOR_HIGH,
        SCB_1_I2C_MEDIAN_FILTER_ENABLE,
        SCB_1_I2C_SLAVE_ADDRESS,
        SCB_1_I2C_SLAVE_ADDRESS_MASK,
        SCB_1_I2C_ACCEPT_ADDRESS,
        SCB_1_I2C_WAKE_ENABLE,
        SCB_1_I2C_BYTE_MODE_ENABLE,
        SCB_1_I2C_DATA_RATE,
        SCB_1_I2C_ACCEPT_GENERAL_CALL,
    };

    /*******************************************************************************
    * Function Name: SCB_1_I2CInit
    ****************************************************************************//**
    *
    *
    *  Configures the SCB_1 for I2C operation.
    *
    *  This function is intended specifically to be used when the SCB_1 
    *  configuration is set to “Unconfigured SCB_1” in the customizer. 
    *  After initializing the SCB_1 in I2C mode using this function, 
    *  the component can be enabled using the SCB_1_Start() or 
    * SCB_1_Enable() function.
    *  This function uses a pointer to a structure that provides the configuration 
    *  settings. This structure contains the same information that would otherwise 
    *  be provided by the customizer settings.
    *
    *  \param config: pointer to a structure that contains the following list of 
    *   fields. These fields match the selections available in the customizer. 
    *   Refer to the customizer for further description of the settings.
    *
    *******************************************************************************/
    void SCB_1_I2CInit(const SCB_1_I2C_INIT_STRUCT *config)
    {
        uint32 medianFilter;
        uint32 locEnableWake;

        if(NULL == config)
        {
            CYASSERT(0u != 0u); /* Halt execution due to bad function parameter */
        }
        else
        {
            /* Configure pins */
            SCB_1_SetPins(SCB_1_SCB_MODE_I2C, SCB_1_DUMMY_PARAM,
                                     SCB_1_DUMMY_PARAM);

            /* Store internal configuration */
            SCB_1_scbMode       = (uint8) SCB_1_SCB_MODE_I2C;
            SCB_1_scbEnableWake = (uint8) config->enableWake;
            SCB_1_scbEnableIntr = (uint8) SCB_1_SCB_IRQ_INTERNAL;

            SCB_1_mode          = (uint8) config->mode;
            SCB_1_acceptAddr    = (uint8) config->acceptAddr;

        #if (SCB_1_CY_SCBIP_V0)
            /* Adjust SDA filter settings. Ticket ID#150521 */
            SCB_1_SET_I2C_CFG_SDA_FILT_TRIM(SCB_1_EC_AM_I2C_CFG_SDA_FILT_TRIM);
        #endif /* (SCB_1_CY_SCBIP_V0) */

            /* Adjust AF and DF filter settings. Ticket ID#176179 */
            if (((SCB_1_I2C_MODE_SLAVE != config->mode) &&
                 (config->dataRate <= SCB_1_I2C_DATA_RATE_FS_MODE_MAX)) ||
                 (SCB_1_I2C_MODE_SLAVE == config->mode))
            {
                /* AF = 1, DF = 0 */
                SCB_1_I2C_CFG_ANALOG_FITER_ENABLE;
                medianFilter = SCB_1_DIGITAL_FILTER_DISABLE;
            }
            else
            {
                /* AF = 0, DF = 1 */
                SCB_1_I2C_CFG_ANALOG_FITER_DISABLE;
                medianFilter = SCB_1_DIGITAL_FILTER_ENABLE;
            }

        #if (!SCB_1_CY_SCBIP_V0)
            locEnableWake = (SCB_1_I2C_MULTI_MASTER_SLAVE) ? (0u) : (config->enableWake);
        #else
            locEnableWake = config->enableWake;
        #endif /* (!SCB_1_CY_SCBIP_V0) */

            /* Configure I2C interface */
            SCB_1_CTRL_REG     = SCB_1_GET_CTRL_BYTE_MODE  (config->enableByteMode) |
                                            SCB_1_GET_CTRL_ADDR_ACCEPT(config->acceptAddr)     |
                                            SCB_1_GET_CTRL_EC_AM_MODE (locEnableWake);

            SCB_1_I2C_CTRL_REG = SCB_1_GET_I2C_CTRL_HIGH_PHASE_OVS(config->oversampleHigh) |
                    SCB_1_GET_I2C_CTRL_LOW_PHASE_OVS (config->oversampleLow)                          |
                    SCB_1_GET_I2C_CTRL_S_GENERAL_IGNORE((uint32)(0u == config->acceptGeneralAddr))    |
                    SCB_1_GET_I2C_CTRL_SL_MSTR_MODE  (config->mode);

            /* Configure RX direction */
            SCB_1_RX_CTRL_REG      = SCB_1_GET_RX_CTRL_MEDIAN(medianFilter) |
                                                SCB_1_I2C_RX_CTRL;
            SCB_1_RX_FIFO_CTRL_REG = SCB_1_CLEAR_REG;

            /* Set default address and mask */
            SCB_1_RX_MATCH_REG    = ((SCB_1_I2C_SLAVE) ?
                                                (SCB_1_GET_I2C_8BIT_ADDRESS(config->slaveAddr) |
                                                 SCB_1_GET_RX_MATCH_MASK(config->slaveAddrMask)) :
                                                (SCB_1_CLEAR_REG));


            /* Configure TX direction */
            SCB_1_TX_CTRL_REG      = SCB_1_I2C_TX_CTRL;
            SCB_1_TX_FIFO_CTRL_REG = SCB_1_CLEAR_REG;

            /* Configure interrupt with I2C handler but do not enable it */
            CyIntDisable    (SCB_1_ISR_NUMBER);
            CyIntSetPriority(SCB_1_ISR_NUMBER, SCB_1_ISR_PRIORITY);
            (void) CyIntSetVector(SCB_1_ISR_NUMBER, &SCB_1_I2C_ISR);

            /* Configure interrupt sources */
        #if(!SCB_1_CY_SCBIP_V1)
            SCB_1_INTR_SPI_EC_MASK_REG = SCB_1_NO_INTR_SOURCES;
        #endif /* (!SCB_1_CY_SCBIP_V1) */

            SCB_1_INTR_I2C_EC_MASK_REG = SCB_1_NO_INTR_SOURCES;
            SCB_1_INTR_RX_MASK_REG     = SCB_1_NO_INTR_SOURCES;
            SCB_1_INTR_TX_MASK_REG     = SCB_1_NO_INTR_SOURCES;

            SCB_1_INTR_SLAVE_MASK_REG  = ((SCB_1_I2C_SLAVE) ?
                            (SCB_1_GET_INTR_SLAVE_I2C_GENERAL(config->acceptGeneralAddr) |
                             SCB_1_I2C_INTR_SLAVE_MASK) : (SCB_1_CLEAR_REG));

            SCB_1_INTR_MASTER_MASK_REG = SCB_1_NO_INTR_SOURCES;

            /* Configure global variables */
            SCB_1_state = SCB_1_I2C_FSM_IDLE;

            /* Internal slave variables */
            SCB_1_slStatus        = 0u;
            SCB_1_slRdBufIndex    = 0u;
            SCB_1_slWrBufIndex    = 0u;
            SCB_1_slOverFlowCount = 0u;

            /* Internal master variables */
            SCB_1_mstrStatus     = 0u;
            SCB_1_mstrRdBufIndex = 0u;
            SCB_1_mstrWrBufIndex = 0u;
        }
    }

#else

    /*******************************************************************************
    * Function Name: SCB_1_I2CInit
    ****************************************************************************//**
    *
    *  Configures the SCB for the I2C operation.
    *
    *******************************************************************************/
    void SCB_1_I2CInit(void)
    {
    #if(SCB_1_CY_SCBIP_V0)
        /* Adjust SDA filter settings. Ticket ID#150521 */
        SCB_1_SET_I2C_CFG_SDA_FILT_TRIM(SCB_1_EC_AM_I2C_CFG_SDA_FILT_TRIM);
    #endif /* (SCB_1_CY_SCBIP_V0) */

        /* Adjust AF and DF filter settings. Ticket ID#176179 */
        SCB_1_I2C_CFG_ANALOG_FITER_ENABLE_ADJ;

        /* Configure I2C interface */
        SCB_1_CTRL_REG     = SCB_1_I2C_DEFAULT_CTRL;
        SCB_1_I2C_CTRL_REG = SCB_1_I2C_DEFAULT_I2C_CTRL;

        /* Configure RX direction */
        SCB_1_RX_CTRL_REG      = SCB_1_I2C_DEFAULT_RX_CTRL;
        SCB_1_RX_FIFO_CTRL_REG = SCB_1_I2C_DEFAULT_RX_FIFO_CTRL;

        /* Set default address and mask */
        SCB_1_RX_MATCH_REG     = SCB_1_I2C_DEFAULT_RX_MATCH;

        /* Configure TX direction */
        SCB_1_TX_CTRL_REG      = SCB_1_I2C_DEFAULT_TX_CTRL;
        SCB_1_TX_FIFO_CTRL_REG = SCB_1_I2C_DEFAULT_TX_FIFO_CTRL;

        /* Configure interrupt with I2C handler but do not enable it */
        CyIntDisable    (SCB_1_ISR_NUMBER);
        CyIntSetPriority(SCB_1_ISR_NUMBER, SCB_1_ISR_PRIORITY);
    #if(!SCB_1_I2C_EXTERN_INTR_HANDLER)
        (void) CyIntSetVector(SCB_1_ISR_NUMBER, &SCB_1_I2C_ISR);
    #endif /* (SCB_1_I2C_EXTERN_INTR_HANDLER) */

        /* Configure interrupt sources */
    #if(!SCB_1_CY_SCBIP_V1)
        SCB_1_INTR_SPI_EC_MASK_REG = SCB_1_I2C_DEFAULT_INTR_SPI_EC_MASK;
    #endif /* (!SCB_1_CY_SCBIP_V1) */

        SCB_1_INTR_I2C_EC_MASK_REG = SCB_1_I2C_DEFAULT_INTR_I2C_EC_MASK;
        SCB_1_INTR_SLAVE_MASK_REG  = SCB_1_I2C_DEFAULT_INTR_SLAVE_MASK;
        SCB_1_INTR_MASTER_MASK_REG = SCB_1_I2C_DEFAULT_INTR_MASTER_MASK;
        SCB_1_INTR_RX_MASK_REG     = SCB_1_I2C_DEFAULT_INTR_RX_MASK;
        SCB_1_INTR_TX_MASK_REG     = SCB_1_I2C_DEFAULT_INTR_TX_MASK;

        /* Configure global variables */
        SCB_1_state = SCB_1_I2C_FSM_IDLE;

    #if(SCB_1_I2C_SLAVE)
        /* Internal slave variable */
        SCB_1_slStatus        = 0u;
        SCB_1_slRdBufIndex    = 0u;
        SCB_1_slWrBufIndex    = 0u;
        SCB_1_slOverFlowCount = 0u;
    #endif /* (SCB_1_I2C_SLAVE) */

    #if(SCB_1_I2C_MASTER)
    /* Internal master variable */
        SCB_1_mstrStatus     = 0u;
        SCB_1_mstrRdBufIndex = 0u;
        SCB_1_mstrWrBufIndex = 0u;
    #endif /* (SCB_1_I2C_MASTER) */
    }
#endif /* (SCB_1_SCB_MODE_UNCONFIG_CONST_CFG) */


/*******************************************************************************
* Function Name: SCB_1_I2CStop
****************************************************************************//**
*
*  Resets the I2C FSM into the default state.
*
*******************************************************************************/
void SCB_1_I2CStop(void)
{
    /* Clear command registers because they keep assigned value after IP block was disabled */
    SCB_1_I2C_MASTER_CMD_REG = 0u;
    SCB_1_I2C_SLAVE_CMD_REG  = 0u;
    
    SCB_1_state = SCB_1_I2C_FSM_IDLE;
}


/*******************************************************************************
* Function Name: SCB_1_I2CFwBlockReset
****************************************************************************//**
*
* Resets the scb IP block and I2C into the known state.
*
*******************************************************************************/
void SCB_1_I2CFwBlockReset(void)
{
    /* Disable scb IP: stop respond to I2C traffic */
    SCB_1_CTRL_REG &= (uint32) ~SCB_1_CTRL_ENABLED;

    /* Clear command registers they are not cleared after scb IP is disabled */
    SCB_1_I2C_MASTER_CMD_REG = 0u;
    SCB_1_I2C_SLAVE_CMD_REG  = 0u;

    SCB_1_DISABLE_AUTO_DATA;

    SCB_1_SetTxInterruptMode(SCB_1_NO_INTR_SOURCES);
    SCB_1_SetRxInterruptMode(SCB_1_NO_INTR_SOURCES);
    
#if(SCB_1_CY_SCBIP_V0)
    /* Clear interrupt sources as they are not cleared after scb IP is disabled */
    SCB_1_ClearTxInterruptSource    (SCB_1_INTR_TX_ALL);
    SCB_1_ClearRxInterruptSource    (SCB_1_INTR_RX_ALL);
    SCB_1_ClearSlaveInterruptSource (SCB_1_INTR_SLAVE_ALL);
    SCB_1_ClearMasterInterruptSource(SCB_1_INTR_MASTER_ALL);
#endif /* (SCB_1_CY_SCBIP_V0) */

    SCB_1_state = SCB_1_I2C_FSM_IDLE;

    /* Enable scb IP: start respond to I2C traffic */
    SCB_1_CTRL_REG |= (uint32) SCB_1_CTRL_ENABLED;
}


#if(SCB_1_I2C_WAKE_ENABLE_CONST)
    /*******************************************************************************
    * Function Name: SCB_1_I2CSaveConfig
    ****************************************************************************//**
    *
    *  Enables SCB_1_INTR_I2C_EC_WAKE_UP interrupt source. This interrupt
    *  triggers on address match and wakes up device.
    *
    *******************************************************************************/
    void SCB_1_I2CSaveConfig(void)
    {
    #if (!SCB_1_CY_SCBIP_V0)
        #if (SCB_1_I2C_MULTI_MASTER_SLAVE_CONST && SCB_1_I2C_WAKE_ENABLE_CONST)
            /* Enable externally clocked address match if it was not enabled before.
            * This applicable only for Multi-Master-Slave. Ticket ID#192742 */
            if (0u == (SCB_1_CTRL_REG & SCB_1_CTRL_EC_AM_MODE))
            {
                /* Enable external address match logic */
                SCB_1_Stop();
                SCB_1_CTRL_REG |= SCB_1_CTRL_EC_AM_MODE;
                SCB_1_Enable();
            }
        #endif /* (SCB_1_I2C_MULTI_MASTER_SLAVE_CONST) */

        #if (SCB_1_SCB_CLK_INTERNAL)
            /* Disable clock to internal address match logic. Ticket ID#187931 */
            SCB_1_SCBCLK_Stop();
        #endif /* (SCB_1_SCB_CLK_INTERNAL) */
    #endif /* (!SCB_1_CY_SCBIP_V0) */

        SCB_1_SetI2CExtClkInterruptMode(SCB_1_INTR_I2C_EC_WAKE_UP);
    }


    /*******************************************************************************
    * Function Name: SCB_1_I2CRestoreConfig
    ****************************************************************************//**
    *
    *  Disables SCB_1_INTR_I2C_EC_WAKE_UP interrupt source. This interrupt
    *  triggers on address match and wakes up device.
    *
    *******************************************************************************/
    void SCB_1_I2CRestoreConfig(void)
    {
        /* Disable wakeup interrupt on address match */
        SCB_1_SetI2CExtClkInterruptMode(SCB_1_NO_INTR_SOURCES);

    #if (!SCB_1_CY_SCBIP_V0)
        #if (SCB_1_SCB_CLK_INTERNAL)
            /* Enable clock to internal address match logic. Ticket ID#187931 */
            SCB_1_SCBCLK_Start();
        #endif /* (SCB_1_SCB_CLK_INTERNAL) */
    #endif /* (!SCB_1_CY_SCBIP_V0) */
    }
#endif /* (SCB_1_I2C_WAKE_ENABLE_CONST) */


/* [] END OF FILE */
