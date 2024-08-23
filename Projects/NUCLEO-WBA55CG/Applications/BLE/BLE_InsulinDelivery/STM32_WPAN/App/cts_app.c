/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service2_app.c
  * @author  MCD Application Team
  * @brief   service2_app application definition.
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
#include "cts_app.h"
#include "cts.h"
#include "stm32_seq.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32_timer.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef enum
{
  Cut_NOTIFICATION_OFF,
  Cut_NOTIFICATION_ON,
  /* USER CODE BEGIN Service2_APP_SendInformation_t */

  /* USER CODE END Service2_APP_SendInformation_t */
  CTS_APP_SENDINFORMATION_LAST
} CTS_APP_SendInformation_t;

typedef struct
{
  CTS_APP_SendInformation_t     Cut_Notification_Status;
  /* USER CODE BEGIN Service2_APP_Context_t */
  CTS_Ch_t cts_ch;
  uint8_t notify_status;
  uint32_t StartTick;
  UTIL_TIMER_Object_t CtsUpdate_Id;
  /* USER CODE END Service2_APP_Context_t */
  uint16_t              ConnectionHandle;
} CTS_APP_Context_t;

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define DEFAULT_CTS_DATE_TIME_YEAR                                         2022
#define DEFAULT_CTS_DATE_TIME_MONTH                                           7
#define DEFAULT_CTS_DATE_TIME_DAY                                             5
#define DEFAULT_CTS_DATE_TIME_HOURS                                           0
#define DEFAULT_CTS_DATE_TIME_MINUTES                                         0
#define DEFAULT_CTS_DATE_TIME_SECONDS                                         0
#define DEFAULT_CTS_DAY_OF_WEEK                                               2 /* Tuesday */
#define DEFAULT_CTS_FRACTION256                                               0
#define DEFAULT_ADJUST_REASON                        CTS_ADJUST_REASON_RESERVED

#define CTS_UPDATE_INTERVAL                                   (1000)  /**< 1s */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static CTS_APP_Context_t CTS_APP_Context;

uint8_t a_CTS_UpdateCharData[247];

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void CTS_Cut_SendNotification(void);

/* USER CODE BEGIN PFP */
static void cts_notify(void);
static void CTS_APP_Update_timCB( void *arg );
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void CTS_Notification(CTS_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service2_Notification_1 */

  /* USER CODE END Service2_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service2_Notification_Service2_EvtOpcode */

    /* USER CODE END Service2_Notification_Service2_EvtOpcode */

    case CTS_CUT_READ_EVT:
      /* USER CODE BEGIN Service2Char1_READ_EVT */

      /* USER CODE END Service2Char1_READ_EVT */
      break;

    case CTS_CUT_WRITE_EVT:
      /* USER CODE BEGIN Service2Char1_WRITE_EVT */

      /* USER CODE END Service2Char1_WRITE_EVT */
      break;

    case CTS_CUT_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN Service2Char1_NOTIFY_ENABLED_EVT */
      CTS_APP_Context.notify_status = 0; /* disable */
      /* USER CODE END Service2Char1_NOTIFY_ENABLED_EVT */
      break;

    case CTS_CUT_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN Service2Char1_NOTIFY_DISABLED_EVT */
      CTS_APP_Context.notify_status = 0; /* disable */
      /* USER CODE END Service2Char1_NOTIFY_DISABLED_EVT */
      break;

    case CTS_LTI_READ_EVT:
      /* USER CODE BEGIN Service2Char2_READ_EVT */

      /* USER CODE END Service2Char2_READ_EVT */
      break;

    case CTS_LTI_WRITE_EVT:
      /* USER CODE BEGIN Service2Char2_WRITE_EVT */

      /* USER CODE END Service2Char2_WRITE_EVT */
      break;

    case CTS_RTI_READ_EVT:
      /* USER CODE BEGIN Service2Char3_READ_EVT */

      /* USER CODE END Service2Char3_READ_EVT */
      break;

    default:
      /* USER CODE BEGIN Service2_Notification_default */

      /* USER CODE END Service2_Notification_default */
      break;
  }
  /* USER CODE BEGIN Service2_Notification_2 */

  /* USER CODE END Service2_Notification_2 */
  return;
}

void CTS_APP_EvtRx(CTS_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service2_APP_EvtRx_1 */

  /* USER CODE END Service2_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service2_APP_EvtRx_Service2_EvtOpcode */

    /* USER CODE END Service2_APP_EvtRx_Service2_EvtOpcode */
    case CTS_CONN_HANDLE_EVT :
      /* USER CODE BEGIN Service2_APP_CONN_HANDLE_EVT */

      /* USER CODE END Service2_APP_CONN_HANDLE_EVT */
      break;

    case CTS_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN Service2_APP_DISCON_HANDLE_EVT */

      /* USER CODE END Service2_APP_DISCON_HANDLE_EVT */
      break;

    default:
      /* USER CODE BEGIN Service2_APP_EvtRx_default */

      /* USER CODE END Service2_APP_EvtRx_default */
      break;
  }

  /* USER CODE BEGIN Service2_APP_EvtRx_2 */

  /* USER CODE END Service2_APP_EvtRx_2 */

  return;
}

void CTS_APP_Init(void)
{
  UNUSED(CTS_APP_Context);
  CTS_Init();

  /* USER CODE BEGIN Service2_APP_Init */
  APP_DBG_MSG("CTS_APP_Init\n\r");

  /* initialize Context */
  CTS_APP_Context.cts_ch.exact_time_256.day_date_time.date_time.Year = DEFAULT_CTS_DATE_TIME_YEAR;
  CTS_APP_Context.cts_ch.exact_time_256.day_date_time.date_time.Month = DEFAULT_CTS_DATE_TIME_MONTH;
  CTS_APP_Context.cts_ch.exact_time_256.day_date_time.date_time.Day = DEFAULT_CTS_DATE_TIME_DAY;
  CTS_APP_Context.cts_ch.exact_time_256.day_date_time.date_time.Hours = DEFAULT_CTS_DATE_TIME_HOURS;
  CTS_APP_Context.cts_ch.exact_time_256.day_date_time.date_time.Minutes = DEFAULT_CTS_DATE_TIME_MINUTES;
  CTS_APP_Context.cts_ch.exact_time_256.day_date_time.date_time.Seconds = DEFAULT_CTS_DATE_TIME_SECONDS;

  CTS_APP_Context.cts_ch.exact_time_256.day_date_time.day_of_week = DEFAULT_CTS_DAY_OF_WEEK;

  CTS_APP_Context.cts_ch.exact_time_256.fractions256 = DEFAULT_CTS_FRACTION256; /* The number of 1/256 fractions of a second. Valid range 0–255 */

  CTS_APP_Context.cts_ch.adjust_reason = DEFAULT_ADJUST_REASON;

  CTS_APP_Context.notify_status = 0; /* disable */

  CTS_APP_Context.StartTick = HAL_GetTick();

  /*
  * Register task for Current Time Notify
  */
  UTIL_SEQ_RegTask( 1<< CFG_TASK_CTS_NOTIFY_ID, UTIL_SEQ_RFU, cts_notify );
  
  /*
   * Create timer for Current Time Update 
   */
  UTIL_TIMER_Create(&(CTS_APP_Context.CtsUpdate_Id),
                    CTS_UPDATE_INTERVAL,
                    UTIL_TIMER_PERIODIC,
                    &CTS_APP_Update_timCB, 0);
  
  /* Start Current Time Update */
  UTIL_TIMER_StartWithPeriod(&(CTS_APP_Context.CtsUpdate_Id), CTS_UPDATE_INTERVAL);
  /* USER CODE END Service2_APP_Init */
  return;
}

/* USER CODE BEGIN FD */
void CTSAPP_Reset(void)
{
  APP_DBG_MSG("CTSAPP_Reset\n\r");

  /* Reset Context */
  CTS_APP_Context.notify_status = 0; /* disable */
}
/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
__USED void CTS_Cut_SendNotification(void) /* Property Notification */
{
  CTS_APP_SendInformation_t notification_on_off = Cut_NOTIFICATION_OFF;
  CTS_Data_t cts_notification_data;

  cts_notification_data.p_Payload = (uint8_t*)a_CTS_UpdateCharData;
  cts_notification_data.Length = 0;

  /* USER CODE BEGIN Service2Char1_NS_1*/

  /* USER CODE END Service2Char1_NS_1*/

  if (notification_on_off != Cut_NOTIFICATION_OFF)
  {
    CTS_UpdateValue(CTS_CUT, &cts_notification_data);
  }

  /* USER CODE BEGIN Service2Char1_NS_Last*/

  /* USER CODE END Service2Char1_NS_Last*/

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/
static void cts_notify(void)
{
  uint32_t seconds;
  CTS_Data_t msg_conf;
  uint8_t length = 0;

//  APP_DBG_MSG("CTS Notify (Tick = %ld, enabled = %d)\n\r", HAL_GetTick(), CTS_APP_Context.notify_status );

#if 0
  if(CTS_APP_Context.notify_status == 0)
  {
    return;
  }
#endif
  
  /* day date time */
  CTS_APP_Context.cts_ch.exact_time_256.day_date_time.date_time.Year    = DEFAULT_CTS_DATE_TIME_YEAR;
  CTS_APP_Context.cts_ch.exact_time_256.day_date_time.date_time.Month   = DEFAULT_CTS_DATE_TIME_MONTH;
  CTS_APP_Context.cts_ch.exact_time_256.day_date_time.date_time.Day     = DEFAULT_CTS_DATE_TIME_DAY;
  CTS_APP_Context.cts_ch.exact_time_256.day_date_time.date_time.Hours   = DEFAULT_CTS_DATE_TIME_HOURS;
  CTS_APP_Context.cts_ch.exact_time_256.day_date_time.date_time.Minutes = DEFAULT_CTS_DATE_TIME_MINUTES;
  CTS_APP_Context.cts_ch.exact_time_256.day_date_time.date_time.Seconds = DEFAULT_CTS_DATE_TIME_SECONDS;
    
  seconds = (HAL_GetTick() - CTS_APP_Context.StartTick) / 1000;
  
  while(seconds >= 60)
  {
    seconds -= 60;
    CTS_APP_Context.cts_ch.exact_time_256.day_date_time.date_time.Minutes += 1;
  }
  CTS_APP_Context.cts_ch.exact_time_256.day_date_time.date_time.Seconds = seconds;
  
  while(CTS_APP_Context.cts_ch.exact_time_256.day_date_time.date_time.Minutes >= 60)
  {
    CTS_APP_Context.cts_ch.exact_time_256.day_date_time.date_time.Minutes -= 60;
    CTS_APP_Context.cts_ch.exact_time_256.day_date_time.date_time.Hours += 1;
  }
  
  while(CTS_APP_Context.cts_ch.exact_time_256.day_date_time.date_time.Hours >= 24)
  {
    CTS_APP_Context.cts_ch.exact_time_256.day_date_time.date_time.Hours -= 24;
    CTS_APP_Context.cts_ch.exact_time_256.day_date_time.date_time.Day += 1;
    CTS_APP_Context.cts_ch.exact_time_256.day_date_time.day_of_week += 1;
    if(CTS_APP_Context.cts_ch.exact_time_256.day_date_time.day_of_week > 7)
    {
      CTS_APP_Context.cts_ch.exact_time_256.day_date_time.day_of_week = 1;
    }
  }
  
  /* let's handle date simply without permanment calendar */
  while(CTS_APP_Context.cts_ch.exact_time_256.day_date_time.date_time.Day >= 31)
  {
    CTS_APP_Context.cts_ch.exact_time_256.day_date_time.date_time.Day -= 31;
    CTS_APP_Context.cts_ch.exact_time_256.day_date_time.date_time.Month += 1;
  }
  
  while(CTS_APP_Context.cts_ch.exact_time_256.day_date_time.date_time.Month >= 12)
  {
    CTS_APP_Context.cts_ch.exact_time_256.day_date_time.date_time.Month -= 11;
    CTS_APP_Context.cts_ch.exact_time_256.day_date_time.date_time.Year += 1;
  }

  /* adjust reason */
  CTS_APP_Context.cts_ch.adjust_reason = CTS_ADJUST_MANUAL_TIME_UPDATE;

  a_CTS_UpdateCharData[length++] = (CTS_APP_Context.cts_ch.exact_time_256.day_date_time.date_time.Year) & 0xFF;
  a_CTS_UpdateCharData[length++] = ((CTS_APP_Context.cts_ch.exact_time_256.day_date_time.date_time.Year) >> 8) & 0xFF;
  a_CTS_UpdateCharData[length++] = CTS_APP_Context.cts_ch.exact_time_256.day_date_time.date_time.Month;
  a_CTS_UpdateCharData[length++] = CTS_APP_Context.cts_ch.exact_time_256.day_date_time.date_time.Day;
  a_CTS_UpdateCharData[length++] = CTS_APP_Context.cts_ch.exact_time_256.day_date_time.date_time.Hours;
  a_CTS_UpdateCharData[length++] = CTS_APP_Context.cts_ch.exact_time_256.day_date_time.date_time.Minutes;
  a_CTS_UpdateCharData[length++] = CTS_APP_Context.cts_ch.exact_time_256.day_date_time.date_time.Seconds;
  a_CTS_UpdateCharData[length++] = CTS_APP_Context.cts_ch.exact_time_256.day_date_time.day_of_week;
  a_CTS_UpdateCharData[length++] = CTS_APP_Context.cts_ch.exact_time_256.fractions256;
  a_CTS_UpdateCharData[length++] = CTS_APP_Context.cts_ch.adjust_reason;
  
  msg_conf.Length = length;
  msg_conf.p_Payload = a_CTS_UpdateCharData;
  CTS_UpdateValue(CTS_CUT, &msg_conf);
}

static void CTS_APP_Update_timCB( void *arg )
{
  /**
   * The code shall be executed in the background as aci command may be sent
   * The background is the only place where the application can make sure a new aci command
   * is not sent if there is a pending one
   */
  UTIL_SEQ_SetTask( 1<<CFG_TASK_CTS_NOTIFY_ID, CFG_SEQ_PRIO_0);

  return;
}
/* USER CODE END FD_LOCAL_FUNCTIONS*/
