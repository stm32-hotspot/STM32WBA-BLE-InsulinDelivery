/**
  ******************************************************************************
  * @file    ids_srcp.c
  * @author  MCD Application Team
  * @brief   IDS SRCP
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

/* Includes ------------------------------------------------------------------*/
#include "common_blesvc.h"
#include "stm32_seq.h"
#include "stm32_timer.h"
#include "ids.h"
#include "ids_srcp.h"
#include "ids_app.h"

/******************************************************************************
* Macro Declarations
******************************************************************************/
/* Private typedef -----------------------------------------------------------*/  
/* ID Status Read Control Point: Op Code Values */
/* SRCP requests */
#define IDS_SRCP_RESET_STATUS                                             0x030C
#define IDS_SRCP_GET_ACTIVE_BOLUS_IDS                                     0x0330
#define IDS_SRCP_GET_ACTIVE_BOLUS_DELIVERY                                0x0356
#define IDS_SRCP_GET_ACTIVE_BASAL_RATE_DELIVERY                           0x0365
#define IDS_SRCP_GET_TOTAL_DAILY_INSULIN_STATUS                           0x0395
#define IDS_SRCP_GET_COUNTER                                              0x03A6
#define IDS_SRCP_GET_DELIVERED_INSULIN                                    0x03C0
#define IDS_SRCP_GET_INSULIN_ON_BOARD                                     0x03F3
  
/* SRCP responses */
/* Mandatory */
#define IDS_SRCP_RESPONSE_CODE                                            0x0303
#define IDS_SRCP_GET_ACTIVE_BOLUS_IDS_RESPONSE                            0x033F
#define IDS_SRCP_GET_ACTIVE_BOLUS_DELIVERY_RESPONSE                       0x0359
#define IDS_SRCP_GET_ACTIVE_BASAL_RATE_DELIVERY_RESPONSE                  0x036A
#define IDS_SRCP_GET_TOTAL_DAILY_INSULIN_STATUS_RESPONSE                  0x039A
#define IDS_SRCP_GET_COUNTER_RESPONSE                                     0x03A9
#define IDS_SRCP_GET_DELIVERED_INSULIN_RESPONSE                           0x03CF
#define IDS_SRCP_GET_INSULIN_ON_BOARD_RESPONSE                            0x03FC
  
/* ID Status Read Control Point: types valid length */
#define IDS_SRCP_NO_OPERAND_REQUEST_LENGTH                                   (5)
#define IDS_SRCP_RESET_REQUEST_LENGTH                                        (7)
#define IDS_SRCP_GET_ACTIVE_BOLUS_DELIVERY_REQUEST_LENGTH                    (8)
#define IDS_SRCP_GET_ACTIVE_COUNTER_REQUEST_LENGTH                           (7)
#define IDS_SRCP_GET_ACTIVE_GET_DELIVERED_INSULIN_REQUEST_LENGTH            (10)
#define IDS_SRCP_FILTER_TYPE_MAX_LENGTH                                     (20)
  
/* CGM Specific Ops Control Point: fields position */
#define IDS_SRCP_OP_CODE_LENGTH                                              (2) 
#define IDS_SRCP_OP_CODE_POSITION                                            (0)
#define IDS_SRCP_OPERAND_POSITION                      (IDS_SRCP_OP_CODE_LENGTH)

/* CGM Specific Ops Control Point: responses values */
#define IDS_SRCP_ERROR_CODE_SUCCESS                                       (0x0F)
#define IDS_SRCP_ERROR_CODE_OPCODE_NOT_SUPPORTED                          (0x70)
#define IDS_SRCP_ERROR_CODE_INVALID_OPERAND                               (0x71)
#define IDS_SRCP_ERROR_CODE_PROCEDURE_NOT_COMPLETED                       (0x72)
#define IDS_SRCP_ERROR_CODE_PARAMETER_OUT_OF_RANGE                        (0x73)
#define IDS_SRCP_ERROR_CODE_PROCEDURE_NOT_APPLICABLE                      (0x74)

typedef enum
{
  NO_BOLUS_FLAG = 0,
  DELAY_TIME_PRESENT         = (1<<0),
  TEMPLATE_NUMBER_PRESENT    = (1<<1),
  ACTIVATION_TYPE_PRESENT    = (1<<2),
  DELIVERY_REASON_CORRECTION = (1<<3),
  DELIVERY_REASON_MEAL       = (1<<4)
} IDS_SRCP_Bolus_Flags_t;

typedef enum
{
  UNDETERMINED_TYPE = (0x0F),
  FAST              = (0x33),
  EXTENDED          = (0x3C),
  MULTI_WAVE        = (0x55)
} IDS_SRCP_Bolus_Type_t;

typedef enum
{
  UNDETERMINED_ACTIVATION_TYPE  = (0x0F),
  MANUAL                        = (0x33),
  RECOMMENDED                   = (0x3C),
  MANUALLY_CHANGED_RECOMMENDED  = (0x55),
  COMMANDED                     = (0x5A)
} IDS_SRCP_Bolus_Activation_Type_t;

typedef enum
{
  PROGRAMMED = (0x0F),
  REMAINING  = (0x33),
  DELIVERED  = (0x3C)
} IDS_SRCP_Bolus_Value_Selection_t;

typedef enum
{
  TBR_PRESENT                    = (1<<0),
  TBR_TEMPLATE_NUMBER_PRESENT    = (1<<1),
  BASAL_DELIVERY_CONTEXT_PRESENT = (1<<2)
} IDS_SRCP_Basal_Flags_t;

typedef enum
{
  ABSOLUTE = (0x33),
  RELATIVE = (0x3c)
} IDS_SRCP_TBR_Type_Value_t;

typedef enum
{
  DEVICE_BASED                    = (0x33),
  REMOTE_CONTROL                  = (0x3C),
  ARTIFICIAL_PANCREAS             = (0x55)
} IDS_SRCP_Basal_Delivery_Context_t;

typedef struct
{
  IDS_SRCP_Procedure_t Procedure;
  IDS_SRCP_Response_t Response;
  IDS_SRCP_State_t State;
  uint16_t Flag;
  uint8_t Last_Received_E2E_Counter;
  uint8_t Current_Received_E2E_Counter;
  uint8_t Transmit_E2E_Counter;
  uint16_t E2E_CRC;
  UTIL_TIMER_Object_t TimerSRCPResponse_Id;
} IDS_SRCP_Context_t;

typedef struct
{
  uint8_t  Flags;
  uint16_t Id;
  uint8_t  Type;
  uint16_t FastAmount;
  uint16_t ExtAmount;
  uint16_t Duration;
  uint16_t DelayTime;
  uint8_t TemplateNumber;
  uint8_t ActivationType;
  uint8_t  State;
} IDS_SRCP_Bolus_t;

typedef struct
{
  uint8_t  Flags;
  uint8_t  TemplateNumber;
  uint16_t CurrentConfigValue;
  uint8_t  TBRType;
  uint16_t TBRAdjustmentValue;
  uint16_t TBRDurationProgrammed;
  uint16_t TBRDurationRemaining;
  uint8_t  TBRTemplateNumber;
  uint8_t  BasalDeliveryContext;
} IDS_SRCP_Active_Basal_Rate_t;

typedef struct
{
  uint16_t  SumOfBolusDelivered;
  uint16_t  SumOfBasalDelivered;
} IDS_SRCP_Total_Daily_Insulin_t;

typedef enum
{
  LIFETIME       = (0x0F),
  WARRANTY_TIME  = (0x33),
  LOANER_TIME    = (0x3C),
  OPERATION_TIME = (0x55)
} IDS_SRCP_Counter_Type_t;

typedef enum
{
  CS_REMAINING = (0x0F),
  CS_ELAPSED   = (0x33)
} IDS_SRCP_Counter_Selection_t;

typedef struct
{
  int32_t Remaining;
  int32_t Elapsed;
} IDS_SRCP_Insulin_Counter_Selection_t;

typedef struct
{
  IDS_SRCP_Insulin_Counter_Selection_t  LifeTime;
  IDS_SRCP_Insulin_Counter_Selection_t  WarrantyTime;
  IDS_SRCP_Insulin_Counter_Selection_t  LoanerTime;
  IDS_SRCP_Insulin_Counter_Selection_t  OperationTime;
} IDS_SRCP_Insulin_Counter_t;

typedef enum
{
  REMAINING_DURATION_PRESENT = (1<<0)
} IDS_SRCP_On_Board_Flags_t;

typedef struct
{
  IDS_SRCP_On_Board_Flags_t OnBoardFlags;
  uint16_t InsulinOnBoard;
  uint16_t RemainingDuration;
} IDS_SRCP_Insulin_On_Board_t;

/* Private variables ---------------------------------------------------------*/
/**
 * START of Section BLE_DRIVER_CONTEXT
 */
PLACE_IN_SECTION("BLE_DRIVER_CONTEXT") static IDS_SRCP_Context_t IDS_SRCP_Context;

IDS_SRCP_Bolus_t IDS_SRCP_ActiveBolusesIDs[4] =
{
  {/* Flags */           DELAY_TIME_PRESENT | TEMPLATE_NUMBER_PRESENT | ACTIVATION_TYPE_PRESENT | DELIVERY_REASON_CORRECTION | DELIVERY_REASON_MEAL, 
   /* Id */              0x01,
   /* Type */            FAST,
   /* Fast Amount */     10,
   /* Ext Amount */      0,
   /* Duration (min) */  0,
   /* Delay (min) */     10,
   /* Template Number */  1,
   /* Activation Type */ MANUAL,
   /* State */           1},
  {/* Flags */           DELAY_TIME_PRESENT | TEMPLATE_NUMBER_PRESENT | ACTIVATION_TYPE_PRESENT | DELIVERY_REASON_CORRECTION | DELIVERY_REASON_MEAL, 
   /* Id */              0x02,
   /* Type */            EXTENDED,
   /* Fast Amount */     0,
   /* Ext Amount */      10,
   /* Duration (min) */  10,
   /* Delay (min) */     10,
   /* Template Number */  2,
   /* Activation Type */ RECOMMENDED,
   /* State */           1},
  {/* Flags */           DELAY_TIME_PRESENT | TEMPLATE_NUMBER_PRESENT | ACTIVATION_TYPE_PRESENT | DELIVERY_REASON_CORRECTION | DELIVERY_REASON_MEAL, 
   /* Id */              0x03,
   /* Type */            MULTI_WAVE,
   /* Fast Amount */     10,
   /* Ext Amount */      10,
   /* Duration (min) */  10,
   /* Delay (min) */     10,
   /* Template Number */  3,
   /* Activation Type */ MANUALLY_CHANGED_RECOMMENDED,
   /* State */           1},
  {/* Flags */           DELAY_TIME_PRESENT | TEMPLATE_NUMBER_PRESENT | ACTIVATION_TYPE_PRESENT | DELIVERY_REASON_CORRECTION | DELIVERY_REASON_MEAL, 
   /* Id */              0x04,
   /* Type */            FAST,
   /* Fast Amount */     10,
   /* Ext Amount */      0,
   /* Duration (min) */  0,
   /* Delay (min) */     10,
   /* Template Number */  4,
   /* Activation Type */ COMMANDED,
   /* State */           0}
};

IDS_SRCP_Active_Basal_Rate_t IDS_SRCP_ActiveBasalRate[4] =
{
  {/* Flags */                    TBR_PRESENT | TBR_TEMPLATE_NUMBER_PRESENT | BASAL_DELIVERY_CONTEXT_PRESENT, 
   /* Template Number */          1,
   /* Current Config Value */     10,
   /* TBR Type Value */           ABSOLUTE,
   /* TBR Adjustment Value */     10,
   /* TBR Duration Programmed */  10,
   /* TBR Duration Remaining */   10,
   /* TBR Template Number */      1,
   /* Basal Delivery Context */   DEVICE_BASED},
  {/* Flags */                    TBR_PRESENT | TBR_TEMPLATE_NUMBER_PRESENT | BASAL_DELIVERY_CONTEXT_PRESENT, 
   /* Template Number */          2,
   /* Current Config Value */     10,
   /* TBR Type Value */           RELATIVE,
   /* TBR Adjustment Value */     10,
   /* TBR Duration Programmed */  10,
   /* TBR Duration Remaining */   10,
   /* TBR Template Number */      2,
   /* Basal Delivery Context */   REMOTE_CONTROL},
  {/* Flags */                    TBR_PRESENT | TBR_TEMPLATE_NUMBER_PRESENT | BASAL_DELIVERY_CONTEXT_PRESENT, 
   /* Template Number */          3,
   /* Current Config Value */     10,
   /* TBR Type Value */           ABSOLUTE,
   /* TBR Adjustment Value */     10,
   /* TBR Duration Programmed */  10,
   /* TBR Duration Remaining */   10,
   /* TBR Template Number */      3,
   /* Basal Delivery Context */   ARTIFICIAL_PANCREAS},
  {/* Flags */                    TBR_PRESENT | TBR_TEMPLATE_NUMBER_PRESENT | BASAL_DELIVERY_CONTEXT_PRESENT, 
   /* Template Number */          4,
   /* Current Config Value */     10,
   /* TBR Type Value */           RELATIVE,
   /* TBR Adjustment Value */     10,
   /* TBR Duration Programmed */  10,
   /* TBR Duration Remaining */   10,
   /* TBR Template Number */      4,
   /* Basal Delivery Context */   ABSOLUTE}
};

IDS_SRCP_Insulin_On_Board_t InsulinOnBoardValue = {REMAINING_DURATION_PRESENT, 100, 50};

/* Global variables ----------------------------------------------------------*/
IDS_SRCP_Total_Daily_Insulin_t TotalDailyInsulin;
IDS_SRCP_Insulin_Counter_t IDDCounter;
uint32_t BolusAmountDelivered;
uint32_t BasalAmountDelivered;
uint8_t ActiveBolusNumber;
uint8_t ActiveBasalRateDeliveryNumber;
/* Private function prototypes -----------------------------------------------*/
static tBleStatus IDS_SRCP_ResponseCode(uint8_t responseCode);
static tBleStatus IDS_SRCP_GetActiveBolusIdsResponse(void);
static tBleStatus IDS_SRCP_GetBolusDeliveryResponse(uint16_t bolusId, uint8_t bolusValueSelection);
static tBleStatus IDS_SRCP_GetActiveBasalRateDeliveryResponse(void);
static tBleStatus IDS_SRCP_GetTotalDailyInsulinResponse(void);
static tBleStatus IDS_SRCP_GetCounterResponse(uint8_t type, uint8_t valueSelection);
static tBleStatus IDS_SRCP_GetDeliveredInsulinResponse(void);
static tBleStatus IDS_SRCP_GetInsulinOnBoardResponse(void);
static void IDS_SRCP_ResponseProc(void *arg);
static void IDS_SRCP_ResponeTask(void);

/* Private functions ---------------------------------------------------------*/
extern uint8_t a_IDS_UpdateCharData[247];

/**
  * @brief SRCP response code to the previous request
  * @param [in] responseCode : response code
  * @retval BLE_STATUS_SUCCESS if ok, error code otherwise
  */
static tBleStatus IDS_SRCP_ResponseCode(uint8_t responseCode)
{
  tBleStatus retval = BLE_STATUS_FAILED;
  IDS_Data_t msg_conf;
  uint8_t length = 0;

  if(IDS_SRCP_Context.Transmit_E2E_Counter == 255)
  {
    IDS_SRCP_Context.Transmit_E2E_Counter = 1;
  }
  else
  {
    (IDS_SRCP_Context.Transmit_E2E_Counter)++;
  }

  LOG_INFO_APP("IDS_SRCP_ResponseCode >>>\r\n");

  a_IDS_UpdateCharData[length++] = (IDS_SRCP_RESPONSE_CODE & 0xFF);
  a_IDS_UpdateCharData[length++] = ((IDS_SRCP_RESPONSE_CODE >> 8) & 0xFF);
  a_IDS_UpdateCharData[length++] = ((IDS_SRCP_Context.Procedure.OpCode) & 0xFF);;
  a_IDS_UpdateCharData[length++] = (((IDS_SRCP_Context.Procedure.OpCode) >> 8) & 0xFF);
  a_IDS_UpdateCharData[length++] = responseCode;
  a_IDS_UpdateCharData[length++] = IDS_SRCP_Context.Transmit_E2E_Counter;
  if(((IDS_APP_GetFeature()) & E2E_PROTECTION_SUPPORTED) == (E2E_PROTECTION_SUPPORTED))
  {
    IDS_SRCP_Context.E2E_CRC = (uint16_t)IDS_APP_ComputeCRC(a_IDS_UpdateCharData, length);
    LOG_INFO_APP("E2E_CRC = 0x%x\r\n", IDS_SRCP_Context.E2E_CRC);
  }
  else
  {
    IDS_SRCP_Context.E2E_CRC = 0xFFFF;
    LOG_INFO_APP("E2E_CRC = 0x%x\r\n", IDS_SRCP_Context.E2E_CRC);
  }
  a_IDS_UpdateCharData[length++] = (uint8_t)((IDS_SRCP_Context.E2E_CRC) & 0xFF);
  a_IDS_UpdateCharData[length++] = (uint8_t)(((IDS_SRCP_Context.E2E_CRC) >> 8) & 0xFF);

  msg_conf.Length = length;
  msg_conf.p_Payload = a_IDS_UpdateCharData;
  retval = IDS_UpdateValue(IDS_IDSRCP, &msg_conf);


  LOG_INFO_APP("IDS_SRCP_ResponseCode <<<\r\n");

  return retval;
} /* end of IDS_SRCP_ResponseCode() */

/**
  * @brief SRCP get active bolus ids response to the previous request
  * @param [in] None
  * @retval BLE_STATUS_SUCCESS if ok, error code otherwise
  */
static tBleStatus IDS_SRCP_GetActiveBolusIdsResponse(void)
{
  tBleStatus retval = BLE_STATUS_FAILED;
  IDS_Data_t msg_conf;
  uint8_t length = 0;

  if(IDS_SRCP_Context.Transmit_E2E_Counter == 255)
  {
    IDS_SRCP_Context.Transmit_E2E_Counter = 1;
  }
  else
  {
    (IDS_SRCP_Context.Transmit_E2E_Counter)++;
  }

  a_IDS_UpdateCharData[length++] = (IDS_SRCP_GET_ACTIVE_BOLUS_IDS_RESPONSE & 0xFF);
  a_IDS_UpdateCharData[length++] = ((IDS_SRCP_GET_ACTIVE_BOLUS_IDS_RESPONSE >> 8) & 0xFF);
  a_IDS_UpdateCharData[length++] = ActiveBolusNumber;
  for(uint8_t i = 0; i < ActiveBolusNumber; i++)
  {
    if(IDS_SRCP_ActiveBolusesIDs[i].State > 0)
    {
      a_IDS_UpdateCharData[length++] = (IDS_SRCP_ActiveBolusesIDs[i].Id & 0xFF);
      a_IDS_UpdateCharData[length++] = ((IDS_SRCP_ActiveBolusesIDs[i].Id >> 8) & 0xFF);
    }
  }

  if(length > 3)
  {  
    a_IDS_UpdateCharData[length++] = IDS_SRCP_Context.Transmit_E2E_Counter;
    if(((IDS_APP_GetFeature()) & E2E_PROTECTION_SUPPORTED) == (E2E_PROTECTION_SUPPORTED))
    {
      IDS_SRCP_Context.E2E_CRC = (uint16_t)IDS_APP_ComputeCRC(a_IDS_UpdateCharData, length);
    }
    else
    {
      IDS_SRCP_Context.E2E_CRC = 0xFFFF;
    }
    a_IDS_UpdateCharData[length++] = (uint8_t)((IDS_SRCP_Context.E2E_CRC) & 0xFF);
    a_IDS_UpdateCharData[length++] = (uint8_t)(((IDS_SRCP_Context.E2E_CRC) >> 8) & 0xFF);

    msg_conf.Length = length;
    msg_conf.p_Payload = a_IDS_UpdateCharData;
    retval = IDS_UpdateValue(IDS_IDSRCP, &msg_conf);
  }
  else
  {
    LOG_INFO_APP("PROCEDURE NOT APPLICABLE: no Bolus found\r\n");
    IDS_SRCP_ResponseCode(IDS_SRCP_ERROR_CODE_PROCEDURE_NOT_APPLICABLE);
    return BLE_STATUS_SUCCESS;
  }

  return retval;
} /* end of IDS_SRCP_GetActiveBolusIdsResponse() */

/**
  * @brief SRCP get bolus delivery response to the previous request
  * @param [in] Bolus ID, Bolus Value Selection
  * @retval BLE_STATUS_SUCCESS if ok, error code otherwise
  */
static tBleStatus IDS_SRCP_GetBolusDeliveryResponse(uint16_t bolusId, uint8_t bolusValueSelection)
{
  tBleStatus retval = BLE_STATUS_FAILED;
  IDS_Data_t msg_conf;
  uint8_t length = 0;

  if(IDS_SRCP_Context.Transmit_E2E_Counter == 255)
  {
    IDS_SRCP_Context.Transmit_E2E_Counter = 1;
  }
  else
  {
    (IDS_SRCP_Context.Transmit_E2E_Counter)++;
  }

  if(((IDS_SRCP_Bolus_Value_Selection_t)bolusValueSelection != PROGRAMMED) &&
     ((IDS_SRCP_Bolus_Value_Selection_t)bolusValueSelection != REMAINING)  &&
     ((IDS_SRCP_Bolus_Value_Selection_t)bolusValueSelection != DELIVERED))
  {
    IDS_SRCP_ResponseCode(IDS_SRCP_ERROR_CODE_INVALID_OPERAND);
    return BLE_STATUS_SUCCESS;
  }
  
  a_IDS_UpdateCharData[length++] = (IDS_SRCP_GET_ACTIVE_BOLUS_DELIVERY_RESPONSE & 0xFF);
  a_IDS_UpdateCharData[length++] = ((IDS_SRCP_GET_ACTIVE_BOLUS_DELIVERY_RESPONSE >> 8) & 0xFF);
  for(uint8_t i = 0; i < NB_ACTIVE_BOLUSES_IDS; i++)
  {
    if((IDS_SRCP_ActiveBolusesIDs[i].State > 0) &&
       (IDS_SRCP_ActiveBolusesIDs[i].Id == bolusId))
    {
      a_IDS_UpdateCharData[length++] = IDS_SRCP_ActiveBolusesIDs[i].Flags;
      a_IDS_UpdateCharData[length++] = (IDS_SRCP_ActiveBolusesIDs[i].Id & 0xFF);
      a_IDS_UpdateCharData[length++] = ((IDS_SRCP_ActiveBolusesIDs[i].Id >> 8) & 0xFF);
      a_IDS_UpdateCharData[length++] = IDS_SRCP_ActiveBolusesIDs[i].Type;
      a_IDS_UpdateCharData[length++] = (IDS_SRCP_ActiveBolusesIDs[i].FastAmount & 0xFF);
      a_IDS_UpdateCharData[length++] = ((IDS_SRCP_ActiveBolusesIDs[i].FastAmount >> 8) & 0xFF);
      a_IDS_UpdateCharData[length++] = (IDS_SRCP_ActiveBolusesIDs[i].ExtAmount & 0xFF);
      a_IDS_UpdateCharData[length++] = ((IDS_SRCP_ActiveBolusesIDs[i].ExtAmount >> 8) & 0xFF);
      a_IDS_UpdateCharData[length++] = (IDS_SRCP_ActiveBolusesIDs[i].Duration & 0xFF);
      a_IDS_UpdateCharData[length++] = ((IDS_SRCP_ActiveBolusesIDs[i].Duration >> 8) & 0xFF);
      if((IDS_SRCP_ActiveBolusesIDs[i].Flags & DELAY_TIME_PRESENT) == DELAY_TIME_PRESENT)
      {
        a_IDS_UpdateCharData[length++] = (IDS_SRCP_ActiveBolusesIDs[i].DelayTime & 0xFF);
        a_IDS_UpdateCharData[length++] = ((IDS_SRCP_ActiveBolusesIDs[i].DelayTime >> 8) & 0xFF);
      }
      if((IDS_SRCP_ActiveBolusesIDs[i].Flags & TEMPLATE_NUMBER_PRESENT) == TEMPLATE_NUMBER_PRESENT)
      {
        a_IDS_UpdateCharData[length++] = IDS_SRCP_ActiveBolusesIDs[i].TemplateNumber;
      }
      if((IDS_SRCP_ActiveBolusesIDs[i].Flags & ACTIVATION_TYPE_PRESENT) == ACTIVATION_TYPE_PRESENT)
      {
        a_IDS_UpdateCharData[length++] = IDS_SRCP_ActiveBolusesIDs[i].ActivationType;
      }
    }
  }

  if(length > 2)
  {
    a_IDS_UpdateCharData[length++] = IDS_SRCP_Context.Transmit_E2E_Counter;
    if(((IDS_APP_GetFeature()) & E2E_PROTECTION_SUPPORTED) == (E2E_PROTECTION_SUPPORTED))
    {
      IDS_SRCP_Context.E2E_CRC = (uint16_t)IDS_APP_ComputeCRC(a_IDS_UpdateCharData, length);
    }
    else
    {
      IDS_SRCP_Context.E2E_CRC = 0xFFFF;
    }
    a_IDS_UpdateCharData[length++] = (uint8_t)((IDS_SRCP_Context.E2E_CRC) & 0xFF);
    a_IDS_UpdateCharData[length++] = (uint8_t)(((IDS_SRCP_Context.E2E_CRC) >> 8) & 0xFF);
        
    msg_conf.Length = length;
    msg_conf.p_Payload = a_IDS_UpdateCharData;
    retval = IDS_UpdateValue(IDS_IDSRCP, &msg_conf);
  }
  else
  {
    LOG_INFO_APP("PROCEDURE NOT APPLICABLE: no Bolus found\r\n");
    IDS_SRCP_ResponseCode(IDS_SRCP_ERROR_CODE_PROCEDURE_NOT_APPLICABLE);
    return BLE_STATUS_SUCCESS;
  }
  
  return retval;
} /* end of IDS_SRCP_get_bolus_delivery_response() */

/**
  * @brief SRCP get active basal rate delivery response to the previous request
  * @param [in] None
  * @retval BLE_STATUS_SUCCESS if ok, error code otherwise
  */
static tBleStatus IDS_SRCP_GetActiveBasalRateDeliveryResponse(void)
{
  tBleStatus retval = BLE_STATUS_FAILED;
  IDS_Data_t msg_conf;
  uint8_t length = 0;

  if(IDS_SRCP_Context.Transmit_E2E_Counter == 255)
  {
    IDS_SRCP_Context.Transmit_E2E_Counter = 1;
  }
  else
  {
    (IDS_SRCP_Context.Transmit_E2E_Counter)++;
  }

  if(ActiveBasalRateDeliveryNumber > 0)
  {
    a_IDS_UpdateCharData[length++] = (IDS_SRCP_GET_ACTIVE_BASAL_RATE_DELIVERY_RESPONSE & 0xFF);
    a_IDS_UpdateCharData[length++] = ((IDS_SRCP_GET_ACTIVE_BASAL_RATE_DELIVERY_RESPONSE >> 8) & 0xFF);
    a_IDS_UpdateCharData[length++] =   IDS_SRCP_ActiveBasalRate[0].Flags;
    a_IDS_UpdateCharData[length++] =   IDS_SRCP_ActiveBasalRate[0].TemplateNumber;
    a_IDS_UpdateCharData[length++] =  (IDS_SRCP_ActiveBasalRate[0].CurrentConfigValue & 0xFF);
    a_IDS_UpdateCharData[length++] = ((IDS_SRCP_ActiveBasalRate[0].CurrentConfigValue >> 8) & 0xFF);
    if((IDS_SRCP_ActiveBasalRate[0].Flags & TBR_PRESENT) == TBR_PRESENT)
    {
      a_IDS_UpdateCharData[length++] =   IDS_SRCP_ActiveBasalRate[0].TBRType;
      a_IDS_UpdateCharData[length++] = ((IDS_SRCP_ActiveBasalRate[0].TBRAdjustmentValue) & 0xFF);
      a_IDS_UpdateCharData[length++] = (((IDS_SRCP_ActiveBasalRate[0].TBRAdjustmentValue) >> 8) & 0xFF);
      a_IDS_UpdateCharData[length++] = ((IDS_SRCP_ActiveBasalRate[0].TBRDurationProgrammed) & 0xFF);
      a_IDS_UpdateCharData[length++] = (((IDS_SRCP_ActiveBasalRate[0].TBRDurationProgrammed) >> 8) & 0xFF);
      a_IDS_UpdateCharData[length++] = ((IDS_SRCP_ActiveBasalRate[0].TBRDurationRemaining) & 0xFF);
      a_IDS_UpdateCharData[length++] = (((IDS_SRCP_ActiveBasalRate[0].TBRDurationRemaining) >> 8) & 0xFF);
    }
    if((IDS_SRCP_ActiveBasalRate[0].Flags & TBR_TEMPLATE_NUMBER_PRESENT) == TBR_TEMPLATE_NUMBER_PRESENT)
    {
      a_IDS_UpdateCharData[length++] = IDS_SRCP_ActiveBasalRate[0].TBRTemplateNumber;
    }
    if((IDS_SRCP_ActiveBasalRate[0].Flags & BASAL_DELIVERY_CONTEXT_PRESENT) == BASAL_DELIVERY_CONTEXT_PRESENT)
    {
      a_IDS_UpdateCharData[length++] = IDS_SRCP_ActiveBasalRate[0].BasalDeliveryContext;
    }

    a_IDS_UpdateCharData[length++] = IDS_SRCP_Context.Transmit_E2E_Counter;
    if(((IDS_APP_GetFeature()) & E2E_PROTECTION_SUPPORTED) == (E2E_PROTECTION_SUPPORTED))
    {
      IDS_SRCP_Context.E2E_CRC = (uint16_t)IDS_APP_ComputeCRC(a_IDS_UpdateCharData, length);
    }
    else
    {
      IDS_SRCP_Context.E2E_CRC = 0xFFFF;
    }
    a_IDS_UpdateCharData[length++] = (uint8_t)((IDS_SRCP_Context.E2E_CRC) & 0xFF);
    a_IDS_UpdateCharData[length++] = (uint8_t)(((IDS_SRCP_Context.E2E_CRC) >> 8) & 0xFF);
        
    msg_conf.Length = length;
    msg_conf.p_Payload = a_IDS_UpdateCharData;
    retval = IDS_UpdateValue(IDS_IDSRCP, &msg_conf);
  }
  else
  {
    LOG_INFO_APP("PROCEDURE NOT APPLICABLE: no Bolus found\r\n");
    IDS_SRCP_ResponseCode(IDS_SRCP_ERROR_CODE_PROCEDURE_NOT_APPLICABLE);
    return BLE_STATUS_SUCCESS;
  }
    
  return retval;
} /* end of IDS_SRCP_GetActiveBasalRateDeliveryResponse() */

/**
  * @brief SRCP Get Total Daily Insulin response to the previous request
  * @param [in] None
  * @retval BLE_STATUS_SUCCESS if ok, error code otherwise
  */
static tBleStatus IDS_SRCP_GetTotalDailyInsulinResponse(void)
{
  tBleStatus retval = BLE_STATUS_FAILED;
  IDS_Data_t msg_conf;
  uint8_t length = 0;

  if(IDS_SRCP_Context.Transmit_E2E_Counter == 255)
  {
    IDS_SRCP_Context.Transmit_E2E_Counter = 1;
  }
  else
  {
    (IDS_SRCP_Context.Transmit_E2E_Counter)++;
  }

  a_IDS_UpdateCharData[length++] = (IDS_SRCP_GET_TOTAL_DAILY_INSULIN_STATUS_RESPONSE & 0xFF);
  a_IDS_UpdateCharData[length++] = ((IDS_SRCP_GET_TOTAL_DAILY_INSULIN_STATUS_RESPONSE >> 8) & 0xFF);
  if(((IDS_APP_GetFeature()) & (FAST_BOLUS_SUPPORTED | EXTENDED_BOLUS_SUPPORTED | MULTIWAVE_BOLUS_SUPPORTED)) == 
     (FAST_BOLUS_SUPPORTED | EXTENDED_BOLUS_SUPPORTED | MULTIWAVE_BOLUS_SUPPORTED))
  {
    a_IDS_UpdateCharData[length++] = ((TotalDailyInsulin.SumOfBolusDelivered) & 0xFF);
    a_IDS_UpdateCharData[length++] = (((TotalDailyInsulin.SumOfBolusDelivered) >> 8) & 0xFF);
  }
  if(((IDS_APP_GetFeature()) & BASAL_RATE_SUPPORTED) == BASAL_RATE_SUPPORTED)
  {
    a_IDS_UpdateCharData[length++] = ((TotalDailyInsulin.SumOfBasalDelivered) & 0xFF);
    a_IDS_UpdateCharData[length++] = (((TotalDailyInsulin.SumOfBasalDelivered) >> 8) & 0xFF);
  }
  a_IDS_UpdateCharData[length++] = ((TotalDailyInsulin.SumOfBolusDelivered + TotalDailyInsulin.SumOfBasalDelivered) & 0xFF);
  a_IDS_UpdateCharData[length++] = (((TotalDailyInsulin.SumOfBolusDelivered + TotalDailyInsulin.SumOfBasalDelivered) >> 8) & 0xFF);

  a_IDS_UpdateCharData[length++] = IDS_SRCP_Context.Transmit_E2E_Counter;
  if(((IDS_APP_GetFeature()) & E2E_PROTECTION_SUPPORTED) == (E2E_PROTECTION_SUPPORTED))
  {
    IDS_SRCP_Context.E2E_CRC = (uint16_t)IDS_APP_ComputeCRC(a_IDS_UpdateCharData, length);
  }
  else
  {
    IDS_SRCP_Context.E2E_CRC = 0xFFFF;
  }
  a_IDS_UpdateCharData[length++] = (uint8_t)((IDS_SRCP_Context.E2E_CRC) & 0xFF);
  a_IDS_UpdateCharData[length++] = (uint8_t)(((IDS_SRCP_Context.E2E_CRC) >> 8) & 0xFF);
      
  msg_conf.Length = length;
  msg_conf.p_Payload = a_IDS_UpdateCharData;
  retval = IDS_UpdateValue(IDS_IDSRCP, &msg_conf);

  return retval;
} /* end of IDS_SRCP_GetTotalDailyInsulinResponse() */

/**
  * @brief SRCP get counter response to the previous request
  * @param [in] type, Value Selection
  * @retval BLE_STATUS_SUCCESS if ok, error code otherwise
  */
static tBleStatus IDS_SRCP_GetCounterResponse(uint8_t type, uint8_t valueSelection)
{
  tBleStatus retval = BLE_STATUS_FAILED;
  IDS_Data_t msg_conf;
  uint8_t length = 0;

  if(IDS_SRCP_Context.Transmit_E2E_Counter == 255)
  {
    IDS_SRCP_Context.Transmit_E2E_Counter = 1;
  }
  else
  {
    (IDS_SRCP_Context.Transmit_E2E_Counter)++;
  }

  if(((IDS_SRCP_Counter_Selection_t)valueSelection != CS_REMAINING) ||
     ((IDS_SRCP_Counter_Selection_t)valueSelection != CS_ELAPSED))
  {
    IDS_SRCP_ResponseCode(IDS_SRCP_ERROR_CODE_INVALID_OPERAND);
    return BLE_STATUS_SUCCESS;
  }
  
  a_IDS_UpdateCharData[length++] = (IDS_SRCP_GET_COUNTER_RESPONSE & 0xFF);
  a_IDS_UpdateCharData[length++] = ((IDS_SRCP_GET_COUNTER_RESPONSE >> 8) & 0xFF);
  a_IDS_UpdateCharData[length++] = type;
  a_IDS_UpdateCharData[length++] = valueSelection;
  if(type == LIFETIME)
  {
    if(valueSelection == CS_REMAINING)
    {
      a_IDS_UpdateCharData[length++] =  ((IDDCounter.LifeTime.Remaining) & 0xFF);
      a_IDS_UpdateCharData[length++] = (((IDDCounter.LifeTime.Remaining) >> 8) & 0xFF);
      a_IDS_UpdateCharData[length++] = (((IDDCounter.LifeTime.Remaining) >> 16) & 0xFF);
      a_IDS_UpdateCharData[length++] = (((IDDCounter.LifeTime.Remaining) >> 24) & 0xFF);
    }
    else
    {
      a_IDS_UpdateCharData[length++] =  ((IDDCounter.LifeTime.Elapsed) & 0xFF);
      a_IDS_UpdateCharData[length++] = (((IDDCounter.LifeTime.Elapsed) >> 8) & 0xFF);
      a_IDS_UpdateCharData[length++] = (((IDDCounter.LifeTime.Elapsed) >> 16) & 0xFF);
      a_IDS_UpdateCharData[length++] = (((IDDCounter.LifeTime.Elapsed) >> 24) & 0xFF);
    }
  }
  else if(type == WARRANTY_TIME)
  {
    if(valueSelection == CS_REMAINING)
    {
      a_IDS_UpdateCharData[length++] =  ((IDDCounter.WarrantyTime.Remaining) & 0xFF);
      a_IDS_UpdateCharData[length++] = (((IDDCounter.WarrantyTime.Remaining) >> 8) & 0xFF);
      a_IDS_UpdateCharData[length++] = (((IDDCounter.WarrantyTime.Remaining) >> 16) & 0xFF);
      a_IDS_UpdateCharData[length++] = (((IDDCounter.WarrantyTime.Remaining) >> 24) & 0xFF);
    }
    else
    {
      a_IDS_UpdateCharData[length++] =  ((IDDCounter.WarrantyTime.Elapsed) & 0xFF);
      a_IDS_UpdateCharData[length++] = (((IDDCounter.WarrantyTime.Elapsed) >> 8) & 0xFF);
      a_IDS_UpdateCharData[length++] = (((IDDCounter.WarrantyTime.Elapsed) >> 16) & 0xFF);
      a_IDS_UpdateCharData[length++] = (((IDDCounter.WarrantyTime.Elapsed) >> 24) & 0xFF);
    }
  }
  else if(type == LOANER_TIME)
  {
    if(valueSelection == CS_REMAINING)
    {
      a_IDS_UpdateCharData[length++] =  ((IDDCounter.LoanerTime.Remaining) & 0xFF);
      a_IDS_UpdateCharData[length++] = (((IDDCounter.LoanerTime.Remaining) >> 8) & 0xFF);
      a_IDS_UpdateCharData[length++] = (((IDDCounter.LoanerTime.Remaining) >> 16) & 0xFF);
      a_IDS_UpdateCharData[length++] = (((IDDCounter.LoanerTime.Remaining) >> 24) & 0xFF);
    }
    else
    {
      a_IDS_UpdateCharData[length++] =  ((IDDCounter.LoanerTime.Elapsed) & 0xFF);
      a_IDS_UpdateCharData[length++] = (((IDDCounter.LoanerTime.Elapsed) >> 8) & 0xFF);
      a_IDS_UpdateCharData[length++] = (((IDDCounter.LoanerTime.Elapsed) >> 16) & 0xFF);
      a_IDS_UpdateCharData[length++] = (((IDDCounter.LoanerTime.Elapsed) >> 24) & 0xFF);
    }
  }
  else
  {
    if(valueSelection == CS_REMAINING)
    {
      a_IDS_UpdateCharData[length++] =  ((IDDCounter.OperationTime.Remaining) & 0xFF);
      a_IDS_UpdateCharData[length++] = (((IDDCounter.OperationTime.Remaining) >> 8) & 0xFF);
      a_IDS_UpdateCharData[length++] = (((IDDCounter.OperationTime.Remaining) >> 16) & 0xFF);
      a_IDS_UpdateCharData[length++] = (((IDDCounter.OperationTime.Remaining) >> 24) & 0xFF);
    }
    else
    {
      a_IDS_UpdateCharData[length++] =  ((IDDCounter.OperationTime.Elapsed) & 0xFF);
      a_IDS_UpdateCharData[length++] = (((IDDCounter.OperationTime.Elapsed) >> 8) & 0xFF);
      a_IDS_UpdateCharData[length++] = (((IDDCounter.OperationTime.Elapsed) >> 16) & 0xFF);
      a_IDS_UpdateCharData[length++] = (((IDDCounter.OperationTime.Elapsed) >> 24) & 0xFF);
    }
  }

  a_IDS_UpdateCharData[length++] = IDS_SRCP_Context.Transmit_E2E_Counter;
  if(((IDS_APP_GetFeature()) & E2E_PROTECTION_SUPPORTED) == (E2E_PROTECTION_SUPPORTED))
  {
    IDS_SRCP_Context.E2E_CRC = (uint16_t)IDS_APP_ComputeCRC(a_IDS_UpdateCharData, length);
  }
  else
  {
    IDS_SRCP_Context.E2E_CRC = 0xFFFF;
  }
  a_IDS_UpdateCharData[length++] = (uint8_t)((IDS_SRCP_Context.E2E_CRC) & 0xFF);
  a_IDS_UpdateCharData[length++] = (uint8_t)(((IDS_SRCP_Context.E2E_CRC) >> 8) & 0xFF);
      
  msg_conf.Length = length;
  msg_conf.p_Payload = a_IDS_UpdateCharData;
  retval = IDS_UpdateValue(IDS_IDSRCP, &msg_conf);
 
  return retval;
} /* end of IDS_SRCP_GetCounterResponse(uint8_t type, uint8_t valueSelection) */

/**
  * @brief SRCP Get Delivered Insulin response to the previous request
  * @param [in] None
  * @retval BLE_STATUS_SUCCESS if ok, error code otherwise
  */
static tBleStatus IDS_SRCP_GetDeliveredInsulinResponse(void)
{
  tBleStatus retval = BLE_STATUS_FAILED;
  IDS_Data_t msg_conf;
  uint8_t length = 0;

  if(IDS_SRCP_Context.Transmit_E2E_Counter == 255)
  {
    IDS_SRCP_Context.Transmit_E2E_Counter = 1;
  }
  else
  {
    (IDS_SRCP_Context.Transmit_E2E_Counter)++;
  }

  a_IDS_UpdateCharData[length++] = (IDS_SRCP_GET_DELIVERED_INSULIN_RESPONSE & 0xFF);
  a_IDS_UpdateCharData[length++] = ((IDS_SRCP_GET_DELIVERED_INSULIN_RESPONSE >> 8) & 0xFF);
  a_IDS_UpdateCharData[length++] =  ((BolusAmountDelivered) & 0xFF);
  a_IDS_UpdateCharData[length++] = (((BolusAmountDelivered) >> 8) & 0xFF);
  a_IDS_UpdateCharData[length++] = (((BolusAmountDelivered) >> 16) & 0xFF);
  a_IDS_UpdateCharData[length++] = (((BolusAmountDelivered) >> 24) & 0xFF);
  a_IDS_UpdateCharData[length++] =  ((BasalAmountDelivered) & 0xFF);
  a_IDS_UpdateCharData[length++] = (((BasalAmountDelivered) >> 8) & 0xFF);
  a_IDS_UpdateCharData[length++] = (((BasalAmountDelivered) >> 16) & 0xFF);
  a_IDS_UpdateCharData[length++] = (((BasalAmountDelivered) >> 24) & 0xFF);

  a_IDS_UpdateCharData[length++] = IDS_SRCP_Context.Transmit_E2E_Counter;
  if(((IDS_APP_GetFeature()) & E2E_PROTECTION_SUPPORTED) == (E2E_PROTECTION_SUPPORTED))
  {
    IDS_SRCP_Context.E2E_CRC = (uint16_t)IDS_APP_ComputeCRC(a_IDS_UpdateCharData, length);
  }
  else
  {
    IDS_SRCP_Context.E2E_CRC = 0xFFFF;
  }
  a_IDS_UpdateCharData[length++] = (uint8_t)((IDS_SRCP_Context.E2E_CRC) & 0xFF);
  a_IDS_UpdateCharData[length++] = (uint8_t)(((IDS_SRCP_Context.E2E_CRC) >> 8) & 0xFF);
      
  msg_conf.Length = length;
  msg_conf.p_Payload = a_IDS_UpdateCharData;
  retval = IDS_UpdateValue(IDS_IDSRCP, &msg_conf);
  
  return retval;
} /* end of IDS_SRCP_GetDeliveredInsulinResponse() */

/**
  * @brief SRCP Get Insulin On Board response to the previous request
  * @param [in] None
  * @retval BLE_STATUS_SUCCESS if ok, error code otherwise
  */
static tBleStatus IDS_SRCP_GetInsulinOnBoardResponse(void)
{
  tBleStatus retval = BLE_STATUS_FAILED;
  IDS_Data_t msg_conf;
  uint8_t length = 0;

  if(IDS_SRCP_Context.Transmit_E2E_Counter == 255)
  {
    IDS_SRCP_Context.Transmit_E2E_Counter = 1;
  }
  else
  {
    (IDS_SRCP_Context.Transmit_E2E_Counter)++;
  }

  a_IDS_UpdateCharData[length++] = (IDS_SRCP_GET_INSULIN_ON_BOARD_RESPONSE & 0xFF);
  a_IDS_UpdateCharData[length++] = ((IDS_SRCP_GET_INSULIN_ON_BOARD_RESPONSE >> 8) & 0xFF);
  a_IDS_UpdateCharData[length++] =    InsulinOnBoardValue.OnBoardFlags;
  a_IDS_UpdateCharData[length++] =  ((InsulinOnBoardValue.InsulinOnBoard) & 0xFF);
  a_IDS_UpdateCharData[length++] = (((InsulinOnBoardValue.InsulinOnBoard) >> 8) & 0xFF);
  if((InsulinOnBoardValue.OnBoardFlags & REMAINING_DURATION_PRESENT) == REMAINING_DURATION_PRESENT)
  {
  a_IDS_UpdateCharData[length++] =  ((InsulinOnBoardValue.RemainingDuration) & 0xFF);
  a_IDS_UpdateCharData[length++] = (((InsulinOnBoardValue.RemainingDuration) >> 8) & 0xFF);
  }

  a_IDS_UpdateCharData[length++] = IDS_SRCP_Context.Transmit_E2E_Counter;
  if(((IDS_APP_GetFeature()) & E2E_PROTECTION_SUPPORTED) == (E2E_PROTECTION_SUPPORTED))
  {
    IDS_SRCP_Context.E2E_CRC = (uint16_t)IDS_APP_ComputeCRC(a_IDS_UpdateCharData, length);
  }
  else
  {
    IDS_SRCP_Context.E2E_CRC = 0xFFFF;
  }
  a_IDS_UpdateCharData[length++] = (uint8_t) ((IDS_SRCP_Context.E2E_CRC) & 0xFF);
  a_IDS_UpdateCharData[length++] = (uint8_t)(((IDS_SRCP_Context.E2E_CRC) >> 8) & 0xFF);
      
  msg_conf.Length = length;
  msg_conf.p_Payload = a_IDS_UpdateCharData;
  retval = IDS_UpdateValue(IDS_IDSRCP, &msg_conf);

  return retval;
} /* end of IDS_SRCP_GetInsulinOnBoardResponse() */

static void IDS_SRCP_ResponseProc(void *arg)
{
  UTIL_SEQ_SetTask(1U << CFG_TASK_IDS_SRCP_ID, CFG_SEQ_PRIO_0);
}

static void IDS_SRCP_ResponeTask(void)
{
  IDS_SRCP_ResponseCode(IDS_SRCP_ERROR_CODE_SUCCESS);
}

/* Public functions ----------------------------------------------------------*/

/**
  * @brief SRCP context initialization
  * @param None
  * @retval None
  */
void IDS_SRCP_Init(void)
{
  IDDCounter.LifeTime.Remaining = 0;
  IDDCounter.LifeTime.Elapsed = 0;
  IDDCounter.LoanerTime.Remaining = 0;
  IDDCounter.LoanerTime.Elapsed = 0;
  IDDCounter.OperationTime.Remaining = 0;
  IDDCounter.OperationTime.Elapsed = 0;
  IDDCounter.WarrantyTime.Remaining = 0;
  IDDCounter.WarrantyTime.Elapsed = 0;
  BolusAmountDelivered = 0;
  BasalAmountDelivered = 0;
  ActiveBolusNumber = NB_ACTIVE_BOLUSES_IDS;
  ActiveBasalRateDeliveryNumber = 1;

  UTIL_TIMER_Create(&(IDS_SRCP_Context.TimerSRCPResponse_Id),
                    0,
                    UTIL_TIMER_ONESHOT,
                    &IDS_SRCP_ResponseProc, 
                    0);
  UTIL_TIMER_SetPeriod(&(IDS_SRCP_Context.TimerSRCPResponse_Id), 1000);
  
  UTIL_SEQ_RegTask( 1<< CFG_TASK_IDS_SRCP_ID, UTIL_SEQ_RFU, IDS_SRCP_ResponeTask );
} /* end IDS_SRCP_Init() */

/**
  * @brief SRCP request handler 
  * @param [in] pRequestData : pointer to received SRCP request data
  * @param [in] requestDataLength : received SRCP request data length
  * @retval None
  */
void IDS_SRCP_RequestHandler(uint8_t * pRequestData, uint8_t requestDataLength)
{
  LOG_INFO_APP("IDS_SRCP_RequestHandler >>>\r\n");
  LOG_INFO_APP("SRCP Request, request data length: %d\r\n", requestDataLength);
  
  /* Get the requested procedure OpCode */
  IDS_SRCP_Context.Procedure.OpCode = ( (pRequestData[IDS_SRCP_OP_CODE_POSITION]) |
                                       ((pRequestData[IDS_SRCP_OP_CODE_POSITION + 1]) << 8) );
  LOG_INFO_APP("SRCP Procedure OpCode: 0x%02X\r\n", IDS_SRCP_Context.Procedure.OpCode);
  
  /* Check and Process the OpCode */
  switch(IDS_SRCP_Context.Procedure.OpCode)
  {
    case IDS_SRCP_RESET_STATUS:
      {
        LOG_INFO_APP("IDS Status Read Control Point Reset Status\r\n"); 
                  
        IDS_SRCP_Context.Procedure.Operand[0] = pRequestData[IDS_SRCP_OPERAND_POSITION];

        if(((pRequestData[IDS_SRCP_OPERAND_POSITION]) | ((pRequestData[IDS_SRCP_OPERAND_POSITION + 1] << 8))) >= (1<<8))
        {
          IDS_SRCP_ResponseCode(IDS_SRCP_ERROR_CODE_INVALID_OPERAND);
        }
        else
        {
          IDS_SRCP_Context.Flag = IDS_APP_GetStatusChangedFlag();
          if((pRequestData[IDS_SRCP_OPERAND_POSITION] & THERAPY_CONTROL_STATE_CHANGED) == THERAPY_CONTROL_STATE_CHANGED)
            IDS_SRCP_Context.Flag &= ~(THERAPY_CONTROL_STATE_CHANGED);
          if((pRequestData[IDS_SRCP_OPERAND_POSITION] & OPERATIONAL_STATE_CHANGED) == OPERATIONAL_STATE_CHANGED)
            IDS_SRCP_Context.Flag &= ~(OPERATIONAL_STATE_CHANGED);
          if((pRequestData[IDS_SRCP_OPERAND_POSITION] & RESERVOIR_STATUS_CHANGED) == RESERVOIR_STATUS_CHANGED)
            IDS_SRCP_Context.Flag &= ~(RESERVOIR_STATUS_CHANGED);
          if((pRequestData[IDS_SRCP_OPERAND_POSITION] & ANNUNCIATION_STATUS_CHANGED) == ANNUNCIATION_STATUS_CHANGED)
            IDS_SRCP_Context.Flag &= ~(ANNUNCIATION_STATUS_CHANGED);
          if((pRequestData[IDS_SRCP_OPERAND_POSITION] & TOTAL_DAILY_INSULIN_STATUS_CHANGED) == TOTAL_DAILY_INSULIN_STATUS_CHANGED)
            IDS_SRCP_Context.Flag &= ~(TOTAL_DAILY_INSULIN_STATUS_CHANGED);
          if((pRequestData[IDS_SRCP_OPERAND_POSITION] & ACTIVE_BASAL_RATE_STATUS_CHANGED) == ACTIVE_BASAL_RATE_STATUS_CHANGED)
            IDS_SRCP_Context.Flag &= ~(ACTIVE_BASAL_RATE_STATUS_CHANGED);
          if((pRequestData[IDS_SRCP_OPERAND_POSITION] & ACTIVE_BOLUS_STATUS_CHANGED) == ACTIVE_BOLUS_STATUS_CHANGED)
            IDS_SRCP_Context.Flag &= ~(ACTIVE_BOLUS_STATUS_CHANGED);
          if((pRequestData[IDS_SRCP_OPERAND_POSITION] & HISTORY_EVENT_RECORDED) == HISTORY_EVENT_RECORDED)
            IDS_SRCP_Context.Flag &= ~(HISTORY_EVENT_RECORDED);
          IDS_APP_SetStatusChangedFlag(IDS_SRCP_Context.Flag);
          LOG_INFO_APP("SUCCESS\r\n");
          UTIL_TIMER_Start(&(IDS_SRCP_Context.TimerSRCPResponse_Id));
        }
      }
      break;

    case IDS_SRCP_GET_ACTIVE_BOLUS_IDS:
      {
        LOG_INFO_APP("GET_ACTIVE_BOLUS_IDS\r\n");
        IDS_SRCP_GetActiveBolusIdsResponse();
      }
      break;
      
    case IDS_SRCP_GET_ACTIVE_BOLUS_DELIVERY:
      {
        uint16_t bolus_id =  (pRequestData[IDS_SRCP_OPERAND_POSITION] & 0xFF) |
                            ((pRequestData[IDS_SRCP_OPERAND_POSITION + 1] << 8) & 0xFF);
        uint8_t bolus_value_selection = pRequestData[IDS_SRCP_OPERAND_POSITION + 2] & 0xFF;
        LOG_INFO_APP("GET_ACTIVE_BOLUS_DELIVERY\r\n");
        
        IDS_SRCP_GetBolusDeliveryResponse(bolus_id, bolus_value_selection);
      }
      break;
      
    case IDS_SRCP_GET_ACTIVE_BASAL_RATE_DELIVERY:
      {
        LOG_INFO_APP("GET_ACTIVE_BASAL_RATE_DELIVERY\r\n");
        
        IDS_SRCP_GetActiveBasalRateDeliveryResponse();
      }
      break;
      
    case IDS_SRCP_GET_TOTAL_DAILY_INSULIN_STATUS:
      {
        LOG_INFO_APP("GET_TOTAL_DAILY_INSULIN_STATUS\r\n");
        
        IDS_SRCP_GetTotalDailyInsulinResponse();
      }
      break;
      
    case IDS_SRCP_GET_COUNTER:
      {
        uint8_t type = pRequestData[IDS_SRCP_OPERAND_POSITION] & 0xFF;
        uint8_t valueSelection = pRequestData[IDS_SRCP_OPERAND_POSITION + 1] & 0xFF;
        
        LOG_INFO_APP("GET_TOTAL_DAILY_INSULIN_STATUS\r\n");
        
        IDS_SRCP_GetCounterResponse(type, valueSelection);
      }
      break;
      
    case IDS_SRCP_GET_DELIVERED_INSULIN:
      {
        LOG_INFO_APP("GET_DELIVERED_INSULIN\r\n");
        
        IDS_SRCP_GetDeliveredInsulinResponse();
      }
      break;
      
    case IDS_SRCP_GET_INSULIN_ON_BOARD:
      {
        LOG_INFO_APP("GET_INSULIN_ON_BOARD\r\n");
        
        IDS_SRCP_GetInsulinOnBoardResponse();
      }
      break;
      
    default:
      {
        IDS_SRCP_ResponseCode(IDS_SRCP_ERROR_CODE_OPCODE_NOT_SUPPORTED);
      }
      break;
  }
  LOG_INFO_APP("IDS_SRCP_RequestHandler <<<\r\n");
} /* end IDS_SRCP_RequestHandler() */


/**
* @brief SRCP new write request permit check
* @param [in] pRequestData: Pointer to the request data byte array
* @param [in] pRequestDataLength: Length of the request data byte array
* @retval 0x00 when no error, error code otherwise
*/
uint8_t IDS_SRCP_CheckRequestValid(uint8_t * pRequestData, uint8_t requestDataLength)
{
  uint16_t E2E_CRC;
  
  LOG_INFO_APP("IDS_SRCP_CheckRequestValid >>>\r\n");
  IDS_SRCP_Context.Procedure.OpCode = (pRequestData[IDS_SRCP_OP_CODE_POSITION] |
                                      ((pRequestData[IDS_SRCP_OP_CODE_POSITION + 1]) << 8));
  LOG_INFO_APP("SRCP Procedure OpCode: 0x%02X\r\n", IDS_SRCP_Context.Procedure.OpCode);
  
  if (IDS_APP_GetIDSRCPCharacteristicIndicationEnabled() == FALSE)
  {
    return IDS_ATT_ERROR_CODE_CLIENT_CHAR_CONF_DESC_IMPROPERLY_CONFIGURED;
  }

  switch(IDS_SRCP_Context.Procedure.OpCode)
  {
    case IDS_SRCP_RESET_STATUS:
      {
        if(requestDataLength != IDS_SRCP_RESET_REQUEST_LENGTH)
        {
          // STM_TODO : Implement error management, error management to be further extended, no propagation currently
          LOG_INFO_APP("IDS_ATT_ERROR_CODE_INVALIDE_OPERAND\r\n"); 
          return IDS_ATT_ERROR_CODE_INVALIDE_OPERAND;
        }
      }
      break;
     
    case IDS_SRCP_GET_ACTIVE_BOLUS_IDS:
    case IDS_SRCP_GET_ACTIVE_BASAL_RATE_DELIVERY:
    case IDS_SRCP_GET_TOTAL_DAILY_INSULIN_STATUS:
    case IDS_SRCP_GET_DELIVERED_INSULIN:
    case IDS_SRCP_GET_INSULIN_ON_BOARD:
      {
        if(requestDataLength != IDS_SRCP_NO_OPERAND_REQUEST_LENGTH)
        {
          // STM_TODO : Implement error management, error management to be further extended, no propagation currently
          LOG_INFO_APP("IDS_ATT_ERROR_CODE_INVALIDE_OPERAND\r\n"); 
          return IDS_ATT_ERROR_CODE_INVALIDE_OPERAND;
        }
      }
      break;
     
    case IDS_SRCP_GET_ACTIVE_BOLUS_DELIVERY:
      {
        if(requestDataLength != IDS_SRCP_GET_ACTIVE_BOLUS_DELIVERY_REQUEST_LENGTH)
        {
          // STM_TODO : Implement error management, error management to be further extended, no propagation currently
          LOG_INFO_APP("IDS_ATT_ERROR_CODE_INVALIDE_OPERAND\r\n"); 
          return IDS_ATT_ERROR_CODE_INVALIDE_OPERAND;
        }
      }
      break;
     
    case IDS_SRCP_GET_COUNTER:
      {
        if(requestDataLength != IDS_SRCP_GET_ACTIVE_COUNTER_REQUEST_LENGTH)
        {
          // STM_TODO : Implement error management, error management to be further extended, no propagation currently
          LOG_INFO_APP("IDS_ATT_ERROR_CODE_INVALIDE_OPERAND\r\n"); 
          return IDS_ATT_ERROR_CODE_INVALIDE_OPERAND;
        }
      }
      break;
     
     default:
      break;
  }
  
  IDS_SRCP_Context.Last_Received_E2E_Counter = IDS_SRCP_Context.Current_Received_E2E_Counter;
  IDS_SRCP_Context.Current_Received_E2E_Counter = pRequestData[requestDataLength - 3];
  if(IDS_SRCP_Context.Transmit_E2E_Counter == 255)
  {
    IDS_SRCP_Context.Transmit_E2E_Counter = 0;
  }
  if(IDS_SRCP_Context.Last_Received_E2E_Counter == 255)
  {
    IDS_SRCP_Context.Last_Received_E2E_Counter = 0;
  }
  if(((IDS_SRCP_Context.Current_Received_E2E_Counter - IDS_SRCP_Context.Transmit_E2E_Counter) - 
      (IDS_SRCP_Context.Last_Received_E2E_Counter - (IDS_SRCP_Context.Transmit_E2E_Counter % 255))) != 1)
  {
    LOG_INFO_APP("Invalid Counter\r\n"); 
    return IDS_ATT_ERROR_CODE_INVALID_COUNTER;
  }
  if(((IDS_APP_GetFeature()) & E2E_PROTECTION_SUPPORTED) == (E2E_PROTECTION_SUPPORTED))
  {
    E2E_CRC = (pRequestData[requestDataLength - 1] << 8) | pRequestData[requestDataLength - 2];
    LOG_INFO_APP("requestDataLength: 0x%x\r\n", requestDataLength); 
    LOG_INFO_APP("Received E2E_CRC: 0x%x\r\n", E2E_CRC); 
    LOG_INFO_APP("Calculated E2E_CRC: 0x%x\r\n", (uint16_t)IDS_APP_ComputeCRC(pRequestData, requestDataLength - 2)); 

    if ((uint16_t)IDS_APP_ComputeCRC(pRequestData, requestDataLength - 2) != E2E_CRC)
    {
      LOG_INFO_APP("Invalid E2E_CRC\r\n"); 
      return IDS_ATT_ERROR_CODE_INVALID_CRC;
    }
  }
  LOG_INFO_APP("IDS_SRCP_CheckRequestValid <<<\r\n");
  return 0;
} /* end of IDS_SRCP_CheckRequestValid() */

/**
  * @brief Process the acknowledge of sent indication
  * @param None
  * @retval None
  */
void IDS_SRCP_AcknowledgeHandler(void)
{
  // STM_TODO : Implement indication acknowledge reception handler, in fact all the operations should be finished after the indication acknowledge is received from GATT client, if not received within 30sec, procedure failed
} /* end IDS_SRCP_AcknowledgeHandler() */

void IDS_SRCP_InitSingleActiveBolus(void)
{
  ActiveBolusNumber = 1;
}

void IDS_SRCP_SetNoActiveBasalRateDelivery(void)
{
  ActiveBasalRateDeliveryNumber = 0;
}

/**
  * @brief Initialize Transmit E2E Counter, Last Received E2E Counter,
  *        Current Received E2E Counter values
  * @param None
  * @retval None
  */
void IDS_SRCP_InitE2ECounter(void)
{
  IDS_SRCP_Context.Last_Received_E2E_Counter = 0;
  IDS_SRCP_Context.Current_Received_E2E_Counter = 0;
  IDS_SRCP_Context.Transmit_E2E_Counter = 0;
} /* end of IDS_SRCP_InitE2ECounter() */
