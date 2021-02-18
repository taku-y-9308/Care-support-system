/*******************************************************************************
* File Name: CapSense_1_SMS.c
* Version 2.60
*
* Description:
*  This file provides the source code of wrapper between CapSense CSD component
*  and Auto Tuning library.
*
* Note:
*
********************************************************************************
* Copyright 2013-2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "CapSense_1_SMS.h"
#include "CapSense_1_PVT.h"

#include "cytypes.h"

#if (CapSense_1_TUNING_METHOD == CapSense_1__TUNING_AUTO)


/*  Real Charge Divider values */
#if ((0u != CapSense_1_IS_M0S8PERI_BLOCK) && (0u == CY_PSOC4_4000))
    /*  Dividers are not chained */
    #if (CYDEV_BCLK__HFCLK__MHZ > 24u)
        const uint8 CapSense_1_prescalerTable[CapSense_1_PRESCALERS_TBL_SIZE] = {
            4u,     4u,     4u,    8u,    8u,    8u,    8u,    16u,    16u,    16u,    16u,    16u,    16u,    16u,    16u,    16u
        };
    #elif (CYDEV_BCLK__HFCLK__MHZ >12u)
        const uint8 CapSense_1_prescalerTable[CapSense_1_PRESCALERS_TBL_SIZE] = {
            2u,     2u,     2u,    4u,    4u,    4u,    4u,    8u,    8u,    8u,    8u,    8u,    8u,    8u,    8u,    8u
        };
    #else   /* (CYDEV_BCLK__HFCLK__MHZ > 12u) */
        const uint8 CapSense_1_prescalerTable[CapSense_1_PRESCALERS_TBL_SIZE] = {
            1u,     1u,     1u,    2u,    2u,    2u,    2u,    4u,    4u,    4u,    4u,    4u,    4u,    4u,    4u,    4u
        };
    #endif /* (CYDEV_BCLK__HFCLK__MHZ > 24u) */

#elif (0u != CapSense_1_IS_M0S8PERI_BLOCK)
    /*  Dividers are not chained (PSoC 4000) */
    #if (CYDEV_BCLK__HFCLK__MHZ >= 12u)
        const uint8 CapSense_1_prescalerTable[CapSense_1_PRESCALERS_TBL_SIZE] = {
            1u,     1u,     1u,    2u,    2u,    2u,    2u,    4u,    4u,    4u,    4u,    4u,    4u,    4u,    4u,    4u
        };

    #elif (CYDEV_BCLK__HFCLK__MHZ >= 6u)
        const uint8 CapSense_1_prescalerTable[CapSense_1_PRESCALERS_TBL_SIZE] = {
            1u,     1u,     1u,    1u,    1u,    1u,    1u,    2u,    2u,    2u,    2u,    2u,    2u,    2u,    2u,    2u
        };
    #else
        const uint8 CapSense_1_prescalerTable[CapSense_1_PRESCALERS_TBL_SIZE] = {
            1u,     1u,     1u,    1u,    1u,    1u,    1u,    1u,    1u,    1u,    1u,    1u,    1u,    1u,    1u,    1u
        };
    #endif /* (CYDEV_BCLK__HFCLK__MHZ > 12u) */

#else
    /*  Dividers are chained (PSoC 4100, PSoC 4200) */
    #if (CYDEV_BCLK__HFCLK__MHZ > 24u)
        const uint8 CapSense_1_prescalerTable[CapSense_1_PRESCALERS_TBL_SIZE] = {
            2u,     2u,     2u,    4u,    4u,    4u,    4u,    8u,    8u,    8u,    8u,    8u,    8u,    8u,    8u,    8u
        };

    #elif (CYDEV_BCLK__HFCLK__MHZ >12u)
        const uint8 CapSense_1_prescalerTable[CapSense_1_PRESCALERS_TBL_SIZE] = {
            1u,     1u,     1u,    2u,    2u,    2u,    2u,    4u,    4u,    4u,    4u,    4u,    4u,    4u,    4u,    4u
        };
    #else   /* (CYDEV_BCLK__HFCLK__MHZ > 12u) */
        const uint8 CapSense_1_prescalerTable[CapSense_1_PRESCALERS_TBL_SIZE] = {
            1u,     1u,     1u,    1u,    1u,    1u,    1u,    2u,    2u,    2u,    2u,    2u,    2u,    2u,    2u,    2u
        };
    #endif /* (CYDEV_BCLK__HFCLK__MHZ > 24u) */
#endif /* ((0u != CapSense_1_IS_M0S8PERI_BLOCK) && (0u == CY_PSOC4_4000)) */


uint8 CapSense_1_noiseEnvelopeTbl[CapSense_1_TOTAL_SENSOR_COUNT];
uint8 CapSense_1_runningDifferenceTbl[CapSense_1_TOTAL_SENSOR_COUNT];
uint8 CapSense_1_signRegisterTbl[CapSense_1_TOTAL_SENSOR_COUNT];
uint16 CapSense_1_sampleMinTbl[CapSense_1_TOTAL_SENSOR_COUNT];
uint16 CapSense_1_sampleMaxTbl[CapSense_1_TOTAL_SENSOR_COUNT];
uint16 CapSense_1_previousSampleTbl[CapSense_1_TOTAL_SENSOR_COUNT];
uint8 CapSense_1_kValueTbl[CapSense_1_TOTAL_SENSOR_COUNT];

uint8 CapSense_1_sensorSensitivity[] = {
    2u, 
};

CapSense_1_CONFIG_TYPE_P4_v2_60 CapSense_1_config;
CapSense_1_CONFIG_TYPE_POINTERS_P4_v2_60 const CapSense_1_configPointers = {
    CapSense_1_modulationIDAC,
    CapSense_1_compensationIDAC,
    CapSense_1_sensorSensitivity,
    CapSense_1_senseClkDividerVal,
    CapSense_1_sampleClkDividerVal,
    CapSense_1_widgetNumber,
    CapSense_1_widgetResolution,
    CapSense_1_noiseEnvelopeTbl,
    CapSense_1_runningDifferenceTbl,
    CapSense_1_signRegisterTbl,
    CapSense_1_sampleMinTbl,
    CapSense_1_sampleMaxTbl,
    CapSense_1_previousSampleTbl,
    CapSense_1_kValueTbl,
    CapSense_1_scanSpeedTbl,
    CapSense_1_prescalerTable,
    (const uint8  *)CapSense_1_rawDataIndex,
    (const uint8  *)CapSense_1_numberOfSensors,
    &CapSense_1_GetSensorRaw,
    &CapSense_1_PreScan,
    &CapSense_1_ReadSensorRaw,
    &CapSense_1_GetBitValue,
    &CapSense_1_SetBitValue
};


/*******************************************************************************
* Function Name: CapSense_1_AutoTune
********************************************************************************
*
* Summary:
*  Provides the tuning procedure for all sensors.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global Variables:
*   CapSense_1_config: the structure which contains configuration parameters
*   CapSense_1_senseClkDividerVal[]: used to store the Analog Switch
*   dividers for each sensor.
*   CapSense_1_senseClkDividerVal[]: used to store the
*    Analog Switch divider for all sensors.
*   CapSense_1_prescalersTuningDone - used to notify the Tuner GUI that
*   the pre-scalers tuning  is done.
*
* Side Effects:
*  None
*
*******************************************************************************/
void CapSense_1_AutoTune(void)
{
    uint32 curSensor;
    uint32 widget;

    CapSense_1_config.totalScanslotsNum = CapSense_1_TOTAL_SCANSLOT_COUNT;
    CapSense_1_config.totalWidgetsNum = CapSense_1_END_OF_WIDGETS_INDEX;
    CapSense_1_config.totalSensorsNum = CapSense_1_TOTAL_SENSOR_COUNT;

    CapSense_1_config.hfclkFreq_MHz = CYDEV_BCLK__HFCLK__MHZ;
    CapSense_1_config.sensetivitySeed = CapSense_1_SENSETIVITY_FACTOR;

    CapSense_1_config.pointers = &CapSense_1_configPointers;

    CapSense_1_SetAnalogSwitchesSrcDirect();

    for(widget = 0u; widget < CapSense_1_config.totalWidgetsNum; widget++)
    {
        CapSense_1_widgetResolution[widget] = CapSense_1_CALIBRATION_RESOLUTION;
    }


    for(curSensor = 0u; curSensor < CapSense_1_TOTAL_SCANSLOT_COUNT; curSensor++)
    {
        CapSense_1_senseClkDividerVal[curSensor] = CapSense_1_CALIBRATION_ASD;
        CapSense_1_sampleClkDividerVal[curSensor] = CapSense_1_CALIBRATION_MD;
    }

    CapSense_1_DisableBaselineIDAC();

    CalibrateSensors_P4_v2_60(&CapSense_1_config, CapSense_1_CAL_RAW_COUNT);

    #if(0u != CapSense_1_TOTAL_CENTROIDS_COUNT)
        CapSense_1_NormalizeWidgets(CapSense_1_END_OF_WIDGETS_INDEX, CapSense_1_modulationIDAC);
    #endif /* (0u != CapSense_1_TOTAL_CENTROIDS_COUNT)  */

    TunePrescalers_P4_v2_60(&CapSense_1_config);

    #if(CapSense_1_PRS_OPTIONS != CapSense_1__PRS_NONE)
        CapSense_1_prescalersTuningDone = 1u;
    #endif /* (CapSense_1_PRS_OPTIONS == CapSense_1__PRS_NONE) */

    CalibrateSensors_P4_v2_60(&CapSense_1_config, CapSense_1_CAL_RAW_COUNT);
    #if(0u != CapSense_1_TOTAL_CENTROIDS_COUNT)
        CapSense_1_NormalizeWidgets(CapSense_1_END_OF_WIDGETS_INDEX, CapSense_1_modulationIDAC);
    #endif /* (0u != CapSense_1_TOTAL_CENTROIDS_COUNT)  */
    TuneSensitivity_P4_v2_60(&CapSense_1_config);

    CapSense_1_EnableBaselineIDAC(&CapSense_1_config);

    for(curSensor = 0u; curSensor < CapSense_1_TOTAL_SCANSLOT_COUNT; curSensor++)
    {
        CapSense_1_UpdateThresholds(curSensor);
    }
}


/*******************************************************************************
* Function Name: CapSense_1_UpdateThresholds
********************************************************************************
*
* Summary:
*  The API updates the Finger Threshold, Hysteresis, Noise Threshold, and
*  Negative Noise Threshold in the AutoTuning (Smartsense) Mode.
*
* Parameters:
*  sensor: sensor number.
*
* Return:
*  None
*
* Global Variables:
*  CapSense_1_widgetNumber[] - contains widget Number for given sensor.
*  0 through 7 (sensor 0 is bit 0, sensor 1 is bit 1).
*  For other sensors the array element equals to 255.
*  CapSense_1_numberOfSensors[widget] - Number of sensors in the widget.
*   CapSense_1_fingerThreshold[] - contains the level of signal for each sensor
*   that determines if a finger present on the sensor.
*   CapSense_1_negativeNoiseThreshold[] - negative Noise Threshold
*   CapSense_1_hysteresis[] - the array with hysteresis values.
*
* Side Effects:
*  None
*
*******************************************************************************/
void CapSense_1_UpdateThresholds(uint32 sensor)
{
    uint8 fingerThreshold;
    uint32 widget;

    widget = CapSense_1_widgetNumber[sensor];

    #if(0u != CapSense_1_FLEXIBLE_THRESHOLDS_EN)
        fingerThreshold = CapSense_1_fingerThreshold[widget];
        /* Update Threshold manually (flexible) */
        CapSense_1_noiseThreshold[widget] = (uint8)(fingerThreshold >> 1u);
        CapSense_1_negativeNoiseThreshold[widget] = (uint8)(fingerThreshold >> 1u);
        CapSense_1_hysteresis[widget] = (uint8)(fingerThreshold >> 3u);
    #else
        /* Calculate Finger Threshold and Noise Threshold with Smartsense (automatic) */
        CalculateThresholds_P4_v2_60(&CapSense_1_config, (uint8)sensor, (uint8)widget, CapSense_1_fingerThreshold, CapSense_1_noiseThreshold);

        fingerThreshold = CapSense_1_fingerThreshold[widget];

        /* Update Threshold based on calculated with Smartsense (automatic) */
        CapSense_1_negativeNoiseThreshold[widget] = CapSense_1_noiseThreshold[widget];

        if(fingerThreshold < CapSense_1_THRESHOLD_1)
        {
            CapSense_1_hysteresis[widget] = (uint8)(fingerThreshold >> 3u);
        }
        else if(fingerThreshold <  CapSense_1_THRESHOLD_2)
        {
            CapSense_1_hysteresis[widget] = (uint8)(fingerThreshold >> 4u);
        }
        else if(fingerThreshold <  CapSense_1_THRESHOLD_3)
        {
            CapSense_1_hysteresis[widget] = (uint8)(fingerThreshold >> 5u);
        }
        else if(fingerThreshold <  CapSense_1_THRESHOLD_4)
        {
            CapSense_1_hysteresis[widget] = (uint8)(fingerThreshold >> 6u);
        }
        else
        {
            CapSense_1_hysteresis[widget] = 0u;
        }
    #endif /* (0u != CapSense_1_FLEXIBLE_THRESHOLDS_EN)  */
}

/*******************************************************************************
* Function Name: CapSense_1_SetAnalogSwitchesSrcDirect
********************************************************************************
*
* Summary:
*  This API switches the charge clock source to prescaler.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global Variables:
*  None
*
* Side Effects:
*  None
*
*******************************************************************************/
void CapSense_1_SetAnalogSwitchesSrcDirect(void)
{
    CapSense_1_CSD_CFG_REG &= ~(CapSense_1_CSD_CFG_PRS_SELECT);
}


/*******************************************************************************
* Function Name: CapSense_1_DisableBaselineIDAC
********************************************************************************
*
* Summary:
*  The API disables the Compensation IDAC.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global Variables:
*  None
*
* Side Effects:
*  None
*
*******************************************************************************/
void CapSense_1_DisableBaselineIDAC(void)
{
    uint32 newRegValue;

    newRegValue = CapSense_1_CSD_IDAC_REG;

    newRegValue &= ~(CapSense_1_CSD_IDAC1_MODE_MASK | CapSense_1_CSD_IDAC2_MODE_MASK);
    newRegValue |= CapSense_1_CSD_IDAC1_MODE_VARIABLE;

    CapSense_1_CSD_IDAC_REG = newRegValue;
}


/*******************************************************************************
* Function Name: CapSense_1_EnableBaselineIDAC
********************************************************************************
*
* Summary:
*  The API enables the Compensation IDAC.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global Variables:
*  CapSense_1_compensationIDAC[] - used to store a 7-bit idac value for all the sensors.
*
* Side Effects:
*  None
*
*******************************************************************************/
void  CapSense_1_EnableBaselineIDAC(const CapSense_1_CONFIG_TYPE_P4_v2_60 *config)
{
    uint32 curSensor;
    uint32 newRegValue;

    for(curSensor = 0u; curSensor < config->totalScanslotsNum; curSensor++)
    {
        CapSense_1_compensationIDAC[curSensor] = CapSense_1_modulationIDAC[curSensor] / 2u;
        CapSense_1_modulationIDAC[curSensor] = (CapSense_1_modulationIDAC[curSensor] + 1u) / 2u;
    }

    CyIntDisable(CapSense_1_ISR_NUMBER);

    newRegValue = CapSense_1_CSD_IDAC_REG;
    newRegValue &= ~(CapSense_1_CSD_IDAC1_MODE_MASK | CapSense_1_CSD_IDAC2_MODE_MASK);
    newRegValue |= (CapSense_1_CSD_IDAC1_MODE_VARIABLE | CapSense_1_CSD_IDAC2_MODE_FIXED);
    CapSense_1_CSD_IDAC_REG = newRegValue;

    CyIntEnable(CapSense_1_ISR_NUMBER);
}


/*******************************************************************************
* Function Name: CapSense_1_SetSensitivity
********************************************************************************
*
* Summary:
*  This API sets the sensitivity value for the sensor. The sensitivity
*  value is used during the auto-tuning algorithm executed as part of the CapSense_Start API.
*  This API is called by the application layer prior to calling the CapSense_Start API.
*  Calling this API after execution of CapSense_Start API has no effect.
*
* Parameters:
*  sensor  Sensor index
*  data    Sensitivity of the sensor. Possible values are below
*  1 - 0.1pf sensitivity
*  2 - 0.2pf sensitivity
*  3 - 0.3pf sensitivity
*  4 - 0.4pf sensitivity
*  5 - 0.5pf sensitivity
*  6 - 0.6pf sensitivity
*  7 - 0.7pf sensitivity
*  8 - 0.8pf sensitivity
*  9 - 0.9pf sensitivity
*  10 - 1.0pf sensitivity
*  All other values, set sensitivity to 1.0pf
*
*  Return Value:
*   None
*
* Global Variables:
* CapSense_1_sensorSensitivity[] - This 8-bits array contains Sensitivity in the LSB nibble
*                                and Noise Reset Counter value in the MSB nibble.
*
* Side Effects:
*  None
*
*******************************************************************************/
void CapSense_1_SetSensitivity(uint32 sensor, uint32 value)
{
    if(value > 10u)
    {
        value = 10u;
    }

    /* Clear SensorSensitivity value in LSB nibble */
    CapSense_1_sensorSensitivity[sensor] &= (uint8)~CapSense_1_SENSITIVITY_MASK;
    /* Set SensorSensitivity value in LSB nibble */
    CapSense_1_sensorSensitivity[sensor] |= (uint8)value;
}


/*******************************************************************************
* Function Name: CapSense_1_GetSensitivityCoefficient
********************************************************************************
*
* Summary:
*  This API returns the K coefficient for the appropriate sensor.
*
* Parameters:
*  sensor:  Sensor index
*
*  Return Value:
*   K - value for the appropriate sensor.
*
* Global Variables:
*  CapSense_1_kValueTbl[] - This 8-bits array contains the K value for each sensor.
*
* Side Effects:
*  None
*
*******************************************************************************/
uint8 CapSense_1_GetSensitivityCoefficient(uint32 sensor)
{
    return (CapSense_1_kValueTbl[sensor]);
}


/*******************************************************************************
* Function Name: CapSense_1_GetNoiseEnvelope
********************************************************************************
*
* Summary:
*  This function returns the noise value of a
*  component.
*
* Parameters:
*  sensor - Sensor number.
*
* Return:
*  The noise envelope value of the sensor
*  indicated by argument.
*
* Global Variables:
*  CapSense_1_noiseEnvelopeTbl[] - array with Noise Envelope.
*
* Side Effects:
*  None
*
*******************************************************************************/
uint16 CapSense_1_GetNoiseEnvelope(uint32 sensor)
{
    return((uint16)((uint16)CapSense_1_noiseEnvelopeTbl[sensor] << 1u) + 1u);
}


/*******************************************************************************
* Function Name: CapSense_1_GetNormalizedDiffCountData
********************************************************************************
*
* Summary:
*  This API returns normalized difference count data.
*
* Parameters:
*  sensor:  Sensor number.
*
* Return:
*  This API returns a normalized count value of the sensor indicated by the
*  argument.
*
* Global Variables:
*  CapSense_1_sensorSignal[] - array with difference counts for sensors
*
* Side Effects:
*  None
*
*******************************************************************************/
uint16 CapSense_1_GetNormalizedDiffCountData(uint32 sensor)
{
    return (uint16)(((uint32)CapSense_1_sensorSignal[sensor] << 7u) / CapSense_1_kValueTbl[sensor]);
}

#endif /* (CapSense_1_TUNING_METHOD == CapSense_1__TUNING_AUTO) */

/* [] END OF FILE */
