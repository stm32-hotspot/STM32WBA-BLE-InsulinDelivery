
/**
  ******************************************************************************
  * @file    ids_srcp.h
  * @author  MCD Application Team
  * @brief   Header for ids_srcp.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __IDS_SRCP_H
#define __IDS_SRCP_H

#ifdef __cplusplus
extern "C" 
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "ids.h"
#include "ids_app.h" 

/* Exported defines-----------------------------------------------------------*/
#define IDS_SRCP_OP_CODE_OPERAND_MAX_LENGTH                                 (15)

/* Exported types ------------------------------------------------------------*/
  
typedef enum
{
  IDS_SRCP_STATE_IDLE,
  IDS_SRCP_STATE_PROCEDURE_IN_PROGRESS,
  IDS_SRCP_STATE_PROCEDURE_FINISHED,
  IDS_SRCP_STATE_PROCEDURE_RESPONSE_PENDING,
  IDS_SRCP_STATE_WAITING_FOR_RESPONSE_ACKNOWLEDGE
} IDS_SRCP_State_t;

typedef __PACKED_STRUCT
{
  uint8_t RequestOpCode;
  uint8_t ResponseCode;
} IDS_SRCP_GeneralResponse_t;

typedef __PACKED_STRUCT
{
  uint16_t OpCode;
  uint8_t Operand[IDS_SRCP_OP_CODE_OPERAND_MAX_LENGTH];
} IDS_SRCP_Procedure_t;

typedef __PACKED_STRUCT
{
  uint8_t OpCode;
  uint8_t Operand[IDS_SRCP_OP_CODE_OPERAND_MAX_LENGTH];
} IDS_SRCP_Response_t;

#if 0
typedef struct
{
  IDS_SRCP_App_EventCode_t      EventCode;
  IDS_Data_t                    EventData;
  uint16_t                       ConnectionHandle;
  uint8_t                        ServiceInstance;
} IDS_SRCP_App_Event_t;
#endif
/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

void IDS_SRCP_Init(void);
void IDS_SRCP_RequestHandler(uint8_t * pRequestData, uint8_t requestDataLength);
void IDS_SRCP_AcknowledgeHandler(void);
uint8_t IDS_SRCP_CheckRequestValid(uint8_t * pRequestData, uint8_t requestDataLength);
void IDS_SRCP_InitSingleActiveBolus(void);
void IDS_SRCP_SetNoActiveBasalRateDelivery(void);
void IDS_SRCP_InitE2ECounter(void);
//uint8_t IDS_SRCP_APP_EventHandler(IDS_SRCP_App_Event_t * pNotification);

#endif /* __IDS_SRCP_H */
