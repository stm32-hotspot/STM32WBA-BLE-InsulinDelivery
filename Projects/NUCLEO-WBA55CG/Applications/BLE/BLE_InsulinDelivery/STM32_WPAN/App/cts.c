/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service2.c
  * @author  MCD Application Team
  * @brief   service2 definition.
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
#include "common_blesvc.h"
#include "cts.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef struct{
  uint16_t  CtsSvcHdle;                  /**< Cts Service Handle */
  uint16_t  CutCharHdle;                  /**< CUT Characteristic Handle */
  uint16_t  LtiCharHdle;                  /**< LTI Characteristic Handle */
  uint16_t  RtiCharHdle;                  /**< RTI Characteristic Handle */
/* USER CODE BEGIN Context */
  /* Place holder for Characteristic Descriptors Handle*/

/* USER CODE END Context */
}CTS_Context_t;

/* Private defines -----------------------------------------------------------*/
#define UUID_128_SUPPORTED  1

#if (UUID_128_SUPPORTED == 1)
#define BM_UUID_LENGTH  UUID_TYPE_128
#else
#define BM_UUID_LENGTH  UUID_TYPE_16
#endif

#define BM_REQ_CHAR_SIZE    (3)

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
#define CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET         2
#define CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET              1
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static const uint16_t SizeCut = 10;
static const uint16_t SizeLti = 18;
static const uint16_t SizeRti = 4;

static CTS_Context_t CTS_Context;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static SVCCTL_EvtAckStatus_t CTS_EventHandler(void *p_pckt);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
/* USER CODE BEGIN PFD */

/* USER CODE END PFD */

/* Private functions ----------------------------------------------------------*/

#define COPY_UUID_128(uuid_struct, uuid_15, uuid_14, uuid_13, uuid_12, uuid_11, uuid_10, uuid_9, uuid_8, uuid_7, uuid_6, uuid_5, uuid_4, uuid_3, uuid_2, uuid_1, uuid_0) \
do {\
    uuid_struct[0] = uuid_0; uuid_struct[1] = uuid_1; uuid_struct[2] = uuid_2; uuid_struct[3] = uuid_3; \
    uuid_struct[4] = uuid_4; uuid_struct[5] = uuid_5; uuid_struct[6] = uuid_6; uuid_struct[7] = uuid_7; \
    uuid_struct[8] = uuid_8; uuid_struct[9] = uuid_9; uuid_struct[10] = uuid_10; uuid_struct[11] = uuid_11; \
    uuid_struct[12] = uuid_12; uuid_struct[13] = uuid_13; uuid_struct[14] = uuid_14; uuid_struct[15] = uuid_15; \
}while(0)

/* USER CODE BEGIN PF */

/* USER CODE END PF */

/**
 * @brief  Event handler
 * @param  p_Event: Address of the buffer holding the p_Event
 * @retval Ack: Return whether the p_Event has been managed or not
 */
static SVCCTL_EvtAckStatus_t CTS_EventHandler(void *p_Event)
{
  SVCCTL_EvtAckStatus_t return_value;
  hci_event_pckt *p_event_pckt;
  evt_blecore_aci *p_blecore_evt;
  aci_gatt_attribute_modified_event_rp0 *p_attribute_modified;
  aci_gatt_write_permit_req_event_rp0   *p_write_perm_req;
  CTS_NotificationEvt_t                 notification;
  /* USER CODE BEGIN Service2_EventHandler_1 */

  /* USER CODE END Service2_EventHandler_1 */

  return_value = SVCCTL_EvtNotAck;
  p_event_pckt = (hci_event_pckt *)(((hci_uart_pckt*)p_Event)->data);

  switch(p_event_pckt->evt)
  {
    case HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE:
      p_blecore_evt = (evt_blecore_aci*)p_event_pckt->data;
      switch(p_blecore_evt->ecode)
      {
        case ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE:
        {
          /* USER CODE BEGIN EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_BEGIN */

          /* USER CODE END EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_BEGIN */
          p_attribute_modified = (aci_gatt_attribute_modified_event_rp0*)p_blecore_evt->data;
          notification.ConnectionHandle         = p_attribute_modified->Connection_Handle;
          notification.AttributeHandle          = p_attribute_modified->Attr_Handle;
          notification.DataTransfered.Length    = p_attribute_modified->Attr_Data_Length;
          notification.DataTransfered.p_Payload = p_attribute_modified->Attr_Data;
          if(p_attribute_modified->Attr_Handle == (CTS_Context.CutCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /* USER CODE BEGIN Service2_Char_1 */

            /* USER CODE END Service2_Char_1 */
            switch(p_attribute_modified->Attr_Data[0])
            {
              /* USER CODE BEGIN Service2_Char_1_attribute_modified */

              /* USER CODE END Service2_Char_1_attribute_modified */

              /* Disabled Notification management */
              case (!(COMSVC_Notification)):
                /* USER CODE BEGIN Service2_Char_1_Disabled_BEGIN */

                /* USER CODE END Service2_Char_1_Disabled_BEGIN */
                notification.EvtOpcode = CTS_CUT_NOTIFY_DISABLED_EVT;
                CTS_Notification(&notification);
                /* USER CODE BEGIN Service2_Char_1_Disabled_END */

                /* USER CODE END Service2_Char_1_Disabled_END */
                break;

              /* Enabled Notification management */
              case COMSVC_Notification:
                /* USER CODE BEGIN Service2_Char_1_COMSVC_Notification_BEGIN */

                /* USER CODE END Service2_Char_1_COMSVC_Notification_BEGIN */
                notification.EvtOpcode = CTS_CUT_NOTIFY_ENABLED_EVT;
                CTS_Notification(&notification);
                /* USER CODE BEGIN Service2_Char_1_COMSVC_Notification_END */

                /* USER CODE END Service2_Char_1_COMSVC_Notification_END */
                break;

              default:
                /* USER CODE BEGIN Service2_Char_1_default */

                /* USER CODE END Service2_Char_1_default */
                break;
            }
          }  /* if(p_attribute_modified->Attr_Handle == (CTS_Context.CutCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))*/

          else if(p_attribute_modified->Attr_Handle == (CTS_Context.CutCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;

            notification.EvtOpcode = CTS_CUT_WRITE_EVT;
            /* USER CODE BEGIN Service2_Char_1_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */

            /* USER CODE END Service2_Char_1_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
            CTS_Notification(&notification);
          } /* if(p_attribute_modified->Attr_Handle == (CTS_Context.CutCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/
          else if(p_attribute_modified->Attr_Handle == (CTS_Context.LtiCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;

            notification.EvtOpcode = CTS_LTI_WRITE_EVT;
            /* USER CODE BEGIN Service2_Char_2_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */

            /* USER CODE END Service2_Char_2_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
            CTS_Notification(&notification);
          } /* if(p_attribute_modified->Attr_Handle == (CTS_Context.LtiCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

          /* USER CODE BEGIN EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_END */

          /* USER CODE END EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_END */
          break;/* ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
        }
        case ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE :
        {
          /* USER CODE BEGIN EVT_BLUE_GATT_READ_PERMIT_REQ_BEGIN */

          /* USER CODE END EVT_BLUE_GATT_READ_PERMIT_REQ_BEGIN */

          /* USER CODE BEGIN EVT_BLUE_GATT_READ_PERMIT_REQ_END */

          /* USER CODE END EVT_BLUE_GATT_READ_PERMIT_REQ_END */
          break;/* ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE */
        }
        case ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE:
        {
          /* USER CODE BEGIN EVT_BLUE_GATT_WRITE_PERMIT_REQ_BEGIN */

          /* USER CODE END EVT_BLUE_GATT_WRITE_PERMIT_REQ_BEGIN */
          p_write_perm_req = (aci_gatt_write_permit_req_event_rp0*)p_blecore_evt->data;
          if(p_write_perm_req->Attribute_Handle == (CTS_Context.CutCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /*USER CODE BEGIN Service2_Char_1_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE */
            APP_DBG_MSG("CTS event -> ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE -> (CurrentTimeCharHdle + 1)\n\r");
            aci_gatt_write_resp(p_write_perm_req->Connection_Handle,
                                p_write_perm_req->Attribute_Handle, 0x01, /* write_status = 1 (error))*/
                                CTS_ERR_CODE_DATA_FIELD_IGNORED, /* err_code */
                                p_write_perm_req->Data_Length,
                                (uint8_t*) &(p_write_perm_req->Data[0]));
            /*USER CODE END Service2_Char_1_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE*/
          } /*if(p_write_perm_req->Attribute_Handle == (CTS_Context.CutCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

          else if(p_write_perm_req->Attribute_Handle == (CTS_Context.LtiCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /*USER CODE BEGIN Service2_Char_2_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE */
            APP_DBG_MSG("CTS event -> ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE -> (LocalTimeCharHdle + 1)\n\r");
            if((((uint16_t)(p_write_perm_req->Data[0]) > 0xCF) && ((uint16_t)(p_write_perm_req->Data[0]) <= 0xFF)) ||
               (((p_write_perm_req->Data[0]) < 0x39) &&
               ((p_write_perm_req->Data[1] == 0x00) || (p_write_perm_req->Data[1] == 0x02) || (p_write_perm_req->Data[1] == 0x04) ||
               (p_write_perm_req->Data[1] == 0x08) || (p_write_perm_req->Data[1] == 0xFF))))
            {
              aci_gatt_write_resp(p_write_perm_req->Connection_Handle,
                                  p_write_perm_req->Attribute_Handle,
                                  0x00, /* write_status = 0 (no error))*/
                                  0x00, /* err_code */
                                  p_write_perm_req->Data_Length,
                                  (uint8_t *)&(p_write_perm_req->Data[0]));
            }
            else
            {
              aci_gatt_write_resp(p_write_perm_req->Connection_Handle,
                                  p_write_perm_req->Attribute_Handle, 0x01, /* write_status = 1 (error))*/
                                  CTS_ERR_CODE_DATA_FIELD_IGNORED, /* err_code */
                                  p_write_perm_req->Data_Length,
                                  (uint8_t*) &(p_write_perm_req->Data[0]));
            }
            /*USER CODE END Service2_Char_2_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE*/
          } /*if(p_write_perm_req->Attribute_Handle == (CTS_Context.LtiCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

          /* USER CODE BEGIN EVT_BLUE_GATT_WRITE_PERMIT_REQ_END */

          /* USER CODE END EVT_BLUE_GATT_WRITE_PERMIT_REQ_END */
          break;/* ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE */
        }
        case ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE:
        {
          aci_gatt_tx_pool_available_event_rp0 *p_tx_pool_available_event;
          p_tx_pool_available_event = (aci_gatt_tx_pool_available_event_rp0 *) p_blecore_evt->data;
          UNUSED(p_tx_pool_available_event);

          /* USER CODE BEGIN ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE */

          /* USER CODE END ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE */
          break;/* ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE*/
        }
        case ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE:
        {
          aci_att_exchange_mtu_resp_event_rp0 *p_exchange_mtu;
          p_exchange_mtu = (aci_att_exchange_mtu_resp_event_rp0 *)  p_blecore_evt->data;
          UNUSED(p_exchange_mtu);

          /* USER CODE BEGIN ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */

          /* USER CODE END ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */
          break;/* ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */
        }
        /* USER CODE BEGIN BLECORE_EVT */

        /* USER CODE END BLECORE_EVT */
        default:
          /* USER CODE BEGIN EVT_DEFAULT */

          /* USER CODE END EVT_DEFAULT */
          break;
      }
      /* USER CODE BEGIN EVT_VENDOR*/

      /* USER CODE END EVT_VENDOR*/
      break; /* HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE */

      /* USER CODE BEGIN EVENT_PCKT_CASES*/

      /* USER CODE END EVENT_PCKT_CASES*/

    default:
      /* USER CODE BEGIN EVENT_PCKT*/

      /* USER CODE END EVENT_PCKT*/
      break;
  }

  /* USER CODE BEGIN Service2_EventHandler_2 */

  /* USER CODE END Service2_EventHandler_2 */

  return(return_value);
}/* end CTS_EventHandler */

/* Public functions ----------------------------------------------------------*/

/**
 * @brief  Service initialization
 * @param  None
 * @retval None
 */
void CTS_Init(void)
{
  Char_UUID_t  uuid;
  tBleStatus ret;
  uint8_t max_attr_record;

  /* USER CODE BEGIN SVCCTL_InitService2Svc_1 */

  /* USER CODE END SVCCTL_InitService2Svc_1 */

  /**
   *  Register the event handler to the BLE controller
   */
  SVCCTL_RegisterSvcHandler(CTS_EventHandler);

  /**
   * CTS
   *
   * Max_Attribute_Records = 1 + 2*3 + 1*no_of_char_with_notify_or_indicate_property + 1*no_of_char_with_broadcast_property
   * service_max_attribute_record = 1 for CTS +
   *                                2 for CUT +
   *                                2 for LTI +
   *                                2 for RTI +
   *                                1 for CUT configuration descriptor +
   *                              = 8
   * This value doesn't take into account number of descriptors manually added
   * In case of descriptors added, please update the max_attr_record value accordingly in the next SVCCTL_InitService User Section
   */
  max_attr_record = 8;

  /* USER CODE BEGIN SVCCTL_InitService */
  /* max_attr_record to be updated if descriptors have been added */

  /* USER CODE END SVCCTL_InitService */

  uuid.Char_UUID_16 = 0x1805;
  ret = aci_gatt_add_service(UUID_TYPE_16,
                             (Service_UUID_t *) &uuid,
                             PRIMARY_SERVICE,
                             max_attr_record,
                             &(CTS_Context.CtsSvcHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_service command: CTS, error code: 0x%x \n\r", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_service command: CTS \n\r");
  }

  /**
   * CUT
   */
  uuid.Char_UUID_16 = 0x2a2b;
  ret = aci_gatt_add_char(CTS_Context.CtsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeCut,
                          CHAR_PROP_READ | CHAR_PROP_WRITE | CHAR_PROP_NOTIFY,
                          ATTR_PERMISSION_NONE,
                          GATT_NOTIFY_ATTRIBUTE_WRITE | GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP,
                          0x10,
                          CHAR_VALUE_LEN_CONSTANT,
                          &(CTS_Context.CutCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : CUT, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : CUT\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService2Char1 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService2Char1 */

  /**
   * LTI
   */
  uuid.Char_UUID_16 = 0x2a0f;
  ret = aci_gatt_add_char(CTS_Context.CtsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeLti,
                          CHAR_PROP_READ | CHAR_PROP_WRITE,
                          ATTR_PERMISSION_NONE,
                          GATT_NOTIFY_ATTRIBUTE_WRITE | GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP,
                          0x10,
                          CHAR_VALUE_LEN_CONSTANT,
                          &(CTS_Context.LtiCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : LTI, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : LTI\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService2Char2 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService2Char2 */

  /**
   * RTI
   */
  uuid.Char_UUID_16 = 0x2a14;
  ret = aci_gatt_add_char(CTS_Context.CtsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeRti,
                          CHAR_PROP_READ,
                          ATTR_PERMISSION_NONE,
                          GATT_DONT_NOTIFY_EVENTS,
                          0x10,
                          CHAR_VALUE_LEN_CONSTANT,
                          &(CTS_Context.RtiCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : RTI, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : RTI\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService2Char3 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService2Char3 */

  /* USER CODE BEGIN SVCCTL_InitService2Svc_2 */

  /* USER CODE END SVCCTL_InitService2Svc_2 */

  return;
}

/**
 * @brief  Characteristic update
 * @param  CharOpcode: Characteristic identifier
 * @param  Service_Instance: Instance of the service to which the characteristic belongs
 *
 */
tBleStatus CTS_UpdateValue(CTS_CharOpcode_t CharOpcode, CTS_Data_t *pData)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  /* USER CODE BEGIN Service2_App_Update_Char_1 */

  /* USER CODE END Service2_App_Update_Char_1 */

  switch(CharOpcode)
  {
    case CTS_CUT:
      ret = aci_gatt_update_char_value(CTS_Context.CtsSvcHdle,
                                       CTS_Context.CutCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value CUT command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value CUT command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_1*/

      /* USER CODE END Service2_Char_Value_1*/
      break;

    case CTS_LTI:
      ret = aci_gatt_update_char_value(CTS_Context.CtsSvcHdle,
                                       CTS_Context.LtiCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value LTI command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value LTI command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_2*/

      /* USER CODE END Service2_Char_Value_2*/
      break;

    case CTS_RTI:
      ret = aci_gatt_update_char_value(CTS_Context.CtsSvcHdle,
                                       CTS_Context.RtiCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value RTI command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value RTI command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_3*/

      /* USER CODE END Service2_Char_Value_3*/
      break;

    default:
      break;
  }

  /* USER CODE BEGIN Service2_App_Update_Char_2 */

  /* USER CODE END Service2_App_Update_Char_2 */

  return ret;
}
