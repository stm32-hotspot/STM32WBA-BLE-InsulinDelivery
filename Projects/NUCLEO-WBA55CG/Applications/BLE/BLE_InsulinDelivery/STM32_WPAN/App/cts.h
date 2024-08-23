/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service2.h
  * @author  MCD Application Team
  * @brief   Header for service2.c
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
#ifndef CTS_H
#define CTS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported defines ----------------------------------------------------------*/
/* USER CODE BEGIN ED */

/* USER CODE END ED */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  CTS_CUT,
  CTS_LTI,
  CTS_RTI,
  /* USER CODE BEGIN Service2_CharOpcode_t */

  /* USER CODE END Service2_CharOpcode_t */
  CTS_CHAROPCODE_LAST
} CTS_CharOpcode_t;

typedef enum
{
  CTS_CUT_READ_EVT,
  CTS_CUT_WRITE_EVT,
  CTS_CUT_NOTIFY_ENABLED_EVT,
  CTS_CUT_NOTIFY_DISABLED_EVT,
  CTS_LTI_READ_EVT,
  CTS_LTI_WRITE_EVT,
  CTS_RTI_READ_EVT,
  /* USER CODE BEGIN Service2_OpcodeEvt_t */

  /* USER CODE END Service2_OpcodeEvt_t */
  CTS_BOOT_REQUEST_EVT
} CTS_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service2_Data_t */

  /* USER CODE END Service2_Data_t */
} CTS_Data_t;

typedef struct
{
  CTS_OpcodeEvt_t       EvtOpcode;
  CTS_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;
  /* USER CODE BEGIN Service2_NotificationEvt_t */

  /* USER CODE END Service2_NotificationEvt_t */
} CTS_NotificationEvt_t;

/* USER CODE BEGIN ET */
typedef struct
{
  uint16_t  Year;
  uint8_t   Month;
  uint8_t   Day;
  uint8_t   Hours;
  uint8_t   Minutes;
  uint8_t   Seconds;
}CTS_DateTime_t;

typedef struct 
{
  CTS_DateTime_t date_time;
  uint8_t day_of_week;
} CTS_DayDateTime_t;

typedef struct 
{
  CTS_DayDateTime_t day_date_time;
  uint8_t           fractions256;
} CTS_ExactTime256_Ch_t;

typedef struct 
{
  CTS_ExactTime256_Ch_t exact_time_256;
  uint8_t               adjust_reason;
} CTS_Ch_t;

typedef enum 
{
  CTS_ADJUST_MANUAL_TIME_UPDATE = (1<<0),
  CTS_ADJUST_EXTERNAL_REFERENCE_TIME_UPDATE = (1<<1),
  CTS_ADJUST_CHANGE_OF_TIME_ZONE = (1<<2),
  CTS_ADJUST_CHANGE_OF_DST = (1<<3),
  CTS_ADJUST_REASON_RESERVED = (1<<4),
} CTS_AdjustReason_t;

typedef enum 
{
  CTS_ERR_CODE_DATA_FIELD_IGNORED = 0x80,
} CTS_ErrorCode_t;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros -----------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void CTS_Init(void);
void CTS_Notification(CTS_NotificationEvt_t *p_Notification);
tBleStatus CTS_UpdateValue(CTS_CharOpcode_t CharOpcode, CTS_Data_t *pData);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*CTS_H */
