/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service4.h
  * @author  MCD Application Team
  * @brief   Header for service4.c
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
#ifndef IDS_H
#define IDS_H

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
  IDS_IDSC,
  IDS_IDS,
  IDS_IDAS,
  IDS_IDF,
  IDS_IDSRCP,
  /* USER CODE BEGIN Service4_CharOpcode_t */

  /* USER CODE END Service4_CharOpcode_t */
  IDS_CHAROPCODE_LAST
} IDS_CharOpcode_t;

typedef enum
{
  IDS_IDSC_READ_EVT,
  IDS_IDSC_INDICATE_ENABLED_EVT,
  IDS_IDSC_INDICATE_DISABLED_EVT,
  IDS_IDS_READ_EVT,
  IDS_IDS_INDICATE_ENABLED_EVT,
  IDS_IDS_INDICATE_DISABLED_EVT,
  IDS_IDAS_READ_EVT,
  IDS_IDAS_INDICATE_ENABLED_EVT,
  IDS_IDAS_INDICATE_DISABLED_EVT,
  IDS_IDF_READ_EVT,
  IDS_IDF_INDICATE_ENABLED_EVT,
  IDS_IDF_INDICATE_DISABLED_EVT,
  IDS_IDSRCP_WRITE_EVT,
  IDS_IDSRCP_INDICATE_ENABLED_EVT,
  IDS_IDSRCP_INDICATE_DISABLED_EVT,
  /* USER CODE BEGIN Service4_OpcodeEvt_t */

  /* USER CODE END Service4_OpcodeEvt_t */
  IDS_BOOT_REQUEST_EVT
} IDS_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service4_Data_t */

  /* USER CODE END Service4_Data_t */
} IDS_Data_t;

typedef struct
{
  IDS_OpcodeEvt_t       EvtOpcode;
  IDS_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;
  /* USER CODE BEGIN Service4_NotificationEvt_t */

  /* USER CODE END Service4_NotificationEvt_t */
} IDS_NotificationEvt_t;

/* USER CODE BEGIN ET */
typedef enum
{
  NO_SC_FLAG = 0,
  THERAPY_CONTROL_STATE_CHANGED = (1<<0),
  OPERATIONAL_STATE_CHANGED = (1<<1),
  RESERVOIR_STATUS_CHANGED = (1<<2),
  ANNUNCIATION_STATUS_CHANGED = (1<<3),
  TOTAL_DAILY_INSULIN_STATUS_CHANGED = (1<<4),
  ACTIVE_BASAL_RATE_STATUS_CHANGED = (1<<5),
  ACTIVE_BOLUS_STATUS_CHANGED = (1<<6),
  HISTORY_EVENT_RECORDED = (1<<7)
} IDS_SC_Flags_t;

typedef enum
{
  NO_SATUS_FLAG = 0,
  RESERVOIR_ATTACHED = (1<<0)
} IDS_Status_Flags_t;

typedef enum
{
  NO_ANNUNCIATION_FLAG = 0,
  ANNUNCIATION_PRESENT = (1<<0),
  AUXINFO1_PRESENT = (1<<1),
  AUXINFO2_PRESENT = (1<<2),
  AUXINFO3_PRESENT = (1<<3),
  AUXINFO4_PRESENT = (1<<4),
  AUXINFO5_PRESENT = (1<<5)
} IDS_Annunciation_Flags_t;

typedef enum
{
  UNDETERMINED                = 0x0F,
  PENDING                     = 0x33,
  SNOOZED                     = 0x3C,
  CONFIRMED                   = 0x55
} IDS_APP_Annunciation_Status_t;

typedef enum
{
  NO_FEATURE_FLAG = 0,
  E2E_PROTECTION_SUPPORTED = (1<<0),
  BASAL_RATE_SUPPORTED = (1<<1),
  TBR_ABSOLUTE_SUPPORTED = (1<<2),
  TBR_REALTIVE_SUPPORTED = (1<<3),
  TBR_TEMPLATE_SUPPORTED = (1<<4),
  FAST_BOLUS_SUPPORTED = (1<<5),
  EXTENDED_BOLUS_SUPPORTED = (1<<6),
  MULTIWAVE_BOLUS_SUPPORTED = (1<<7),
  BOLUS_DELAY_TIME_SUPPORTED = (1<<8),
  BOLUS_TEMPLATE_SUPPORTED = (1<<9),
  BOLUS_ACTIVATION_TYPE_SUPPORTED = (1<<10),
  MULTIPLE_BOND_SUPPORTED = (1<<11),
  ISF_PROFILE_TEMPLATE_SUPPORTED = (1<<12),
  I2CHO_RATIO_PTOFILE_TEMPLATE_SUPPORTED = (1<<13),
  TARGET_GLUCOSE_RANGE_PROFILE_TEMPLATE_SUPPORTED = (1<<14),
  INSULIN_ON_BOARD_SUPPORTED = (1<<15),
  FEATURE_EXTENSION = (1<<23)
} IDS_Feature_Flags_t;

typedef struct
{
  uint16_t Flags;
  uint8_t E2E_Counter;
  uint16_t E2E_CRC;
}IDS_Status_Changed_Value_t;

typedef struct
{
  uint8_t Therapy_Control_State;
  uint8_t Operational_State;
  uint16_t Reservoir_Remaining_Amount;
  uint8_t Flags;
  uint8_t E2E_Counter;
  uint16_t E2E_CRC;
}IDS_Status_Value_t;

typedef struct
{
  uint8_t Flags;
  uint16_t Instance_ID;
  uint16_t Type;
  uint8_t Status;
  uint16_t Aux_Info[5];
  uint8_t E2E_Counter;
  uint16_t E2E_CRC;
}IDS_Annunciation_Status_Value_t;

typedef struct
{
  uint16_t E2E_CRC;
  uint8_t E2E_Counter;
  uint16_t Insulin_Concentration;
  uint32_t Flags;
}IDS_Feature_Value_t;

typedef struct
{
  uint16_t Flag;
  uint16_t OpCode;
  uint8_t Operand[15];
  uint8_t E2E_Counter;
  uint16_t E2E_CRC;
  uint8_t Response;
}IDS_StatusRCP_Value_t;

typedef struct
{
  uint16_t OpCode;
  uint8_t Operand[15];
  uint8_t E2E_Counter;
  uint16_t E2E_CRC;
}IDS_CCP_Value_t;

typedef struct
{
  uint16_t Response_OpCode;
  uint8_t Operand[15];
  uint8_t E2E_Counter;
  uint16_t E2E_CRC;
}IDS_CD_Value_t;

typedef struct
{
  uint8_t OpCode;
  uint8_t Operator;
  uint8_t Operand[18];
}IDS_RACP_Value_t;

typedef struct
{
  uint16_t Year;
  uint8_t Month;
  uint8_t Day;
  uint8_t Hours;
  uint8_t Minutes;
  uint8_t Seconds;
}IDS_Date_Time_t;

typedef struct
{
  uint8_t RecordingReason;
  IDS_Date_Time_t DateTime;
  uint8_t TimeZone;
  uint8_t DSTOffset;
}IDS_Event_Data_t;

typedef struct
{
  uint16_t EventType;
  uint32_t SeqNum;
  uint16_t RelativeOffset;
  IDS_Event_Data_t EventData[10];
  uint16_t E2E_CRC;
}IDS_HD_Value_t;

#if 0 
typedef enum
{
  NO_ON_BOARD_FLAG = 0,
  ON_BOARD_REM_DUR_PRESENT = (1<<0)
} IDS_OnBoard.Flags_t;
#endif
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
/* New values defined on Core spec supplement (CSS v4), common profile and service error code
   It is used on PTS 5.3.0 */
#define IDS_ATT_ERROR_CODE_PROCEDURE_ALREADY_IN_PROGRESS                  (0xFE)
/* New values defined on Core spec supplement (CSS v4), common profile and service error code
   It is used on PTS 5.3.0 */
#define IDS_ATT_ERROR_CODE_PROCEDURE_ALREADY_IN_PROGRESS                  (0xFE)
  
/* New values defined on Core spec supplement (CSS v4), common profile and service error code
   It is used on PTS 5.3.0 */
#define IDS_ATT_ERROR_CODE_CLIENT_CHAR_CONF_DESC_IMPROPERLY_CONFIGURED    (0xFD)
#define IDS_ATT_ERROR_CODE_OPCODE_NOT_SUPPORTED                           (0x70)                  
#define IDS_ATT_ERROR_CODE_INVALIDE_OPERAND                               (0x71)                  
#define IDS_ATT_ERROR_CODE_PROCEDURE_NOT_COMPLETED                        (0x72)                  
#define IDS_ATT_ERROR_CODE_PARAMETER_OUT_OF_RANGE                         (0x73)                  
#define IDS_ATT_ERROR_CODE_PROCEDURE_NOT_APPLICABLE                       (0x74)                  
#define IDS_ATT_ERROR_CODE_MISSING_CRC                                    (0x80)
#define IDS_ATT_ERROR_CODE_INVALID_CRC                                    (0x81)
#define IDS_ATT_ERROR_CODE_INVALID_COUNTER                                (0x82)
#define IDS_ATT_ERROR_CODE_OUT_OF_RANGE                                   (0xFF)
  
#define NB_ACTIVE_BOLUSES_IDS                                                (2)
/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros -----------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void IDS_Init(void);
void IDS_Notification(IDS_NotificationEvt_t *p_Notification);
tBleStatus IDS_UpdateValue(IDS_CharOpcode_t CharOpcode, IDS_Data_t *pData);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*IDS_H */
