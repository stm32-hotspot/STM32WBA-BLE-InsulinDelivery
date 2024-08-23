/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service4_app.c
  * @author  MCD Application Team
  * @brief   service4_app application definition.
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
#include "ids_app.h"
#include "ids.h"
#include "stm32_seq.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32_timer.h"
#include "crc_ctrl.h"
#include "crc_ctrl_conf.h"
//#include "ids_racp.h"
#include "ids_srcp.h"
//#include "ids_ccp.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
  SYSTEM_ISSUE                = 0x000F,
  MECHANICAL_ISSUE            = 0x0033,
  OCCLUSION_DETECTED          = 0x003C,
  RESERVOIR_ISSUE             = 0x0055,
  RESERVOIR_EMPTY             = 0x005A,
  RESERVOIR_LOW               = 0x0066,
  PRIMING_ISSUE               = 0x0069,
  INFUSION_SET_INCOMPLETE     = 0x0096,
  INFUSION_SET_DETACHED       = 0x0099,
  POWER_SOURCE_INSUFFICIENT   = 0x00A5,
  BATTERY_EMPTY               = 0x00AA,
  BATTERY_LOW                 = 0x00C3,
  BATTERY_MEDIUM              = 0x00C,
  BATTERY_FULL                = 0x00F0,
  TEMPERATURE_OUT_OF_RANGE    = 0x00FF,
  AIR_PRESSURE_OUT_OF_RANGE   = 0x0303,
  BOLUS_CANCELED              = 0x030C,
  TBR_OVER                    = 0x0330,
  TBR_CANCELED                = 0x033F,
  MAX_DELIVERY                = 0x0356,
  DATE_TIME_ISSUE             = 0x0359,
  TEMPERATURE                 = 0x0365
} IDS_APP_Annunciation_Type_t;

typedef enum
{
  NO_BOLUS_FLAG = 0,
  DELAY_TIME_PRESENT         = (1<<0),
  TEMPLATE_NUMBER_PRESENT    = (1<<1),
  ACTIVATION_TYPE_PRESENT    = (1<<2),
  DELIVERY_REASON_CORRECTION = (1<<3),
  DELIVERY_REASON_MEAL       = (1<<4)
} IDS_APP_Bolus_Flags_t;

typedef enum
{
  UNDETERMINED_TYPE = (0x0F),
  FAST              = (0x33),
  EXTENDED          = (0x3C),
  MULTI_WAVE        = (0x55)
} IDS_APP_Bolus_Type_t;

typedef enum
{
  UNDETERMINED_ACTIVATION_TYPE  = (0x0F),
  MANUAL                        = (0x33),
  RECOMMENDED                   = (0x3C),
  MANUALLY_CHANGED_RECOMMENDED  = (0x55),
  COMMANDED                     = (0x5A)
} IDS_APP_Bolus_Activation_Type_t;

typedef enum
{
  ABSOLUTE = (0x33),
  RELATIVE = (0x3c)
} IDS_APP_TBR_Type_Value_t;

/* USER CODE END PTD */

typedef enum
{
  Idsc_INDICATION_OFF,
  Idsc_INDICATION_ON,
  Ids_INDICATION_OFF,
  Ids_INDICATION_ON,
  Idas_INDICATION_OFF,
  Idas_INDICATION_ON,
  Idf_INDICATION_OFF,
  Idf_INDICATION_ON,
  Idsrcp_INDICATION_OFF,
  Idsrcp_INDICATION_ON,
  /* USER CODE BEGIN Service4_APP_SendInformation_t */

  /* USER CODE END Service4_APP_SendInformation_t */
  IDS_APP_SENDINFORMATION_LAST
} IDS_APP_SendInformation_t;

typedef struct
{
  IDS_APP_SendInformation_t     Idsc_Indication_Status;
  IDS_APP_SendInformation_t     Ids_Indication_Status;
  IDS_APP_SendInformation_t     Idas_Indication_Status;
  IDS_APP_SendInformation_t     Idf_Indication_Status;
  IDS_APP_SendInformation_t     Idsrcp_Indication_Status;
  /* USER CODE BEGIN Service4_APP_Context_t */
  IDS_Status_Changed_Value_t IDSCChar;
  IDS_Status_Value_t IDSChar;
  IDS_Annunciation_Status_Value_t IDASChar;
  IDS_Feature_Value_t IDFChar;
  IDS_StatusRCP_Value_t IDSRCPChar;
  UTIL_TIMER_Object_t TimerRACPProcess_Id;
  /* USER CODE END Service4_APP_Context_t */
  uint16_t              ConnectionHandle;
} IDS_APP_Context_t;

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* Operational States */
#define UNDETERMINED                                                        0x0F
#define OFF                                                                 0x33
#define STANDBY                                                             0x3C
#define PREPARING                                                           0x55
#define PRIMING                                                             0x5A
#define WAITING                                                             0x66
#define READY                                                               0x96

/* Therapy Control States */
#define STOP_STATE                                                          0x33
#define PAUSE_STATE                                                         0x3C
#define RUN_STATE                                                           0x55

#define INSTANCE_ID                                                       0x1111
/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */
extern CRC_HandleTypeDef hcrc;
extern uint8_t ActiveBolusNumber;
extern uint8_t ActiveBasalRateDeliveryNumber;

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static IDS_APP_Context_t IDS_APP_Context;

uint8_t a_IDS_UpdateCharData[247];

/* USER CODE BEGIN PV */
CRCCTRL_Handle_t ID_Handle;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void IDS_Idsc_SendIndication(void);
static void IDS_Ids_SendIndication(void);
static void IDS_Idas_SendIndication(void);
static void IDS_Idf_SendIndication(void);
static void IDS_Idsrcp_SendIndication(void);

/* USER CODE BEGIN PFP */
//static tBleStatus IDS_Srcp_SendResponseCode(uint16_t Opcode, uint8_t responseCode);
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void IDS_Notification(IDS_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service4_Notification_1 */

  /* USER CODE END Service4_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service4_Notification_Service4_EvtOpcode */

    /* USER CODE END Service4_Notification_Service4_EvtOpcode */

    case IDS_IDSC_READ_EVT:
      /* USER CODE BEGIN Service4Char1_READ_EVT */

      /* USER CODE END Service4Char1_READ_EVT */
      break;

    case IDS_IDSC_INDICATE_ENABLED_EVT:
      /* USER CODE BEGIN Service4Char1_INDICATE_ENABLED_EVT */
      LOG_INFO_APP("Status Change Indication Enabled\r\n");
      IDS_APP_Context.Idsc_Indication_Status = Idsc_INDICATION_ON;
      /* USER CODE END Service4Char1_INDICATE_ENABLED_EVT */
      break;

    case IDS_IDSC_INDICATE_DISABLED_EVT:
      /* USER CODE BEGIN Service4Char1_INDICATE_DISABLED_EVT */
      LOG_INFO_APP("Status Change Indication Disabled\r\n");
      IDS_APP_Context.Idsc_Indication_Status = Idsc_INDICATION_OFF;
      /* USER CODE END Service4Char1_INDICATE_DISABLED_EVT */
      break;

    case IDS_IDS_READ_EVT:
      /* USER CODE BEGIN Service4Char2_READ_EVT */

      /* USER CODE END Service4Char2_READ_EVT */
      break;

    case IDS_IDS_INDICATE_ENABLED_EVT:
      /* USER CODE BEGIN Service4Char2_INDICATE_ENABLED_EVT */
      LOG_INFO_APP("Status Indication Enabled\r\n");
      IDS_APP_Context.Ids_Indication_Status = Ids_INDICATION_ON;
      /* USER CODE END Service4Char2_INDICATE_ENABLED_EVT */
      break;

    case IDS_IDS_INDICATE_DISABLED_EVT:
      /* USER CODE BEGIN Service4Char2_INDICATE_DISABLED_EVT */
      LOG_INFO_APP("Status Indication Disabled\r\n");
      IDS_APP_Context.Ids_Indication_Status = Ids_INDICATION_OFF;
      /* USER CODE END Service4Char2_INDICATE_DISABLED_EVT */
      break;

    case IDS_IDAS_READ_EVT:
      /* USER CODE BEGIN Service4Char3_READ_EVT */

      /* USER CODE END Service4Char3_READ_EVT */
      break;

    case IDS_IDAS_INDICATE_ENABLED_EVT:
      /* USER CODE BEGIN Service4Char3_INDICATE_ENABLED_EVT */
      LOG_INFO_APP("Annunciation Status Indication Enabled\r\n");
      IDS_APP_Context.Idas_Indication_Status = Idas_INDICATION_ON;
      /* USER CODE END Service4Char3_INDICATE_ENABLED_EVT */
      break;

    case IDS_IDAS_INDICATE_DISABLED_EVT:
      /* USER CODE BEGIN Service4Char3_INDICATE_DISABLED_EVT */
      LOG_INFO_APP("Annunciation Status Indication Disabled\r\n");
      IDS_APP_Context.Idas_Indication_Status = Idas_INDICATION_OFF;
      /* USER CODE END Service4Char3_INDICATE_DISABLED_EVT */
      break;

    case IDS_IDF_READ_EVT:
      /* USER CODE BEGIN Service4Char4_READ_EVT */

      /* USER CODE END Service4Char4_READ_EVT */
      break;

    case IDS_IDF_INDICATE_ENABLED_EVT:
      /* USER CODE BEGIN Service4Char4_INDICATE_ENABLED_EVT */
      LOG_INFO_APP("Feature Indication Enabled\r\n");
      IDS_APP_Context.Idf_Indication_Status = Idf_INDICATION_ON;
      /* USER CODE END Service4Char4_INDICATE_ENABLED_EVT */
      break;

    case IDS_IDF_INDICATE_DISABLED_EVT:
      /* USER CODE BEGIN Service4Char4_INDICATE_DISABLED_EVT */
      LOG_INFO_APP("Feature Indication Disabled\r\n");
      IDS_APP_Context.Idf_Indication_Status = Idf_INDICATION_OFF;
      /* USER CODE END Service4Char4_INDICATE_DISABLED_EVT */
      break;

    case IDS_IDSRCP_WRITE_EVT:
      /* USER CODE BEGIN Service4Char5_WRITE_EVT */

      /* USER CODE END Service4Char5_WRITE_EVT */
      break;

    case IDS_IDSRCP_INDICATE_ENABLED_EVT:
      /* USER CODE BEGIN Service4Char5_INDICATE_ENABLED_EVT */
      LOG_INFO_APP("Status Reader Control Point Indication Enabled\r\n");
      IDS_APP_Context.Idsrcp_Indication_Status = Idsrcp_INDICATION_ON;
      /* USER CODE END Service4Char5_INDICATE_ENABLED_EVT */
      break;

    case IDS_IDSRCP_INDICATE_DISABLED_EVT:
      /* USER CODE BEGIN Service4Char5_INDICATE_DISABLED_EVT */
      LOG_INFO_APP("Status Reader Control Point Indication Disabled\r\n");
      IDS_APP_Context.Idsrcp_Indication_Status = Idsrcp_INDICATION_OFF;
      /* USER CODE END Service4Char5_INDICATE_DISABLED_EVT */
      break;

    default:
      /* USER CODE BEGIN Service4_Notification_default */

      /* USER CODE END Service4_Notification_default */
      break;
  }
  /* USER CODE BEGIN Service4_Notification_2 */

  /* USER CODE END Service4_Notification_2 */
  return;
}

void IDS_APP_EvtRx(IDS_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service4_APP_EvtRx_1 */

  /* USER CODE END Service4_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service4_APP_EvtRx_Service4_EvtOpcode */

    /* USER CODE END Service4_APP_EvtRx_Service4_EvtOpcode */
    case IDS_CONN_HANDLE_EVT :
      /* USER CODE BEGIN Service4_APP_CONN_HANDLE_EVT */

      /* USER CODE END Service4_APP_CONN_HANDLE_EVT */
      break;

    case IDS_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN Service4_APP_DISCON_HANDLE_EVT */
      ActiveBolusNumber = NB_ACTIVE_BOLUSES_IDS;
      ActiveBasalRateDeliveryNumber = 1;
      /* USER CODE END Service4_APP_DISCON_HANDLE_EVT */
      break;

    default:
      /* USER CODE BEGIN Service4_APP_EvtRx_default */

      /* USER CODE END Service4_APP_EvtRx_default */
      break;
  }

  /* USER CODE BEGIN Service4_APP_EvtRx_2 */

  /* USER CODE END Service4_APP_EvtRx_2 */

  return;
}

void IDS_APP_Init(void)
{
  UNUSED(IDS_APP_Context);
  IDS_Init();

  /* USER CODE BEGIN Service4_APP_Init */
  IDS_Data_t msg_conf;
  uint8_t length = 0;
  CRCCTRL_Cmd_Status_t result;
  
  IDS_SRCP_Init();
  
  /* Insulin Delivery Feature */
  IDS_APP_Context.IDFChar.Flags = E2E_PROTECTION_SUPPORTED                        |
                                  BASAL_RATE_SUPPORTED                            |
                                  TBR_ABSOLUTE_SUPPORTED                          |
                                  TBR_REALTIVE_SUPPORTED                          |
                                  TBR_TEMPLATE_SUPPORTED                          |
                                  FAST_BOLUS_SUPPORTED                            |
                                  EXTENDED_BOLUS_SUPPORTED                        |
                                  MULTIWAVE_BOLUS_SUPPORTED                       |
                                  BOLUS_DELAY_TIME_SUPPORTED                      |
                                  BOLUS_TEMPLATE_SUPPORTED                        |
                                  BOLUS_ACTIVATION_TYPE_SUPPORTED                 |
                                  MULTIPLE_BOND_SUPPORTED                         |
                                  ISF_PROFILE_TEMPLATE_SUPPORTED                  |
                                  I2CHO_RATIO_PTOFILE_TEMPLATE_SUPPORTED          |
                                  TARGET_GLUCOSE_RANGE_PROFILE_TEMPLATE_SUPPORTED |
                                  INSULIN_ON_BOARD_SUPPORTED                      |
                                  FEATURE_EXTENSION ;
  IDS_APP_Context.IDFChar.Insulin_Concentration = 0;

  if(((IDS_APP_Context.IDFChar.Flags) & E2E_PROTECTION_SUPPORTED) != (E2E_PROTECTION_SUPPORTED))
  {
    IDS_APP_Context.IDFChar.E2E_CRC = 0xFFFF;
    IDS_APP_Context.IDFChar.E2E_Counter = 0;
  }
  else
  {
    IDS_APP_Context.IDFChar.E2E_Counter = 1;
    
    ID_Handle.Uid = 0x00;
    ID_Handle.PreviousComputedValue = 0x00;
    ID_Handle.State = HANDLE_NOT_REG;
    ID_Handle.Configuration.DefaultPolynomialUse = hcrc.Init.DefaultPolynomialUse;
    ID_Handle.Configuration.DefaultInitValueUse = hcrc.Init.DefaultInitValueUse;
    ID_Handle.Configuration.GeneratingPolynomial = hcrc.Init.GeneratingPolynomial;
    ID_Handle.Configuration.CRCLength = hcrc.Init.CRCLength;
    ID_Handle.Configuration.InputDataInversionMode = hcrc.Init.InputDataInversionMode;
    ID_Handle.Configuration.OutputDataInversionMode = hcrc.Init.OutputDataInversionMode;
    ID_Handle.Configuration.InputDataFormat = hcrc.InputDataFormat;

    /* Register CRC Handle */
    result = CRCCTRL_RegisterHandle(&ID_Handle);
    if (result != CRCCTRL_OK)
    {
      Error_Handler();
    }
  }

  a_IDS_UpdateCharData[2 + length] = IDS_APP_Context.IDFChar.E2E_Counter;
  length++;
  a_IDS_UpdateCharData[2 + length] =  (IDS_APP_Context.IDFChar.Insulin_Concentration) & 0xFF;
  length++;
  a_IDS_UpdateCharData[2 + length] = ((IDS_APP_Context.IDFChar.Insulin_Concentration) >> 8) & 0xFF;
  length++;
  a_IDS_UpdateCharData[2 + length] =  (IDS_APP_Context.IDFChar.Flags) & 0xFF;
  length++;
  a_IDS_UpdateCharData[2 + length] = ((IDS_APP_Context.IDFChar.Flags) >> 8) & 0xFF;
  length++;
  a_IDS_UpdateCharData[2 + length] = ((IDS_APP_Context.IDFChar.Flags) >> 16) & 0xFF;
  length++;
  if(((IDS_APP_Context.IDFChar.Flags) & E2E_PROTECTION_SUPPORTED) == (E2E_PROTECTION_SUPPORTED))
  {
    IDS_APP_Context.IDFChar.E2E_CRC =  (uint16_t)IDS_APP_ComputeCRC(&(a_IDS_UpdateCharData[2]), length);
    a_IDS_UpdateCharData[0] = (uint8_t)((IDS_APP_Context.IDFChar.E2E_CRC) & 0xFF);
    a_IDS_UpdateCharData[1] = (uint8_t)(((IDS_APP_Context.IDFChar.E2E_CRC) >> 8) & 0xFF);
  }
  
  msg_conf.Length = 2 + length;
  msg_conf.p_Payload = a_IDS_UpdateCharData;
  IDS_UpdateValue(IDS_IDF, &msg_conf);
  
  /* Insulin Delivery Status Changed */
  IDS_APP_Context.IDSCChar.Flags = THERAPY_CONTROL_STATE_CHANGED      |
                                   OPERATIONAL_STATE_CHANGED          |
                                   RESERVOIR_STATUS_CHANGED           |
                                   ANNUNCIATION_STATUS_CHANGED        |
                                   TOTAL_DAILY_INSULIN_STATUS_CHANGED |
                                   ACTIVE_BASAL_RATE_STATUS_CHANGED   |
                                   ACTIVE_BOLUS_STATUS_CHANGED        /*|
                                   HISTORY_EVENT_RECORDED*/;
  if(((IDS_APP_Context.IDFChar.Flags) & E2E_PROTECTION_SUPPORTED) != (E2E_PROTECTION_SUPPORTED))
  {
    IDS_APP_Context.IDSCChar.E2E_CRC = 0xFFFF;
    IDS_APP_Context.IDSCChar.E2E_Counter = 0;
  }
  else
  {
    IDS_APP_Context.IDSCChar.E2E_Counter = 1;
  }

  length = 0;
  a_IDS_UpdateCharData[length++] =  (IDS_APP_Context.IDSCChar.Flags) & 0xFF;
  a_IDS_UpdateCharData[length++] = ((IDS_APP_Context.IDSCChar.Flags) >> 8) & 0xFF;
  a_IDS_UpdateCharData[length++] = IDS_APP_Context.IDSCChar.E2E_Counter;
  if(((IDS_APP_Context.IDFChar.Flags) & E2E_PROTECTION_SUPPORTED) == (E2E_PROTECTION_SUPPORTED))
  {
    IDS_APP_Context.IDSCChar.E2E_CRC =  (uint16_t)IDS_APP_ComputeCRC(a_IDS_UpdateCharData, length);
  }
  a_IDS_UpdateCharData[length++] = (uint8_t)((IDS_APP_Context.IDSCChar.E2E_CRC) & 0xFF);
  a_IDS_UpdateCharData[length++] = (uint8_t)(((IDS_APP_Context.IDSCChar.E2E_CRC) >> 8) & 0xFF);
 
  msg_conf.Length = length;
  msg_conf.p_Payload = a_IDS_UpdateCharData;
  IDS_UpdateValue(IDS_IDSC, &msg_conf);
  
  /* Insulin Delivery Status */
  IDS_APP_Context.IDSChar.Flags = RESERVOIR_ATTACHED;
  IDS_APP_Context.IDSChar.Operational_State = OFF;
  IDS_APP_Context.IDSChar.Reservoir_Remaining_Amount = 0;
  IDS_APP_Context.IDSChar.Therapy_Control_State = STOP_STATE;
  if(((IDS_APP_Context.IDFChar.Flags) & E2E_PROTECTION_SUPPORTED) != (E2E_PROTECTION_SUPPORTED))
  {
    IDS_APP_Context.IDSChar.E2E_CRC = 0xFFFF;
    IDS_APP_Context.IDSChar.E2E_Counter = 0;
  }
  else
  {
    IDS_APP_Context.IDSChar.E2E_Counter = 1;
  }

  length = 0;
  a_IDS_UpdateCharData[length++] =   IDS_APP_Context.IDSChar.Therapy_Control_State;
  a_IDS_UpdateCharData[length++] =   IDS_APP_Context.IDSChar.Operational_State;
  a_IDS_UpdateCharData[length++] =  (IDS_APP_Context.IDSChar.Reservoir_Remaining_Amount) & 0xFF;
  a_IDS_UpdateCharData[length++] = ((IDS_APP_Context.IDSChar.Reservoir_Remaining_Amount) >> 8) & 0xFF;
  a_IDS_UpdateCharData[length++] =   IDS_APP_Context.IDSChar.Flags;
  a_IDS_UpdateCharData[length++] = IDS_APP_Context.IDSChar.E2E_Counter;
  if(((IDS_APP_Context.IDFChar.Flags) & E2E_PROTECTION_SUPPORTED) == (E2E_PROTECTION_SUPPORTED))
  {
    IDS_APP_Context.IDSChar.E2E_CRC = (uint16_t)IDS_APP_ComputeCRC(a_IDS_UpdateCharData, length);
  }
  a_IDS_UpdateCharData[length++] = (uint8_t)((IDS_APP_Context.IDSChar.E2E_CRC) & 0xFF);
  a_IDS_UpdateCharData[length++] = (uint8_t)(((IDS_APP_Context.IDSChar.E2E_CRC) >> 8) & 0xFF);

  msg_conf.Length = length;
  msg_conf.p_Payload = a_IDS_UpdateCharData;
  IDS_UpdateValue(IDS_IDS, &msg_conf);
  
  /* Insulin Delivery Annunciation Status */
  IDS_APP_Context.IDASChar.Flags = ANNUNCIATION_PRESENT |
                                   AUXINFO1_PRESENT     |
                                   AUXINFO2_PRESENT     |
                                   AUXINFO3_PRESENT     |
                                   AUXINFO4_PRESENT     |
                                   AUXINFO5_PRESENT;
  IDS_APP_Context.IDASChar.Instance_ID = INSTANCE_ID;
  IDS_APP_Context.IDASChar.Type = RESERVOIR_EMPTY;
  IDS_APP_Context.IDASChar.Status = CONFIRMED;
  if(((IDS_APP_Context.IDASChar.Flags) & AUXINFO1_PRESENT) == AUXINFO1_PRESENT)
  {
    IDS_APP_Context.IDASChar.Aux_Info[0] = 0x01;
    if(((IDS_APP_Context.IDASChar.Flags) & AUXINFO2_PRESENT) == AUXINFO2_PRESENT)
    {
      IDS_APP_Context.IDASChar.Aux_Info[1] = 0x02;
      if(((IDS_APP_Context.IDASChar.Flags) & AUXINFO3_PRESENT) == AUXINFO3_PRESENT)
      {
        IDS_APP_Context.IDASChar.Aux_Info[2] = 0x03;
        if(((IDS_APP_Context.IDASChar.Flags) & AUXINFO4_PRESENT) == AUXINFO4_PRESENT)
        {
          IDS_APP_Context.IDASChar.Aux_Info[3] = 0x04;
          if(((IDS_APP_Context.IDASChar.Flags) & AUXINFO5_PRESENT) == AUXINFO5_PRESENT)
          {
            IDS_APP_Context.IDASChar.Aux_Info[4] = 0x05;
          }
        }
      }
    }
  }
  
  if(((IDS_APP_Context.IDFChar.Flags) & E2E_PROTECTION_SUPPORTED) != (E2E_PROTECTION_SUPPORTED))
  {
    IDS_APP_Context.IDASChar.E2E_CRC = 0xFFFF;
    IDS_APP_Context.IDASChar.E2E_Counter = 0;
  }
  else
  {
    IDS_APP_Context.IDASChar.E2E_Counter = 1;
  }

  length = 0;
  a_IDS_UpdateCharData[length++] = IDS_APP_Context.IDASChar.Flags;
  a_IDS_UpdateCharData[length++] = (uint8_t)((IDS_APP_Context.IDASChar.Instance_ID) & 0xFF);
  a_IDS_UpdateCharData[length++] = (uint8_t)(((IDS_APP_Context.IDASChar.Instance_ID) >> 8) & 0xFF);
  a_IDS_UpdateCharData[length++] = (uint8_t)((IDS_APP_Context.IDASChar.Type) & 0xFF);
  a_IDS_UpdateCharData[length++] = (uint8_t)(((IDS_APP_Context.IDASChar.Type) >> 8) & 0xFF);
  a_IDS_UpdateCharData[length++] = IDS_APP_Context.IDASChar.Status;
  if(((IDS_APP_Context.IDASChar.Flags) & AUXINFO1_PRESENT) == AUXINFO1_PRESENT)
  {
    a_IDS_UpdateCharData[length++] = (uint8_t)((IDS_APP_Context.IDASChar.Aux_Info[0]) & 0xFF);
    a_IDS_UpdateCharData[length++] = (uint8_t)(((IDS_APP_Context.IDASChar.Aux_Info[0]) >> 8) & 0xFF);
    if(((IDS_APP_Context.IDASChar.Flags) & AUXINFO2_PRESENT) == AUXINFO2_PRESENT)
    {
      a_IDS_UpdateCharData[length++] = (uint8_t)((IDS_APP_Context.IDASChar.Aux_Info[1]) & 0xFF);
      a_IDS_UpdateCharData[length++] = (uint8_t)(((IDS_APP_Context.IDASChar.Aux_Info[1]) >> 8) & 0xFF);
      if(((IDS_APP_Context.IDASChar.Flags) & AUXINFO3_PRESENT) == AUXINFO3_PRESENT)
      {
        a_IDS_UpdateCharData[length++] = (uint8_t)((IDS_APP_Context.IDASChar.Aux_Info[2]) & 0xFF);
        a_IDS_UpdateCharData[length++] = (uint8_t)(((IDS_APP_Context.IDASChar.Aux_Info[2]) >> 8) & 0xFF);
        if(((IDS_APP_Context.IDASChar.Flags) & AUXINFO4_PRESENT) == AUXINFO4_PRESENT)
        {
          a_IDS_UpdateCharData[length++] = (uint8_t)((IDS_APP_Context.IDASChar.Aux_Info[3]) & 0xFF);
          a_IDS_UpdateCharData[length++] = (uint8_t)(((IDS_APP_Context.IDASChar.Aux_Info[3]) >> 8) & 0xFF);
          if(((IDS_APP_Context.IDASChar.Flags) & AUXINFO5_PRESENT) == AUXINFO5_PRESENT)
          {
            a_IDS_UpdateCharData[length++] = (uint8_t)((IDS_APP_Context.IDASChar.Aux_Info[4]) & 0xFF);
            a_IDS_UpdateCharData[length++] = (uint8_t)(((IDS_APP_Context.IDASChar.Aux_Info[4]) >> 8) & 0xFF);
          }
        }
      }
    }
  }
  a_IDS_UpdateCharData[length++] = IDS_APP_Context.IDASChar.E2E_Counter;

  if(((IDS_APP_Context.IDFChar.Flags) & E2E_PROTECTION_SUPPORTED) == (E2E_PROTECTION_SUPPORTED))
  {
    IDS_APP_Context.IDASChar.E2E_CRC =  (uint16_t)IDS_APP_ComputeCRC(a_IDS_UpdateCharData, length);
  }
  a_IDS_UpdateCharData[length++] = (uint8_t)((IDS_APP_Context.IDASChar.E2E_CRC) & 0xFF);
  a_IDS_UpdateCharData[length++] = (uint8_t)(((IDS_APP_Context.IDASChar.E2E_CRC) >> 8) & 0xFF);

  msg_conf.Length = length;
  msg_conf.p_Payload = a_IDS_UpdateCharData;
  IDS_UpdateValue(IDS_IDAS, &msg_conf);
  /* USER CODE END Service4_APP_Init */
  return;
}

/* USER CODE BEGIN FD */
uint16_t IDS_APP_GetStatusChangedFlag(void)
{
  return(IDS_APP_Context.IDSCChar.Flags);
}

void IDS_APP_SetStatusChangedFlag(uint16_t flag)
{
  IDS_Data_t msg_conf;
  uint8_t length = 0;

  IDS_APP_Context.IDSCChar.Flags = flag;
  
  if(((IDS_APP_Context.IDFChar.Flags) & E2E_PROTECTION_SUPPORTED) != (E2E_PROTECTION_SUPPORTED))
  {
    IDS_APP_Context.IDSCChar.E2E_CRC = 0xFFFF;
    IDS_APP_Context.IDSCChar.E2E_Counter = 0;
  }
  else
  {
    IDS_APP_Context.IDSCChar.E2E_Counter += 1;
  }

  a_IDS_UpdateCharData[length++] =  (IDS_APP_Context.IDSCChar.Flags) & 0xFF;
  a_IDS_UpdateCharData[length++] = ((IDS_APP_Context.IDSCChar.Flags) >> 8) & 0xFF;
  a_IDS_UpdateCharData[length++] = IDS_APP_Context.IDSCChar.E2E_Counter;
  if(((IDS_APP_Context.IDFChar.Flags) & E2E_PROTECTION_SUPPORTED) == (E2E_PROTECTION_SUPPORTED))
  {
    IDS_APP_Context.IDSCChar.E2E_CRC =  (uint16_t)IDS_APP_ComputeCRC(a_IDS_UpdateCharData, length);
  }
  a_IDS_UpdateCharData[length++] = (uint8_t)((IDS_APP_Context.IDSCChar.E2E_CRC) & 0xFF);
  a_IDS_UpdateCharData[length++] = (uint8_t)(((IDS_APP_Context.IDSCChar.E2E_CRC) >> 8) & 0xFF);

  msg_conf.Length = length;
  msg_conf.p_Payload = a_IDS_UpdateCharData;
  IDS_UpdateValue(IDS_IDSC, &msg_conf);
}

#if 0
/**
* @brief SRCP request handler 
* @param requestData: pointer to received SRCP request data
* @param requestDataLength: received SRCP request length
* @retval None
*/
void IDS_APP_IdsrcpRequestHandler(uint8_t * pRequestData, uint8_t requestDataLength)
{
  uint16_t OpCode = pRequestData[IDS_SRCP_OP_CODE_POSITION] | (pRequestData[IDS_SRCP_OP_CODE_POSITION + 1] << 8);

  UTIL_SEQ_ClrEvt ( 1 << CFG_IDLEEVT_PROC_GATT_INDICATION_COMPLETE);
  
  /* Check and Process the OpCode */
  switch(OpCode)
  {
    case RESET_STATUS:
      {
        LOG_INFO_APP("RESET_STATUS\r\n");
        if (requestDataLength != IDS_SRCP_RESET_REQUEST_LENGTH)
        {
          LOG_INFO_APP("INVALID OPERAND\r\n");
          IDS_Srcp_SendResponseCode(OpCode, INVALID_OPERAND);
          UTIL_SEQ_WaitEvt(1U << CFG_IDLEEVT_PROC_GATT_INDICATION_COMPLETE);
        }
        else 
        {
          if(((IDS_APP_Context.IDFChar.Flags) & E2E_PROTECTION_SUPPORTED) == (E2E_PROTECTION_SUPPORTED))
          {
            IDS_APP_Context.IDSRCPChar.E2E_Counter = pRequestData[4];
            IDS_APP_Context.IDSRCPChar.E2E_CRC = pRequestData[5] | (pRequestData[6] << 8);
          }
          else
          {
            IDS_APP_Context.IDSRCPChar.E2E_Counter = 0;
            IDS_APP_Context.IDSRCPChar.E2E_CRC = 0xFFFF;
          }
          
          LOG_INFO_APP("Received E2E_Counter %d\r\n", IDS_APP_Context.IDSRCPChar.E2E_Counter);
          LOG_INFO_APP("Received E2E_CRC 0x%x\r\n", IDS_APP_Context.IDSRCPChar.E2E_CRC);
          LOG_INFO_APP("Calculated E2E_CRC 0x%x\r\n", 
                       (uint16_t)IDS_APP_ComputeCRC(pRequestData, requestDataLength - 2));

          IDS_APP_Context.IDSRCPChar.Flag = 0;
          
          if((pRequestData[IDS_SRCP_OPERAND_POSITION] & THERAPY_CONTROL_STATE_CHANGED) == 0)
            IDS_APP_Context.IDSRCPChar.Flag |= THERAPY_CONTROL_STATE_CHANGED;
          if((pRequestData[IDS_SRCP_OPERAND_POSITION] & OPERATIONAL_STATE_CHANGED) == 0)
            IDS_APP_Context.IDSRCPChar.Flag |= OPERATIONAL_STATE_CHANGED;
          if((pRequestData[IDS_SRCP_OPERAND_POSITION] & RESERVOIR_STATUS_CHANGED) == 0)
            IDS_APP_Context.IDSRCPChar.Flag |= RESERVOIR_STATUS_CHANGED;
          if((pRequestData[IDS_SRCP_OPERAND_POSITION] & ANNUNCIATION_STATUS_CHANGED) == 0)
            IDS_APP_Context.IDSRCPChar.Flag |= ANNUNCIATION_STATUS_CHANGED;
          if((pRequestData[IDS_SRCP_OPERAND_POSITION] & TOTAL_DAILY_INSULIN_STATUS_CHANGED) == 0)
            IDS_APP_Context.IDSRCPChar.Flag |= TOTAL_DAILY_INSULIN_STATUS_CHANGED;
          if((pRequestData[IDS_SRCP_OPERAND_POSITION] & ACTIVE_BASAL_RATE_STATUS_CHANGED) == 0)
            IDS_APP_Context.IDSRCPChar.Flag |= ACTIVE_BASAL_RATE_STATUS_CHANGED;
          if((pRequestData[IDS_SRCP_OPERAND_POSITION] & ACTIVE_BOLUS_STATUS_CHANGED) == 0)
            IDS_APP_Context.IDSRCPChar.Flag |= ACTIVE_BOLUS_STATUS_CHANGED;
          if((pRequestData[IDS_SRCP_OPERAND_POSITION] & HISTORY_EVENT_RECORDED) == 0)
            IDS_APP_Context.IDSRCPChar.Flag |= HISTORY_EVENT_RECORDED;
          IDS_APP_SetStatusFlag((IDS_SC_Flags_t)(IDS_APP_Context.IDSRCPChar.Flag));
          UTIL_SEQ_WaitEvt(1U << CFG_IDLEEVT_PROC_GATT_INDICATION_COMPLETE);
          UTIL_SEQ_ClrEvt ( 1 << CFG_IDLEEVT_PROC_GATT_INDICATION_COMPLETE);
          LOG_INFO_APP("SUCCESS\r\n");
          IDS_Srcp_SendResponseCode(OpCode, SUCCESS_RESPONSE);
          UTIL_SEQ_WaitEvt(1U << CFG_IDLEEVT_PROC_GATT_INDICATION_COMPLETE);
        }
      }
      break;
                      
    case GET_ACTIVE_BOLUS_IDS:
      {
        LOG_INFO_APP("GET_ACTIVE_BOLUS_IDS\r\n");
        if (requestDataLength != IDS_SRCP_NO_OPERAND_REQUEST_LENGTH)
        {
          LOG_INFO_APP("INVALID OPERAND\r\n");
          IDS_Srcp_SendResponseCode(OpCode, INVALID_OPERAND);
          UTIL_SEQ_WaitEvt(1U << CFG_IDLEEVT_PROC_GATT_INDICATION_COMPLETE);
        }
        else 
        {
          if(((IDS_APP_Context.IDFChar.Flags) & E2E_PROTECTION_SUPPORTED) == (E2E_PROTECTION_SUPPORTED))
          {
            IDS_APP_Context.IDSRCPChar.E2E_Counter = pRequestData[2];
            IDS_APP_Context.IDSRCPChar.E2E_CRC = pRequestData[3] | (pRequestData[4] << 8);
          }
          else
          {
            IDS_APP_Context.IDSRCPChar.E2E_Counter = 0;
            IDS_APP_Context.IDSRCPChar.E2E_CRC = 0xFFFF;
          }
          
          LOG_INFO_APP("Received E2E_Counter %d\r\n", IDS_APP_Context.IDSRCPChar.E2E_Counter);
          LOG_INFO_APP("Received E2E_CRC 0x%x\r\n", IDS_APP_Context.IDSRCPChar.E2E_CRC);
          LOG_INFO_APP("Calculated E2E_CRC 0x%x\r\n", 
                       (uint16_t)IDS_APP_ComputeCRC(pRequestData, requestDataLength - 2));

          IDS_APP_Context.IDSRCPChar.Flag = 0;
          
//          UTIL_SEQ_WaitEvt(1U << CFG_IDLEEVT_PROC_GATT_INDICATION_COMPLETE);
//          UTIL_SEQ_ClrEvt ( 1 << CFG_IDLEEVT_PROC_GATT_INDICATION_COMPLETE);
          LOG_INFO_APP("SUCCESS\r\n");
          IDS_Srcp_SendResponseCode(OpCode, SUCCESS_RESPONSE);
//          UTIL_SEQ_WaitEvt(1U << CFG_IDLEEVT_PROC_GATT_INDICATION_COMPLETE);
        }
      }
      break;
                      
    case GET_ACTIVE_BOLUS_DELIVERY:
      {
        LOG_INFO_APP("GET_ACTIVE_BOLUS_DELIVERY\r\n");
        if (requestDataLength != IDS_SRCP_GET_ACTIVE_BOLUS_DELIVERY_REQUEST_LENGTH)
        {
          LOG_INFO_APP("INVALID OPERAND\r\n");
          IDS_Srcp_SendResponseCode(OpCode, INVALID_OPERAND);
          UTIL_SEQ_WaitEvt(1U << CFG_IDLEEVT_PROC_GATT_INDICATION_COMPLETE);
        }
        else 
        {
          if(((IDS_APP_Context.IDFChar.Flags) & E2E_PROTECTION_SUPPORTED) == (E2E_PROTECTION_SUPPORTED))
          {
            IDS_APP_Context.IDSRCPChar.E2E_Counter = pRequestData[5];
            IDS_APP_Context.IDSRCPChar.E2E_CRC = pRequestData[6] | (pRequestData[7] << 8);
          }
          else
          {
            IDS_APP_Context.IDSRCPChar.E2E_Counter = 0;
            IDS_APP_Context.IDSRCPChar.E2E_CRC = 0xFFFF;
          }
          
          LOG_INFO_APP("Received E2E_Counter %d\r\n", IDS_APP_Context.IDSRCPChar.E2E_Counter);
          LOG_INFO_APP("Received E2E_CRC 0x%x\r\n", IDS_APP_Context.IDSRCPChar.E2E_CRC);
          LOG_INFO_APP("Calculated E2E_CRC 0x%x\r\n", 
                       (uint16_t)IDS_APP_ComputeCRC(pRequestData, requestDataLength - 2));

          IDS_APP_Context.IDSRCPChar.Flag = 0;
          
//          UTIL_SEQ_WaitEvt(1U << CFG_IDLEEVT_PROC_GATT_INDICATION_COMPLETE);
//          UTIL_SEQ_ClrEvt ( 1 << CFG_IDLEEVT_PROC_GATT_INDICATION_COMPLETE);
          LOG_INFO_APP("SUCCESS\r\n");
          IDS_Srcp_SendResponseCode(OpCode, SUCCESS_RESPONSE);
//          UTIL_SEQ_WaitEvt(1U << CFG_IDLEEVT_PROC_GATT_INDICATION_COMPLETE);
        }
      }
      break;
                      
    default:
      {
        LOG_INFO_APP("OP CODE NOT SUPPORTED\r\n");
        IDS_Srcp_SendResponseCode(OpCode, OP_CODE_NOT_SUPPORTED);
//        UTIL_SEQ_WaitEvt(1U << CFG_IDLEEVT_PROC_GATT_INDICATION_COMPLETE);
      }
      break;
  }
} /* end of IDS_SRCP_RequestHandler() */

/**
* @brief IDCCP new write request permit check
* @param [in] requestData: pointer to received IDCCP request data
* @param [in] requestDataLength: received IDCCP request length
* @retval 0x00 when no error, error code otherwise
*/
uint8_t IDS_APP_IdccpCheckRequestValid(uint8_t * pRequestData, uint8_t requestDataLength)
{
  uint8_t retval = 0x00;
  
  LOG_INFO_APP("IDCCP Request, request data length: %d\r\n", requestDataLength);
  
  if (requestDataLength > IDS_IDCCP_FILTER_TYPE_MAX_LENGTH)
  {
    retval = INVALID_OPERAND;
  }

  return retval;
} /* end of IDS_APP_IdccpCheckRequestValid() */

/**
* @brief IDCCP request handler 
* @param requestData: pointer to received IDCCP request data
* @param requestDataLength: received IDCCP request length
* @retval None
*/
void IDS_APP_IdccpRequestHandler(uint8_t * pRequestData, uint8_t requestDataLength)
{
#if 0
  uint16_t OpCode = pRequestData[IDS_SRCP_OP_CODE_POSITION] | (pRequestData[IDS_SRCP_OP_CODE_POSITION + 1] << 8);

  UTIL_SEQ_ClrEvt ( 1 << CFG_IDLEEVT_PROC_GATT_INDICATION_COMPLETE);
  
  /* Check and Process the OpCode */
  switch(OpCode)
  {
    case RESET_STATUS:
      {
        LOG_INFO_APP("RESET_STATUS\r\n");
        if (requestDataLength != IDS_SRCP_FILTER_TYPE_RESET_LENGTH)
        {
          LOG_INFO_APP("INVALID OPERAND\r\n");
          IDS_Srcp_SendResponseCode(OpCode, INVALID_OPERAND);
          UTIL_SEQ_WaitEvt(1U << CFG_IDLEEVT_PROC_GATT_INDICATION_COMPLETE);
        }
        else 
        {
          IDS_APP_Context.IDSRCPChar.Flag = 0;
          
          if((pRequestData[IDS_SRCP_OPERAND_POSITION] & THERAPY_CONTROL_STATE_CHANGED) == 0)
            IDS_APP_Context.IDSRCPChar.Flag |= THERAPY_CONTROL_STATE_CHANGED;
          if((pRequestData[IDS_SRCP_OPERAND_POSITION] & OPERATIONAL_STATE_CHANGED) == 0)
            IDS_APP_Context.IDSRCPChar.Flag |= OPERATIONAL_STATE_CHANGED;
          if((pRequestData[IDS_SRCP_OPERAND_POSITION] & RESERVOIR_STATUS_CHANGED) == 0)
            IDS_APP_Context.IDSRCPChar.Flag |= RESERVOIR_STATUS_CHANGED;
          if((pRequestData[IDS_SRCP_OPERAND_POSITION] & ANNUNCIATION_STATUS_CHANGED) == 0)
            IDS_APP_Context.IDSRCPChar.Flag |= ANNUNCIATION_STATUS_CHANGED;
          if((pRequestData[IDS_SRCP_OPERAND_POSITION] & TOTAL_DAILY_INSULIN_STATUS_CHANGED) == 0)
            IDS_APP_Context.IDSRCPChar.Flag |= TOTAL_DAILY_INSULIN_STATUS_CHANGED;
          if((pRequestData[IDS_SRCP_OPERAND_POSITION] & ACTIVE_BASAL_RATE_STATUS_CHANGED) == 0)
            IDS_APP_Context.IDSRCPChar.Flag |= ACTIVE_BASAL_RATE_STATUS_CHANGED;
          if((pRequestData[IDS_SRCP_OPERAND_POSITION] & ACTIVE_BOLUS_STATUS_CHANGED) == 0)
            IDS_APP_Context.IDSRCPChar.Flag |= ACTIVE_BOLUS_STATUS_CHANGED;
          if((pRequestData[IDS_SRCP_OPERAND_POSITION] & HISTORY_EVENT_RECORDED) == 0)
            IDS_APP_Context.IDSRCPChar.Flag |= HISTORY_EVENT_RECORDED;
          IDS_APP_SetStatusFlag((IDS_SC_Flags_t)(IDS_APP_Context.IDSRCPChar.Flag));
          UTIL_SEQ_WaitEvt(1U << CFG_IDLEEVT_PROC_GATT_INDICATION_COMPLETE);
          UTIL_SEQ_ClrEvt ( 1 << CFG_IDLEEVT_PROC_GATT_INDICATION_COMPLETE);
          LOG_INFO_APP("SUCCESS\r\n");
          IDS_Srcp_SendResponseCode(OpCode, SUCCESS_RESPONSE);
          UTIL_SEQ_WaitEvt(1U << CFG_IDLEEVT_PROC_GATT_INDICATION_COMPLETE);
        }
      }
      break;
                      
    default:
      {
        LOG_INFO_APP("OP CODE NOT SUPPORTED\r\n");
        IDS_Srcp_SendResponseCode(OpCode, OP_CODE_NOT_SUPPORTED);
        UTIL_SEQ_WaitEvt(1U << CFG_IDLEEVT_PROC_GATT_INDICATION_COMPLETE);
      }
      break;
  }
#endif
} /* end of IDS_SRCP_RequestHandler() */

uint8_t IDS_RACP_APP_EventHandler(IDS_RACP_App_Event_t *pRACPAppEvent)
{
  switch(pRACPAppEvent->EventCode)
  {      
    case IDS_RACP_APP_REPORT_RECORDS_PROCEDURE_STARTED_EVENT:
      {
        LOG_INFO_APP("IDS_RACP_APP_REPORT_RECORDS_PROCEDURE_STARTED_EVENT\r\n"); 

        UTIL_TIMER_Start(&(IDS_APP_Context.TimerRACPProcess_Id));
      }
      break;
      
    case IDS_RACP_APP_REPORT_RECORDS_PROCEDURE_FINISHED_EVENT:
      {
        LOG_INFO_APP("IDS_RACP_APP_REPORT_RECORDS_PROCEDURE_FINISHED_EVENT\r\n");
      }
      break;
      
    case IDS_RACP_APP_REPORT_RECORDS_PROCEDURE_NOTIFY_NEXT_RECORD_EVENT:
      {
        LOG_INFO_APP("IDS_RACP_APP_REPORT_RECORDS_PROCEDURE_NOTIFY_NEXT_RECORD_EVENT\r\n"); 

        UTIL_TIMER_Start(&(IDS_APP_Context.TimerRACPProcess_Id));
      }
      break;
      
    default:
      break;
  }
  
  return TRUE;
}
#endif
#if 0
uint16_t IDS_APP_ComputeCRC(uint8_t * pData, uint8_t dataLength)
{
  uint16_t result;
  
  // STM_TODO : Return value of both HAL functions should be further tested
  // STM_TODO : Add HW semaphore for CRC
  
  result = HAL_CRC_Calculate(&hcrc, (uint32_t *)pData, dataLength);
  
  return result;
}
#else
uint32_t IDS_APP_ComputeCRC(uint8_t * pData, uint8_t dataLength)
{
  CRCCTRL_Cmd_Status_t result;
  uint32_t crcValue;
  // STM_TODO : Return value of both HAL functions should be further tested
  // STM_TODO : Add HW semaphore for CRC
  
  result = CRCCTRL_Calculate (&ID_Handle,
                              (uint32_t *)pData,
                              dataLength,
                              &crcValue);
  
  if (result != CRCCTRL_OK)
  {
    Error_Handler();
  }
  return crcValue;
}

#endif

/**
  * @brief Update Feature characteristic
  * @param None
  * @retval None
  */
void IDS_APP_UpdateFeature(void)
{
  IDS_Data_t msg_conf;
  uint8_t ids_char_length = 0;
  tBleStatus ret;
  
  /* Insulin Delivery Feature */
  IDS_APP_Context.IDFChar.Flags = E2E_PROTECTION_SUPPORTED                        |
                                  BASAL_RATE_SUPPORTED                            |
                                  TBR_ABSOLUTE_SUPPORTED                          |
                                  TBR_REALTIVE_SUPPORTED                          |
                                  TBR_TEMPLATE_SUPPORTED                          |
                                  FAST_BOLUS_SUPPORTED                            |
                                  EXTENDED_BOLUS_SUPPORTED                        |
                                  MULTIWAVE_BOLUS_SUPPORTED                       |
                                  BOLUS_DELAY_TIME_SUPPORTED                      |
                                  BOLUS_TEMPLATE_SUPPORTED                        |
                                  BOLUS_ACTIVATION_TYPE_SUPPORTED                 |
                                  MULTIPLE_BOND_SUPPORTED                         |
                                  ISF_PROFILE_TEMPLATE_SUPPORTED                  |
                                  I2CHO_RATIO_PTOFILE_TEMPLATE_SUPPORTED          |
                                  TARGET_GLUCOSE_RANGE_PROFILE_TEMPLATE_SUPPORTED |
                                  INSULIN_ON_BOARD_SUPPORTED                      |
                                  FEATURE_EXTENSION ;
  IDS_APP_Context.IDFChar.Insulin_Concentration = 0;

  if(((IDS_APP_Context.IDFChar.Flags) & E2E_PROTECTION_SUPPORTED) != (E2E_PROTECTION_SUPPORTED))
  {
    IDS_APP_Context.IDFChar.E2E_CRC = 0xFFFF;
    IDS_APP_Context.IDFChar.E2E_Counter = 0;
  }


  a_IDS_UpdateCharData[ids_char_length++] =  (IDS_APP_Context.IDFChar.E2E_CRC) & 0xFF;
  a_IDS_UpdateCharData[ids_char_length++] = ((IDS_APP_Context.IDFChar.E2E_CRC) >> 8) & 0xFF;
  a_IDS_UpdateCharData[ids_char_length++] =   IDS_APP_Context.IDFChar.E2E_Counter;
  a_IDS_UpdateCharData[ids_char_length++] =  (IDS_APP_Context.IDFChar.Insulin_Concentration) & 0xFF;
  a_IDS_UpdateCharData[ids_char_length++] = ((IDS_APP_Context.IDFChar.Insulin_Concentration) >> 8) & 0xFF;
  a_IDS_UpdateCharData[ids_char_length++] =  (IDS_APP_Context.IDFChar.Flags) & 0xFF;
  a_IDS_UpdateCharData[ids_char_length++] = ((IDS_APP_Context.IDFChar.Flags) >> 8) & 0xFF;
  a_IDS_UpdateCharData[ids_char_length++] = ((IDS_APP_Context.IDFChar.Flags) >> 16) & 0xFF;
  if(((IDS_APP_Context.IDFChar.Flags) & E2E_PROTECTION_SUPPORTED) == (E2E_PROTECTION_SUPPORTED))
  {
    IDS_APP_Context.IDFChar.E2E_CRC =  (uint16_t)IDS_APP_ComputeCRC(&(a_IDS_UpdateCharData[3]), ids_char_length - 3);
    IDS_APP_Context.IDFChar.E2E_Counter += 1;
    a_IDS_UpdateCharData[0] = (uint8_t)((IDS_APP_Context.IDFChar.E2E_CRC) & 0xFF);
    a_IDS_UpdateCharData[1] = (uint8_t)(((IDS_APP_Context.IDFChar.E2E_CRC) >> 8) & 0xFF);
  }
  
  msg_conf.Length = ids_char_length;
  msg_conf.p_Payload = a_IDS_UpdateCharData;
  ret = IDS_UpdateValue(IDS_IDF, &msg_conf);
  if(ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("Feature update fails\n");
  }

} /* end of IDS_APP_UpdateFeature() */

/**
  * @brief Update Feature characteristic
  * @param None
  * @retval None
  */
uint32_t IDS_APP_GetFeature(void)
{
  return IDS_APP_Context.IDFChar.Flags;
} /* end of IDS_APP_GetFeature() */

/**
  * @brief Get the flag holding whether ID SRCP characteristic indication is enabled or not
  * @param None
  * @retval None
  */
uint8_t IDS_APP_GetIDSRCPCharacteristicIndicationEnabled(void)
{
  return ((IDS_APP_Context.Idsrcp_Indication_Status == Idsrcp_INDICATION_ON) ? TRUE: FALSE);
} /* end of IDS_APP_GetIDSRCPCharacteristicIndicationEnabled() */

/**
  * @brief Get the flag holding whether ID Status characteristic Reservoir Attahed or not
  * @param None
  * @retval None
  */
uint8_t IDS_APP_GetStatusFlagReservoirAttached(void)
{
  return ((((IDS_APP_Context.IDSChar.Flags) & RESERVOIR_ATTACHED) == RESERVOIR_ATTACHED) ? TRUE: FALSE);
} /* end of IDS_APP_GetStatusFlagReservoirAttached() */

/**
  * @brief Get pointeur on ID Status characteristic value
  * @param None
  * @retval None
  */
IDS_Status_Value_t * IDS_APP_GetStatus(void)
{
  return(&(IDS_APP_Context.IDSChar));
} /* end of IDS_APP_GetStatus() */

/**
  * @brief Get the flag holding whether ID Status characteristic Reservoir Attahed or not
  * @param None
  * @retval None
  */
void IDS_APP_SetStatus(IDS_Status_Value_t * pStatus)
{
  IDS_Data_t msg_conf;
  uint8_t length = 0;

  memcpy(&(IDS_APP_Context.IDSChar), pStatus, sizeof(IDS_Status_Value_t));

  length = 0;
  a_IDS_UpdateCharData[length++] =   IDS_APP_Context.IDSChar.Therapy_Control_State;
  a_IDS_UpdateCharData[length++] =   IDS_APP_Context.IDSChar.Operational_State;
  a_IDS_UpdateCharData[length++] =  (IDS_APP_Context.IDSChar.Reservoir_Remaining_Amount) & 0xFF;
  a_IDS_UpdateCharData[length++] = ((IDS_APP_Context.IDSChar.Reservoir_Remaining_Amount) >> 8) & 0xFF;
  a_IDS_UpdateCharData[length++] =   IDS_APP_Context.IDSChar.Flags;
  a_IDS_UpdateCharData[length++] = IDS_APP_Context.IDSChar.E2E_Counter;
  if(((IDS_APP_Context.IDFChar.Flags) & E2E_PROTECTION_SUPPORTED) == (E2E_PROTECTION_SUPPORTED))
  {
    IDS_APP_Context.IDSChar.E2E_CRC = (uint16_t)IDS_APP_ComputeCRC(a_IDS_UpdateCharData, length);
  }
  a_IDS_UpdateCharData[length++] = (uint8_t)((IDS_APP_Context.IDSChar.E2E_CRC) & 0xFF);
  a_IDS_UpdateCharData[length++] = (uint8_t)(((IDS_APP_Context.IDSChar.E2E_CRC) >> 8) & 0xFF);

  msg_conf.Length = length;
  msg_conf.p_Payload = a_IDS_UpdateCharData;
  IDS_UpdateValue(IDS_IDS, &msg_conf);  
} /* end of IDS_APP_GetStatusFlagReservoirAttached() */

/**
  * @brief Get pointeur on ID Annunciation Status characteristic value
  * @param None
  * @retval None
  */
IDS_Annunciation_Status_Value_t * IDS_APP_GetAnnunciationStatus(void)
{
  return(&(IDS_APP_Context.IDASChar));
} /* end of IDS_APP_GetAnnunciationStatus() */

/**
  * @brief Get pointeur on ID Annunciation Status characteristic value
  * @param None
  * @retval None
  */
void IDS_APP_SetAnnunciationStatus(IDS_Annunciation_Status_Value_t * pAnnunciationStatus)
{
  IDS_Data_t msg_conf;
  uint8_t length = 0;

  memcpy(&(IDS_APP_Context.IDASChar), pAnnunciationStatus, sizeof(IDS_Annunciation_Status_Value_t));

  length = 0;
  a_IDS_UpdateCharData[length++] = IDS_APP_Context.IDASChar.Flags;
  a_IDS_UpdateCharData[length++] = (uint8_t)((IDS_APP_Context.IDASChar.Instance_ID) & 0xFF);
  a_IDS_UpdateCharData[length++] = (uint8_t)(((IDS_APP_Context.IDASChar.Instance_ID) >> 8) & 0xFF);
  a_IDS_UpdateCharData[length++] = (uint8_t)((IDS_APP_Context.IDASChar.Type) & 0xFF);
  a_IDS_UpdateCharData[length++] = (uint8_t)(((IDS_APP_Context.IDASChar.Type) >> 8) & 0xFF);
  a_IDS_UpdateCharData[length++] = IDS_APP_Context.IDASChar.Status;
  if(((IDS_APP_Context.IDASChar.Flags) & AUXINFO1_PRESENT) == AUXINFO1_PRESENT)
  {
    a_IDS_UpdateCharData[length++] = (uint8_t)((IDS_APP_Context.IDASChar.Aux_Info[0]) & 0xFF);
    a_IDS_UpdateCharData[length++] = (uint8_t)(((IDS_APP_Context.IDASChar.Aux_Info[0]) >> 8) & 0xFF);
    if(((IDS_APP_Context.IDASChar.Flags) & AUXINFO2_PRESENT) == AUXINFO2_PRESENT)
    {
      a_IDS_UpdateCharData[length++] = (uint8_t)((IDS_APP_Context.IDASChar.Aux_Info[1]) & 0xFF);
      a_IDS_UpdateCharData[length++] = (uint8_t)(((IDS_APP_Context.IDASChar.Aux_Info[1]) >> 8) & 0xFF);
      if(((IDS_APP_Context.IDASChar.Flags) & AUXINFO3_PRESENT) == AUXINFO3_PRESENT)
      {
        a_IDS_UpdateCharData[length++] = (uint8_t)((IDS_APP_Context.IDASChar.Aux_Info[2]) & 0xFF);
        a_IDS_UpdateCharData[length++] = (uint8_t)(((IDS_APP_Context.IDASChar.Aux_Info[2]) >> 8) & 0xFF);
        if(((IDS_APP_Context.IDASChar.Flags) & AUXINFO4_PRESENT) == AUXINFO4_PRESENT)
        {
          a_IDS_UpdateCharData[length++] = (uint8_t)((IDS_APP_Context.IDASChar.Aux_Info[3]) & 0xFF);
          a_IDS_UpdateCharData[length++] = (uint8_t)(((IDS_APP_Context.IDASChar.Aux_Info[3]) >> 8) & 0xFF);
          if(((IDS_APP_Context.IDASChar.Flags) & AUXINFO5_PRESENT) == AUXINFO5_PRESENT)
          {
            a_IDS_UpdateCharData[length++] = (uint8_t)((IDS_APP_Context.IDASChar.Aux_Info[4]) & 0xFF);
            a_IDS_UpdateCharData[length++] = (uint8_t)(((IDS_APP_Context.IDASChar.Aux_Info[4]) >> 8) & 0xFF);
          }
        }
      }
    }
  }
  a_IDS_UpdateCharData[length++] = IDS_APP_Context.IDASChar.E2E_Counter;

  if(((IDS_APP_Context.IDFChar.Flags) & E2E_PROTECTION_SUPPORTED) == (E2E_PROTECTION_SUPPORTED))
  {
    IDS_APP_Context.IDASChar.E2E_CRC = (uint16_t)IDS_APP_ComputeCRC(a_IDS_UpdateCharData, length);
  }
  a_IDS_UpdateCharData[length++] = (uint8_t)((IDS_APP_Context.IDASChar.E2E_CRC) & 0xFF);
  a_IDS_UpdateCharData[length++] = (uint8_t)(((IDS_APP_Context.IDASChar.E2E_CRC) >> 8) & 0xFF);

  msg_conf.Length = length;
  msg_conf.p_Payload = a_IDS_UpdateCharData;
  IDS_UpdateValue(IDS_IDAS, &msg_conf);
} /* end of IDS_APP_GetAnnunciationStatus() */

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
__USED void IDS_Idsc_SendIndication(void) /* Property Indication */
{
  IDS_APP_SendInformation_t indication_on_off = Idsc_INDICATION_OFF;
  IDS_Data_t ids_indication_data;

  ids_indication_data.p_Payload = (uint8_t*)a_IDS_UpdateCharData;
  ids_indication_data.Length = 0;

  /* USER CODE BEGIN Service4Char1_IS_1*/

  /* USER CODE END Service4Char1_IS_1*/

  if (indication_on_off != Idsc_INDICATION_OFF)
  {
    IDS_UpdateValue(IDS_IDSC, &ids_indication_data);
  }

  /* USER CODE BEGIN Service4Char1_IS_Last*/

  /* USER CODE END Service4Char1_IS_Last*/

  return;
}

__USED void IDS_Ids_SendIndication(void) /* Property Indication */
{
  IDS_APP_SendInformation_t indication_on_off = Ids_INDICATION_OFF;
  IDS_Data_t ids_indication_data;

  ids_indication_data.p_Payload = (uint8_t*)a_IDS_UpdateCharData;
  ids_indication_data.Length = 0;

  /* USER CODE BEGIN Service4Char2_IS_1*/

  /* USER CODE END Service4Char2_IS_1*/

  if (indication_on_off != Ids_INDICATION_OFF)
  {
    IDS_UpdateValue(IDS_IDS, &ids_indication_data);
  }

  /* USER CODE BEGIN Service4Char2_IS_Last*/

  /* USER CODE END Service4Char2_IS_Last*/

  return;
}

__USED void IDS_Idas_SendIndication(void) /* Property Indication */
{
  IDS_APP_SendInformation_t indication_on_off = Idas_INDICATION_OFF;
  IDS_Data_t ids_indication_data;

  ids_indication_data.p_Payload = (uint8_t*)a_IDS_UpdateCharData;
  ids_indication_data.Length = 0;

  /* USER CODE BEGIN Service4Char3_IS_1*/

  /* USER CODE END Service4Char3_IS_1*/

  if (indication_on_off != Idas_INDICATION_OFF)
  {
    IDS_UpdateValue(IDS_IDAS, &ids_indication_data);
  }

  /* USER CODE BEGIN Service4Char3_IS_Last*/

  /* USER CODE END Service4Char3_IS_Last*/

  return;
}

__USED void IDS_Idf_SendIndication(void) /* Property Indication */
{
  IDS_APP_SendInformation_t indication_on_off = Idf_INDICATION_OFF;
  IDS_Data_t ids_indication_data;

  ids_indication_data.p_Payload = (uint8_t*)a_IDS_UpdateCharData;
  ids_indication_data.Length = 0;

  /* USER CODE BEGIN Service4Char4_IS_1*/

  /* USER CODE END Service4Char4_IS_1*/

  if (indication_on_off != Idf_INDICATION_OFF)
  {
    IDS_UpdateValue(IDS_IDF, &ids_indication_data);
  }

  /* USER CODE BEGIN Service4Char4_IS_Last*/

  /* USER CODE END Service4Char4_IS_Last*/

  return;
}

__USED void IDS_Idsrcp_SendIndication(void) /* Property Indication */
{
  IDS_APP_SendInformation_t indication_on_off = Idsrcp_INDICATION_OFF;
  IDS_Data_t ids_indication_data;

  ids_indication_data.p_Payload = (uint8_t*)a_IDS_UpdateCharData;
  ids_indication_data.Length = 0;

  /* USER CODE BEGIN Service4Char5_IS_1*/

  /* USER CODE END Service4Char5_IS_1*/

  if (indication_on_off != Idsrcp_INDICATION_OFF)
  {
    IDS_UpdateValue(IDS_IDSRCP, &ids_indication_data);
  }

  /* USER CODE BEGIN Service4Char5_IS_Last*/

  /* USER CODE END Service4Char5_IS_Last*/

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/
#if 0
/**
* @brief Send a SRCP response as consequence of a SRCP request
* @param Opcode: SRCP op code
* @param responseCode: SRCP response code or number of stored records
* @retval None
*/
static tBleStatus IDS_Srcp_SendResponseCode(uint16_t Opcode, uint8_t responseCode)
{
  tBleStatus retval = BLE_STATUS_FAILED;
  IDS_Data_t msg_conf;
  uint8_t length = 0;

  switch(Opcode)
  {
    case RESET_STATUS:
      {
        length = 0;
        a_IDS_UpdateCharData[length++] = (RESPONSE_CODE & 0xFF);
        a_IDS_UpdateCharData[length++] = ((RESPONSE_CODE >> 8) & 0xFF);
        a_IDS_UpdateCharData[length++] = (Opcode & 0xFF);;
        a_IDS_UpdateCharData[length++] = ((Opcode >> 8) & 0xFF);
        a_IDS_UpdateCharData[length++] = responseCode;
        a_IDS_UpdateCharData[length++] = IDS_APP_Context.IDSRCPChar.E2E_Counter;
        if(((IDS_APP_Context.IDFChar.Flags) & E2E_PROTECTION_SUPPORTED) == (E2E_PROTECTION_SUPPORTED))
        {
          IDS_APP_Context.IDSRCPChar.E2E_CRC = IDS_APP_ComputeCRC(a_IDS_UpdateCharData, length);
          LOG_INFO_APP("E2E_CRC = 0x%x\r\n", IDS_APP_Context.IDSRCPChar.E2E_CRC);
        }
        else
        {
          IDS_APP_Context.IDSRCPChar.E2E_CRC = 0xFFFF;
          LOG_INFO_APP("E2E_CRC = 0x%x\r\n", IDS_APP_Context.IDSRCPChar.E2E_CRC);
        }
        a_IDS_UpdateCharData[length++] = (uint8_t)((IDS_APP_Context.IDSRCPChar.E2E_CRC) & 0xFF);
        a_IDS_UpdateCharData[length++] = (uint8_t)(((IDS_APP_Context.IDSRCPChar.E2E_CRC) >> 8) & 0xFF);

        msg_conf.Length = length;
        msg_conf.p_Payload = a_IDS_UpdateCharData;
        retval = IDS_UpdateValue(IDS_IDSRCP, &msg_conf);
      }
      break;
  
    case GET_ACTIVE_BOLUS_IDS:
      {
        length = 0;
        a_IDS_UpdateCharData[length++] = (GET_ACTIVE_BOLUS_IDS_RESPONSE & 0xFF);
        a_IDS_UpdateCharData[length++] = ((GET_ACTIVE_BOLUS_IDS_RESPONSE >> 8) & 0xFF);
        a_IDS_UpdateCharData[length++] = NB_ACTIVE_BOLUSES_IDS;
        for(uint8_t i = 0; i < NB_ACTIVE_BOLUSES_IDS; i++)
        {
          if(IDS_ActiveBolusesIDs[i].State > 0)
          {
            a_IDS_UpdateCharData[length++] = (IDS_ActiveBolusesIDs[i].Id & 0xFF);
            a_IDS_UpdateCharData[length++] = ((IDS_ActiveBolusesIDs[i].Id >> 8) & 0xFF);
          }
        }
        a_IDS_UpdateCharData[length++] = IDS_APP_Context.IDSRCPChar.E2E_Counter;
        if(((IDS_APP_Context.IDFChar.Flags) & E2E_PROTECTION_SUPPORTED) == (E2E_PROTECTION_SUPPORTED))
        {
          IDS_APP_Context.IDSRCPChar.E2E_CRC = (uint16_t)IDS_APP_ComputeCRC(a_IDS_UpdateCharData, length);
        }
        else
        {
          IDS_APP_Context.IDSRCPChar.E2E_CRC = 0xFFFF;
        }
        a_IDS_UpdateCharData[length++] = (uint8_t)((IDS_APP_Context.IDSRCPChar.E2E_CRC) & 0xFF);
        a_IDS_UpdateCharData[length++] = (uint8_t)(((IDS_APP_Context.IDSRCPChar.E2E_CRC) >> 8) & 0xFF);

        msg_conf.Length = length;
        msg_conf.p_Payload = a_IDS_UpdateCharData;
        retval = IDS_UpdateValue(IDS_IDSRCP, &msg_conf);
      }
      break;
  
    case GET_ACTIVE_BOLUS_DELIVERY:
      {
        length = 0;
        a_IDS_UpdateCharData[length++] = (GET_ACTIVE_BOLUS_DELIVERY_RESPONSE & 0xFF);
        a_IDS_UpdateCharData[length++] = ((GET_ACTIVE_BOLUS_DELIVERY_RESPONSE >> 8) & 0xFF);
        for(uint8_t i = 0; i < NB_ACTIVE_BOLUSES_IDS; i++)
        {
          if((IDS_ActiveBolusesIDs[i].State > 0)/* &&
             (IDS_ActiveBolusesIDs[i].Id == */)
          {
            a_IDS_UpdateCharData[length++] = IDS_ActiveBolusesIDs[i].Flags;
            a_IDS_UpdateCharData[length++] = (IDS_ActiveBolusesIDs[i].Id & 0xFF);
            a_IDS_UpdateCharData[length++] = ((IDS_ActiveBolusesIDs[i].Id >> 8) & 0xFF);
            a_IDS_UpdateCharData[length++] = IDS_ActiveBolusesIDs[i].Type;
            a_IDS_UpdateCharData[length++] = (IDS_ActiveBolusesIDs[i].FastAmount & 0xFF);
            a_IDS_UpdateCharData[length++] = ((IDS_ActiveBolusesIDs[i].FastAmount >> 8) & 0xFF);
            a_IDS_UpdateCharData[length++] = (IDS_ActiveBolusesIDs[i].ExtAmount & 0xFF);
            a_IDS_UpdateCharData[length++] = ((IDS_ActiveBolusesIDs[i].ExtAmount >> 8) & 0xFF);
            a_IDS_UpdateCharData[length++] = (IDS_ActiveBolusesIDs[i].Duration & 0xFF);
            a_IDS_UpdateCharData[length++] = ((IDS_ActiveBolusesIDs[i].Duration >> 8) & 0xFF);
            if((IDS_ActiveBolusesIDs[i].Flags & DELAY_TIME_PRESENT) == DELAY_TIME_PRESENT)
            {
              a_IDS_UpdateCharData[length++] = (IDS_ActiveBolusesIDs[i].DelayTime & 0xFF);
              a_IDS_UpdateCharData[length++] = ((IDS_ActiveBolusesIDs[i].DelayTime >> 8) & 0xFF);
            }
            if((IDS_ActiveBolusesIDs[i].Flags & TEMPLATE_NUMBER_PRESENT) == TEMPLATE_NUMBER_PRESENT)
            {
              a_IDS_UpdateCharData[length++] = (IDS_ActiveBolusesIDs[i].TemplateNumber & 0xFF);
              a_IDS_UpdateCharData[length++] = ((IDS_ActiveBolusesIDs[i].TemplateNumber >> 8) & 0xFF);
            }
            if((IDS_ActiveBolusesIDs[i].Flags & ACTIVATION_TYPE_PRESENT) == ACTIVATION_TYPE_PRESENT)
            {
              a_IDS_UpdateCharData[length++] = (IDS_ActiveBolusesIDs[i].ActivationType & 0xFF);
              a_IDS_UpdateCharData[length++] = ((IDS_ActiveBolusesIDs[i].ActivationType >> 8) & 0xFF);
            }
          }
        }
        a_IDS_UpdateCharData[length++] = IDS_APP_Context.IDSRCPChar.E2E_Counter;
        if(((IDS_APP_Context.IDFChar.Flags) & E2E_PROTECTION_SUPPORTED) == (E2E_PROTECTION_SUPPORTED))
        {
          IDS_APP_Context.IDSRCPChar.E2E_CRC = (uint16_t)IDS_APP_ComputeCRC(a_IDS_UpdateCharData, length);
        }
        else
        {
          IDS_APP_Context.IDSRCPChar.E2E_CRC = 0xFFFF;
        }
        a_IDS_UpdateCharData[length++] = (uint8_t)((IDS_APP_Context.IDSRCPChar.E2E_CRC) & 0xFF);
        a_IDS_UpdateCharData[length++] = (uint8_t)(((IDS_APP_Context.IDSRCPChar.E2E_CRC) >> 8) & 0xFF);
            
        msg_conf.Length = length;
        msg_conf.p_Payload = a_IDS_UpdateCharData;
        retval = IDS_UpdateValue(IDS_IDSRCP, &msg_conf);
      }
      break;
  
    case GET_ACTIVE_BASAL_RATE_DELIVERY:
      {
#if 0
        length = 0;
        a_IDS_UpdateCharData[length++] = (GET_ACTIVE_BASAL_RATE_DELIVERY_RESPONSE & 0xFF);
        a_IDS_UpdateCharData[length++] = ((GET_ACTIVE_BASAL_RATE_DELIVERY_RESPONSE >> 8) & 0xFF);
        a_IDS_UpdateCharData[length++] =   IDS_BasalRate.Flags;
        a_IDS_UpdateCharData[length++] =   IDS_BasalRate.TemplateNumber;
        a_IDS_UpdateCharData[length++] =  (IDS_BasalRate.CurrentCfgVal & 0xFF);
        a_IDS_UpdateCharData[length++] = ((IDS_BasalRate.CurrentCfgVal >> 8) & 0xFF);
        if(IDS_Bolus.Flags & TBR_PRESENT == TBR_PRESENT)
        {
          a_IDS_UpdateCharData[length++] =   IDS_BasalRate.TBRType;
          a_IDS_UpdateCharData[length++] = ( IDS_Bolus.TBRAdjVal & 0xFF);
          a_IDS_UpdateCharData[length++] = ((IDS_Bolus.TBRAdjVal >> 8) & 0xFF);
          a_IDS_UpdateCharData[length++] = ( IDS_Bolus.TBRDurPrg & 0xFF);
          a_IDS_UpdateCharData[length++] = ((IDS_Bolus.TBRDurPrg >> 8) & 0xFF);
          a_IDS_UpdateCharData[length++] = ( IDS_Bolus.TBRDurRem & 0xFF);
          a_IDS_UpdateCharData[length++] = ((IDS_Bolus.TBRDurRem >> 8) & 0xFF);
        }
        if(IDS_Bolus.Flags & TBR_TEMPLATE_NUMBER_PRESENT == TBR_TEMPLATE_NUMBER_PRESENT)
        {
          a_IDS_UpdateCharData[length++] =   IDS_BasalRate.TBRTemplateNum;
        }
        if(IDS_Bolus.Flags & BASAL_DELIVERY_CONTEXT_PRESENT == BASAL_DELIVERY_CONTEXT_PRESENT)
        {
          a_IDS_UpdateCharData[length++] =   IDS_BasalRate.DeliveryCtx;
        }
        msg_conf.Length = length;
        msg_conf.p_Payload = a_IDS_UpdateCharData;
        retval = IDS_UpdateValue(IDS_IDSRCP, &msg_conf);
#endif
      }
      break;
  
    case GET_TOTAL_DAILY_INSULIN_STATUS:
      {
#if 0
        length = 0;
        a_IDS_UpdateCharData[length++] = (GET_TOTAL_DAILY_INSULIN_STATUS_RESPONSE & 0xFF);
        a_IDS_UpdateCharData[length++] = ((GET_TOTAL_DAILY_INSULIN_STATUS_RESPONSE >> 8) & 0xFF);
        a_IDS_UpdateCharData[length++] =   IDS_BasalRate.Flags;
        a_IDS_UpdateCharData[length++] =   IDS_BasalRate.TemplateNumber;
        a_IDS_UpdateCharData[length++] =  (IDS_TotalDailyInsulinSumBolusDelivered & 0xFF);
        a_IDS_UpdateCharData[length++] = ((IDS_TotalDailyInsulinSumBolusDelivered >> 8) & 0xFF);
        a_IDS_UpdateCharData[length++] =  (IDS_TotalDailyInsulinSumBasalDelivered & 0xFF);
        a_IDS_UpdateCharData[length++] = ((IDS_TotalDailyInsulinSumBasalDelivered >> 8) & 0xFF);
        a_IDS_UpdateCharData[length++] =  (IDS_TotalDailyInsulinSumBolusBasalDelivered & 0xFF);
        a_IDS_UpdateCharData[length++] = ((IDS_TotalDailyInsulinSumBolusBasalDelivered >> 8) & 0xFF);
        msg_conf.Length = length;
        msg_conf.p_Payload = a_IDS_UpdateCharData;
        retval = IDS_UpdateValue(IDS_IDSRCP, &msg_conf);
#endif
      }
      break;
  
    case GET_COUNTER:
      {
#if 0
        length = 0;
        a_IDS_UpdateCharData[length++] = (GET_COUNTER_RESPONSE & 0xFF);
        a_IDS_UpdateCharData[length++] = ((GET_COUNTER_RESPONSE >> 8) & 0xFF);
        a_IDS_UpdateCharData[length++] =   IDS_Counter.Type;
        a_IDS_UpdateCharData[length++] =   IDS_Counter.ValSel;
        a_IDS_UpdateCharData[length++] =  (IDS_Counter.Value & 0xFF);
        a_IDS_UpdateCharData[length++] = ((IDS_Counter.Value >> 8) & 0xFF);
        msg_conf.Length = length;
        msg_conf.p_Payload = a_IDS_UpdateCharData;
        retval = IDS_UpdateValue(IDS_IDSRCP, &msg_conf);
#endif
      }
      break;
  
    case GET_DELIVERED_INSULIN:
      {
#if 0
        length = 0;
        a_IDS_UpdateCharData[length++] = ( GET_DELIVERED_INSULIN_RESPONSE & 0xFF);
        a_IDS_UpdateCharData[length++] = ((GET_DELIVERED_INSULIN_RESPONSE >> 8) & 0xFF);
        a_IDS_UpdateCharData[length++] =  (IDS_BolusAmount & 0xFF);
        a_IDS_UpdateCharData[length++] = ((IDS_BolusAmount >> 8) & 0xFF);
        a_IDS_UpdateCharData[length++] =  (IDS_BaslAmount & 0xFF);
        a_IDS_UpdateCharData[length++] = ((IDS_BaslAmount >> 8) & 0xFF);
        msg_conf.Length = length;
        msg_conf.p_Payload = a_IDS_UpdateCharData;
        retval = IDS_UpdateValue(IDS_IDSRCP, &msg_conf);
#endif
      }
      break;
  
    case GET_INSULIN_ON_BOARD:
      {
#if 0
        length = 0;
        a_IDS_UpdateCharData[length++] = ( GET_INSULIN_ON_BOARD_RESPONSE & 0xFF);
        a_IDS_UpdateCharData[length++] = ((GET_INSULIN_ON_BOARD_RESPONSE >> 8) & 0xFF);
        a_IDS_UpdateCharData[length++] =   IDS_InsulinOnBoard.Flags;
        a_IDS_UpdateCharData[length++] =  (IDS_InsulinOnBoard.Value & 0xFF);
        a_IDS_UpdateCharData[length++] = ((IDS_InsulinOnBoard.Value >> 8) & 0xFF);
        if(IDS_InsulinOnBoard.Flags & ON_BOARD_REM_DUR_PRESENT == ON_BOARD_REM_DUR_PRESENT)
        {
          a_IDS_UpdateCharData[length++] =  (IDS_InsulinOnBoard.RemDur & 0xFF);
          a_IDS_UpdateCharData[length++] = ((IDS_InsulinOnBoard.RemDur >> 8) & 0xFF);
        }
        msg_conf.Length = length;
        msg_conf.p_Payload = a_IDS_UpdateCharData;
        retval = IDS_UpdateValue(IDS_IDSRCP, &msg_conf);
#endif
      }
      break;
  
    default:
      break;
  }
  return retval;
} /* end of IDS_Srcp_SendResponseCode() */
#endif
/* USER CODE END FD_LOCAL_FUNCTIONS*/
