/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service4_app.h
  * @author  MCD Application Team
  * @brief   Header for service4_app.c
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef IDS_APP_H
#define IDS_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ids.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  IDS_CONN_HANDLE_EVT,
  IDS_DISCON_HANDLE_EVT,

  /* USER CODE BEGIN Service4_OpcodeNotificationEvt_t */

  /* USER CODE END Service4_OpcodeNotificationEvt_t */

  IDS_LAST_EVT,
} IDS_APP_OpcodeNotificationEvt_t;

typedef struct
{
  IDS_APP_OpcodeNotificationEvt_t          EvtOpcode;
  uint16_t                                 ConnectionHandle;

  /* USER CODE BEGIN IDS_APP_ConnHandleNotEvt_t */

  /* USER CODE END IDS_APP_ConnHandleNotEvt_t */
} IDS_APP_ConnHandleNotEvt_t;
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros -----------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define DUMMY_RECORDS_COUNT                                                   20
#define BLE_CFG_IDS_CALIBRATION_DATA_RECORD_TEST_NUM                         100
#define BLE_CFG_IDS_FEATURE_CALIBRATION                                        1
#define BLE_CFG_IDS_FEATURE_PATIENT_LOW_HIGH_ALERTS                            1
#define BLE_CFG_IDS_FEATURE_HYPO_ALERTS                                        1
#define BLE_CFG_IDS_FEATURE_HYPER_ALERTS                                       1
#define BLE_CFG_IDS_FEATURE_RATE_OF_INCREASE_DECREASE_ALERTS                   1
#define BLE_CFG_IDS_FEATURE_DEVICE_SPECIFIC_ALERT                              1
#define BLE_CFG_IDS_FEATURE_SENSOR_MALFUNCTION_DETECTION                       1
#define BLE_CFG_IDS_FEATURE_SENSOR_TEMPERATURE_HIGH_LOW_DETECTION              1
#define BLE_CFG_IDS_FEATURE_SENSOR_RESULT_HIGH_LOW_DETECTION                   1
#define BLE_CFG_IDS_FEATURE_LOW_BATTERY_DETECTION                              1
#define BLE_CFG_IDS_FEATURE_SENSOR_TYPE_ERROR_DETECTION                        1
#define BLE_CFG_IDS_FEATURE_GENERAL_DEVICE_FAULT                               1
#define BLE_CFG_IDS_FEATURE_E2E_CRC                                            1
#define BLE_CFG_IDS_FEATURE_MULTIPLE_BOND                                      1
#define BLE_CFG_IDS_FEATURE_MULTIPLE_SESSIONS                                  1
#define BLE_CFG_IDS_FEATURE_CGM_TREND_INFORMATION                              1
#define BLE_CFG_IDS_FEATURE_CGM_QUALITY                                        1
#define BLE_CFG_IDS_SAMPLE_LOCATION                   IDS_SAMPLE_LOCATION_FINGER
#define BLE_CFG_IDS_TYPE                          IDS_TYPE_CAPILLARY_WHOLE_BLOOD
   
#define BLE_CFG_IDS_SHORTEST_CGM_COMM_INTERVAL_SUPPORTED_BY_DEVICE_IN_MINS    10  /**< 1min */
#define BLE_CFG_IDS_DEFAULT_CGM_COMM_INTERVAL_VALUE                       (0xFF)  /** SHORTEST */

#define BLE_CFG_IDS_BATTERY_MEASUREMENT_INTERVAL_IN_SECS                      10

#define BLE_CFG_IDS_DB_RECORD_MAX_NUM                                        500

#define BLE_CFG_IDS_DB_RECORD_TEST_NUM                                        40
#define BLE_CFG_IDS_CALIBRATION_DATA_RECORD_TEST_NUM                         100
    
#if (BLE_CFG_IDS_FEATURE_E2E_CRC == 1)
#define IDS_CRC_LENGTH                                        (sizeof(uint16_t))
#else
#define IDS_CRC_LENGTH                                                       (0)
#endif
    
/**
 * Enabled or Disable PTS testing
 */
#define BLE_CFG_IDS_PTS                                                        0
#define BLE_CFG_IDS_DEBUG_RESET_DB_ON_DISCONNECT                               0
#define BLE_CFG_IDS_DEBUG_STOP_SESSION_ON_DISCONNECT                           0

#define IDS_APP_MEASUREMENT_INTERVAL                                   (10*1000)
#define IDS_APP_BATTERY_MEASUREMENT_INTERVAL (BLE_CFG_IDS_BATTERY_MEASUREMENT_INTERVAL_IN_SECS*1000)
#define IDS_APP_RACP_INTERVAL                                             (1000)

#define IDS_APP_MEASUREMENT_FLAGS    ((BLE_CFG_IDS_CGM_FEATURE_CGM_TREND_INFORMATION*IDS_FLAGS_CGM_TREND_INFORMATION_PRESENT) |\
                                       (BLE_CFG_IDS_CGM_FEATURE_CGM_QUALITY*IDS_FLAGS_CGM_QUALITY_PRESENT)                     |\
                                       (IDS_FLAGS_WARNING_OCTET_PRESENT)                                                        |\
                                       (IDS_FLAGS_CAL_TEMP_OCTET_PRESENT)                                                       |\
                                       (IDS_FLAGS_MEASUREMENT_STATUS_OCTET_PRESENT))
 
#define IDS_IDCCP_FILTER_TYPE_MAX_LENGTH                                  (15)
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void IDS_APP_Init(void);
void IDS_APP_EvtRx(IDS_APP_ConnHandleNotEvt_t *p_Notification);
/* USER CODE BEGIN EFP */
uint16_t IDS_APP_GetStatusChangedFlag(void);
void IDS_APP_SetStatusChangedFlag(uint16_t flag);
void IDS_APP_IdsrcpRequestHandler(uint8_t * pRequestData, 
                                  uint8_t requestDataLength);
uint8_t IDS_APP_IdsrcpCheckRequestValid(uint8_t * pRequestData, 
                                        uint8_t requestDataLength);
void IDS_APP_IdccpRequestHandler(uint8_t * pRequestData, 
                                 uint8_t requestDataLength);
uint8_t IDS_APP_IdccpCheckRequestValid(uint8_t * pRequestData, 
                                       uint8_t requestDataLength);
uint32_t IDS_APP_ComputeCRC(uint8_t * pData, uint8_t dataLength);
void IDS_APP_UpdateFeature(void);
uint32_t IDS_APP_GetFeature(void);
uint8_t IDS_APP_GetIDSRCPCharacteristicIndicationEnabled(void);
uint8_t IDS_APP_GetIDCCPCharacteristicIndicationEnabled(void);
uint8_t IDS_APP_GetStatusFlagReservoirAttached(void);
IDS_Status_Value_t * IDS_APP_GetStatus(void);
void IDS_APP_SetStatus(IDS_Status_Value_t * pStatus);
IDS_Annunciation_Status_Value_t * IDS_APP_GetAnnunciationStatus(void);
void IDS_APP_SetAnnunciationStatus(IDS_Annunciation_Status_Value_t * pAnnunciationStatus);
/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*IDS_APP_H */
