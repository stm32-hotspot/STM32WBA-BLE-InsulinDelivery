/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service5.h
  * @author  MCD Application Team
  * @brief   Header for service5.c
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
#ifndef IAS_H
#define IAS_H

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
  IAS_ALL,
  /* USER CODE BEGIN Service5_CharOpcode_t */

  /* USER CODE END Service5_CharOpcode_t */
  IAS_CHAROPCODE_LAST
} IAS_CharOpcode_t;

typedef enum
{
  IAS_ALL_WRITE_NO_RESP_EVT,
  /* USER CODE BEGIN Service5_OpcodeEvt_t */

  /* USER CODE END Service5_OpcodeEvt_t */
  IAS_BOOT_REQUEST_EVT
} IAS_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service5_Data_t */

  /* USER CODE END Service5_Data_t */
} IAS_Data_t;

typedef struct
{
  IAS_OpcodeEvt_t       EvtOpcode;
  IAS_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;
  /* USER CODE BEGIN Service5_NotificationEvt_t */

  /* USER CODE END Service5_NotificationEvt_t */
} IAS_NotificationEvt_t;

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

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void IAS_Init(void);
void IAS_Notification(IAS_NotificationEvt_t *p_Notification);
tBleStatus IAS_UpdateValue(IAS_CharOpcode_t CharOpcode, IAS_Data_t *pData);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*IAS_H */
