/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service5_app.c
  * @author  MCD Application Team
  * @brief   service5_app application definition.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_common.h"
#include "app_ble.h"
#include "ll_sys_if.h"
#include "dbg_trace.h"
#include "ble.h"
#include "ias_app.h"
#include "ias.h"
#include "stm32_seq.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef enum
{
  /* USER CODE BEGIN Service5_APP_SendInformation_t */

  /* USER CODE END Service5_APP_SendInformation_t */
  IAS_APP_SENDINFORMATION_LAST
} IAS_APP_SendInformation_t;

typedef struct
{
  /* USER CODE BEGIN Service5_APP_Context_t */

  /* USER CODE END Service5_APP_Context_t */
  uint16_t              ConnectionHandle;
} IAS_APP_Context_t;

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static IAS_APP_Context_t IAS_APP_Context;

uint8_t a_IAS_UpdateCharData[247];

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void IAS_Notification(IAS_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service5_Notification_1 */

  /* USER CODE END Service5_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service5_Notification_Service5_EvtOpcode */

    /* USER CODE END Service5_Notification_Service5_EvtOpcode */

    case IAS_ALL_WRITE_NO_RESP_EVT:
      /* USER CODE BEGIN Service5Char1_WRITE_NO_RESP_EVT */

      /* USER CODE END Service5Char1_WRITE_NO_RESP_EVT */
      break;

    default:
      /* USER CODE BEGIN Service5_Notification_default */

      /* USER CODE END Service5_Notification_default */
      break;
  }
  /* USER CODE BEGIN Service5_Notification_2 */

  /* USER CODE END Service5_Notification_2 */
  return;
}

void IAS_APP_EvtRx(IAS_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service5_APP_EvtRx_1 */

  /* USER CODE END Service5_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service5_APP_EvtRx_Service5_EvtOpcode */

    /* USER CODE END Service5_APP_EvtRx_Service5_EvtOpcode */
    case IAS_CONN_HANDLE_EVT :
      /* USER CODE BEGIN Service5_APP_CONN_HANDLE_EVT */

      /* USER CODE END Service5_APP_CONN_HANDLE_EVT */
      break;

    case IAS_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN Service5_APP_DISCON_HANDLE_EVT */

      /* USER CODE END Service5_APP_DISCON_HANDLE_EVT */
      break;

    default:
      /* USER CODE BEGIN Service5_APP_EvtRx_default */

      /* USER CODE END Service5_APP_EvtRx_default */
      break;
  }

  /* USER CODE BEGIN Service5_APP_EvtRx_2 */

  /* USER CODE END Service5_APP_EvtRx_2 */

  return;
}

void IAS_APP_Init(void)
{
  UNUSED(IAS_APP_Context);
  IAS_Init();

  /* USER CODE BEGIN Service5_APP_Init */

  /* USER CODE END Service5_APP_Init */
  return;
}

/* USER CODE BEGIN FD */

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/

/* USER CODE END FD_LOCAL_FUNCTIONS*/
