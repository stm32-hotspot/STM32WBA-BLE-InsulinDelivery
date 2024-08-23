/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service5_app.h
  * @author  MCD Application Team
  * @brief   Header for service5_app.c
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
#ifndef IAS_APP_H
#define IAS_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  IAS_CONN_HANDLE_EVT,
  IAS_DISCON_HANDLE_EVT,

  /* USER CODE BEGIN Service5_OpcodeNotificationEvt_t */

  /* USER CODE END Service5_OpcodeNotificationEvt_t */

  IAS_LAST_EVT,
} IAS_APP_OpcodeNotificationEvt_t;

typedef struct
{
  IAS_APP_OpcodeNotificationEvt_t          EvtOpcode;
  uint16_t                                 ConnectionHandle;

  /* USER CODE BEGIN IAS_APP_ConnHandleNotEvt_t */

  /* USER CODE END IAS_APP_ConnHandleNotEvt_t */
} IAS_APP_ConnHandleNotEvt_t;
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
void IAS_APP_Init(void);
void IAS_APP_EvtRx(IAS_APP_ConnHandleNotEvt_t *p_Notification);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*IAS_APP_H */
