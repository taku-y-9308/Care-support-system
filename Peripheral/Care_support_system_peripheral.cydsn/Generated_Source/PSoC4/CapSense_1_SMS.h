/*******************************************************************************
* File Name: CapSense_1_SMS.h
* Version 2.60
*
* Description:
*  This file provides the declarations of the wrapper between the CapSense CSD component
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
#if !defined(CY_CAPSENSE_CSD_SMS_CapSense_1_H)
#define CY_CAPSENSE_CSD_SMS_CapSense_1_H

#include "CapSense_1.h"
#include "CapSense_1_PVT.h"

#if(0u != CapSense_1_CSHL_API_GENERATE)
    #include "CapSense_1_CSHL.h"
#endif /* (0u != CapSense_1_CSHL_API_GENERATE) */

#define CapSense_1_PRESCALERS_TBL_SIZE        (16u)

#define CapSense_1_CALIBRATION_FREQ_KHZ       (3000u)
#define CapSense_1_CALIBRATION_MD             (4u)

#define CapSense_1_MAX_RESOLUTION         (65535u)
#define CapSense_1_CALIBRATION_RESOLUTION (0x0FFF0000u)

#define CapSense_1_PRS_CORRECTION_DIVIDER (4u)
#define CapSense_1_SENSITIVITY_DIVIDER    (10u)

#define CapSense_1_SENSITIVITY_MASK           (0x0Fu)
#define CapSense_1_FLEXIBLE_THRESHOLDS_EN     (0u)
#define CapSense_1_CAL_RAW_COUNT              (3482u)

#define CapSense_1_THRESHOLD_1                (220u)
#define CapSense_1_THRESHOLD_2                (237u)
#define CapSense_1_THRESHOLD_3                (245u)
#define CapSense_1_THRESHOLD_4                (250u)


#if (0u != CapSense_1_IS_M0S8PERI_BLOCK)
    #define CapSense_1_CALIBRATION_ASD        (CYDEV_BCLK__HFCLK__KHZ /\
                                                     CapSense_1_CALIBRATION_FREQ_KHZ)
#else
    #define CapSense_1_MIN_IMO_FREQ_KHZ       (6000u)
    #if(CYDEV_BCLK__HFCLK__KHZ < CapSense_1_MIN_IMO_FREQ_KHZ)
        #define CapSense_1_CALIBRATION_ASD            (1u)
    #else
        #define CapSense_1_CALIBRATION_ASD    (CYDEV_BCLK__HFCLK__KHZ / CapSense_1_CALIBRATION_MD /\
                                                     CapSense_1_CALIBRATION_FREQ_KHZ)
    #endif /* (CYDEV_BCLK__HFCLK__KHZ < CapSense_1_MIN_IMO_FREQ_KHZ) */
#endif /* (0u != CapSense_1_IS_M0S8PERI_BLOCK) */

#if (0u != CapSense_1_IS_M0S8PERI_BLOCK)
    /*  Dividers are not chained */
    #if(CapSense_1_PRS_OPTIONS != CapSense_1__PRS_NONE)
        #define CapSense_1_SENSETIVITY_FACTOR (1228u)
    #else
        #define CapSense_1_SENSETIVITY_FACTOR (2456u)
    #endif /* (CapSense_1_PRS_OPTIONS == CapSense_1__PRS_NONE) */
#else
    /*  Dividers are chained */
    #if(CapSense_1_PRS_OPTIONS != CapSense_1__PRS_NONE)
        #define CapSense_1_SENSETIVITY_FACTOR (1228u/CapSense_1_CALIBRATION_MD)
    #else
        #define CapSense_1_SENSETIVITY_FACTOR (2456u/CapSense_1_CALIBRATION_MD)
    #endif /* (CapSense_1_PRS_OPTIONS == CapSense_1__PRS_NONE) */
#endif /* (0u != CapSense_1_IS_M0S8PERI_BLOCK) */


/***************************************
*        Function Prototypes
***************************************/
void CapSense_1_AutoTune(void);
void CapSense_1_UpdateThresholds(uint32 sensor);
void CapSense_1_SetSensitivity(uint32 sensor, uint32 value);
void CapSense_1_SetAnalogSwitchesSrcDirect(void);
uint16 CapSense_1_GetNoiseEnvelope(uint32 sensor);
uint8 CapSense_1_GetSensitivityCoefficient(uint32 sensor);
uint16 CapSense_1_GetNormalizedDiffCountData(uint32 sensor);


/***************************************
*     Vars with External Linkage
***************************************/

/*  Real Charge Divider values */
#if (0u != CapSense_1_IS_M0S8PERI_BLOCK)
    /*  Dividers are not chained */
    #if (CYDEV_BCLK__HFCLK__MHZ >= 12u)
        extern const uint8 prescalerTable[CapSense_1_PRESCALERS_TBL_SIZE];
    #elif (CYDEV_BCLK__HFCLK__MHZ >= 6u)
        extern const uint8 prescalerTable[CapSense_1_PRESCALERS_TBL_SIZE];
    #else
        extern const uint8 prescalerTable[CapSense_1_PRESCALERS_TBL_SIZE];
    #endif /* (CYDEV_BCLK__HFCLK__MHZ > 12u) */
#else
    /*  Dividers are chained */
    #if (CYDEV_BCLK__HFCLK__MHZ > 24u)
        extern const uint8 prescalerTable[CapSense_1_PRESCALERS_TBL_SIZE];
    #elif (CYDEV_BCLK__HFCLK__MHZ >12u)
        extern const uint8 prescalerTable[CapSense_1_PRESCALERS_TBL_SIZE];
    #else   /* (CYDEV_BCLK__HFCLK__MHZ > 12u) */
        extern const uint8 prescalerTable[CapSense_1_PRESCALERS_TBL_SIZE];
    #endif /* (CYDEV_BCLK__HFCLK__MHZ > 24u) */
#endif /* (0u != CapSense_1_IS_M0S8PERI_BLOCK) */


extern uint8 CapSense_1_noiseEnvelopeTbl[CapSense_1_TOTAL_SENSOR_COUNT];
extern uint8 CapSense_1_runningDifferenceTbl[CapSense_1_TOTAL_SENSOR_COUNT];
extern uint8 CapSense_1_signRegisterTbl[CapSense_1_TOTAL_SENSOR_COUNT];
extern uint16 CapSense_1_sampleMinTbl[CapSense_1_TOTAL_SENSOR_COUNT];
extern uint16 CapSense_1_sampleMaxTbl[CapSense_1_TOTAL_SENSOR_COUNT];
extern uint16 CapSense_1_previousSampleTbl[CapSense_1_TOTAL_SENSOR_COUNT];
extern uint8 CapSense_1_kValueTbl[CapSense_1_TOTAL_SENSOR_COUNT];


#endif  /* (CY_CAPSENSE_CSD_SMS_CapSense_1_H) */

/* [] END OF FILE */
