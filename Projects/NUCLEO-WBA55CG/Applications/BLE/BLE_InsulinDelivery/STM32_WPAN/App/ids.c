/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service4.c
  * @author  MCD Application Team
  * @brief   service4 definition.
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
#include "ids.h"

/* USER CODE BEGIN Includes */
#include "ids_app.h"
#include "stm32_seq.h"
#include "app_conf.h"
#include "ids_app.h"
#include "ids_srcp.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef struct{
  uint16_t  IdsSvcHdle;                  /**< Ids Service Handle */
  uint16_t  IdscCharHdle;                  /**< IDSC Characteristic Handle */
  uint16_t  IdsCharHdle;                  /**< IDS Characteristic Handle */
  uint16_t  IdasCharHdle;                  /**< IDAS Characteristic Handle */
  uint16_t  IdfCharHdle;                  /**< IDF Characteristic Handle */
  uint16_t  IdsrcpCharHdle;                  /**< IDSRCP Characteristic Handle */
/* USER CODE BEGIN Context */
  /* Place holder for Characteristic Descriptors Handle*/

/* USER CODE END Context */
}IDS_Context_t;

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
static const uint16_t SizeIdsc = 5;
static const uint16_t SizeIds = 8;
static const uint16_t SizeIdas = 19;
static const uint16_t SizeIdf = 8;
static const uint16_t SizeIdsrcp = 20;

static IDS_Context_t IDS_Context;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static SVCCTL_EvtAckStatus_t IDS_EventHandler(void *p_pckt);

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
static SVCCTL_EvtAckStatus_t IDS_EventHandler(void *p_Event)
{
  SVCCTL_EvtAckStatus_t return_value;
  hci_event_pckt *p_event_pckt;
  evt_blecore_aci *p_blecore_evt;
  aci_gatt_attribute_modified_event_rp0 *p_attribute_modified;
  aci_gatt_write_permit_req_event_rp0   *p_write_perm_req;
  IDS_NotificationEvt_t                 notification;
  /* USER CODE BEGIN Service4_EventHandler_1 */
  uint8_t error_code = 0;
  /* USER CODE END Service4_EventHandler_1 */

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
          if(p_attribute_modified->Attr_Handle == (IDS_Context.IdscCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /* USER CODE BEGIN Service4_Char_1 */

            /* USER CODE END Service4_Char_1 */

            switch(p_attribute_modified->Attr_Data[0])
            {
              /* USER CODE BEGIN Service4_Char_1_attribute_modified */

              /* USER CODE END Service4_Char_1_attribute_modified */

              /* Disabled Indication management */
              case (!(COMSVC_Indication)):
                /* USER CODE BEGIN Service4_Char_1_Disabled_BEGIN */

                /* USER CODE END Service4_Char_1_Disabled_BEGIN */
                notification.EvtOpcode = IDS_IDSC_INDICATE_DISABLED_EVT;
                IDS_Notification(&notification);
                /* USER CODE BEGIN Service4_Char_1_Disabled_END */

                /* USER CODE END Service4_Char_1_Disabled_END */
                break;

              /* Enabled Indication management */
              case COMSVC_Indication:
                /* USER CODE BEGIN Service4_Char_1_COMSVC_Indication_BEGIN */

                /* USER CODE END Service4_Char_1_COMSVC_Indication_BEGIN */
                notification.EvtOpcode = IDS_IDSC_INDICATE_ENABLED_EVT;
                IDS_Notification(&notification);
                /* USER CODE BEGIN Service4_Char_1_COMSVC_Indication_END */

                /* USER CODE END Service4_Char_1_COMSVC_Indication_END */
                break;

              default:
                /* USER CODE BEGIN Service4_Char_1_default */

                /* USER CODE END Service4_Char_1_default */
                break;
            }
          }  /* if(p_attribute_modified->Attr_Handle == (IDS_Context.IDSCHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))*/

          else if(p_attribute_modified->Attr_Handle == (IDS_Context.IdsCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /* USER CODE BEGIN Service4_Char_2 */

            /* USER CODE END Service4_Char_2 */

            switch(p_attribute_modified->Attr_Data[0])
            {
              /* USER CODE BEGIN Service4_Char_2_attribute_modified */

              /* USER CODE END Service4_Char_2_attribute_modified */

              /* Disabled Indication management */
              case (!(COMSVC_Indication)):
                /* USER CODE BEGIN Service4_Char_2_Disabled_BEGIN */

                /* USER CODE END Service4_Char_2_Disabled_BEGIN */
                notification.EvtOpcode = IDS_IDS_INDICATE_DISABLED_EVT;
                IDS_Notification(&notification);
                /* USER CODE BEGIN Service4_Char_2_Disabled_END */

                /* USER CODE END Service4_Char_2_Disabled_END */
                break;

              /* Enabled Indication management */
              case COMSVC_Indication:
                /* USER CODE BEGIN Service4_Char_2_COMSVC_Indication_BEGIN */

                /* USER CODE END Service4_Char_2_COMSVC_Indication_BEGIN */
                notification.EvtOpcode = IDS_IDS_INDICATE_ENABLED_EVT;
                IDS_Notification(&notification);
                /* USER CODE BEGIN Service4_Char_2_COMSVC_Indication_END */

                /* USER CODE END Service4_Char_2_COMSVC_Indication_END */
                break;

              default:
                /* USER CODE BEGIN Service4_Char_2_default */

                /* USER CODE END Service4_Char_2_default */
                break;
            }
          }  /* if(p_attribute_modified->Attr_Handle == (IDS_Context.IDSHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))*/

          else if(p_attribute_modified->Attr_Handle == (IDS_Context.IdasCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /* USER CODE BEGIN Service4_Char_3 */

            /* USER CODE END Service4_Char_3 */

            switch(p_attribute_modified->Attr_Data[0])
            {
              /* USER CODE BEGIN Service4_Char_3_attribute_modified */

              /* USER CODE END Service4_Char_3_attribute_modified */

              /* Disabled Indication management */
              case (!(COMSVC_Indication)):
                /* USER CODE BEGIN Service4_Char_3_Disabled_BEGIN */

                /* USER CODE END Service4_Char_3_Disabled_BEGIN */
                notification.EvtOpcode = IDS_IDAS_INDICATE_DISABLED_EVT;
                IDS_Notification(&notification);
                /* USER CODE BEGIN Service4_Char_3_Disabled_END */

                /* USER CODE END Service4_Char_3_Disabled_END */
                break;

              /* Enabled Indication management */
              case COMSVC_Indication:
                /* USER CODE BEGIN Service4_Char_3_COMSVC_Indication_BEGIN */

                /* USER CODE END Service4_Char_3_COMSVC_Indication_BEGIN */
                notification.EvtOpcode = IDS_IDAS_INDICATE_ENABLED_EVT;
                IDS_Notification(&notification);
                /* USER CODE BEGIN Service4_Char_3_COMSVC_Indication_END */

                /* USER CODE END Service4_Char_3_COMSVC_Indication_END */
                break;

              default:
                /* USER CODE BEGIN Service4_Char_3_default */

                /* USER CODE END Service4_Char_3_default */
                break;
            }
          }  /* if(p_attribute_modified->Attr_Handle == (IDS_Context.IDASHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))*/

          else if(p_attribute_modified->Attr_Handle == (IDS_Context.IdfCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /* USER CODE BEGIN Service4_Char_4 */

            /* USER CODE END Service4_Char_4 */

            switch(p_attribute_modified->Attr_Data[0])
            {
              /* USER CODE BEGIN Service4_Char_4_attribute_modified */

              /* USER CODE END Service4_Char_4_attribute_modified */

              /* Disabled Indication management */
              case (!(COMSVC_Indication)):
                /* USER CODE BEGIN Service4_Char_4_Disabled_BEGIN */

                /* USER CODE END Service4_Char_4_Disabled_BEGIN */
                notification.EvtOpcode = IDS_IDF_INDICATE_DISABLED_EVT;
                IDS_Notification(&notification);
                /* USER CODE BEGIN Service4_Char_4_Disabled_END */

                /* USER CODE END Service4_Char_4_Disabled_END */
                break;

              /* Enabled Indication management */
              case COMSVC_Indication:
                /* USER CODE BEGIN Service4_Char_4_COMSVC_Indication_BEGIN */

                /* USER CODE END Service4_Char_4_COMSVC_Indication_BEGIN */
                notification.EvtOpcode = IDS_IDF_INDICATE_ENABLED_EVT;
                IDS_Notification(&notification);
                /* USER CODE BEGIN Service4_Char_4_COMSVC_Indication_END */

                /* USER CODE END Service4_Char_4_COMSVC_Indication_END */
                break;

              default:
                /* USER CODE BEGIN Service4_Char_4_default */

                /* USER CODE END Service4_Char_4_default */
                break;
            }
          }  /* if(p_attribute_modified->Attr_Handle == (IDS_Context.IDFHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))*/

          else if(p_attribute_modified->Attr_Handle == (IDS_Context.IdsrcpCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /* USER CODE BEGIN Service4_Char_5 */

            /* USER CODE END Service4_Char_5 */

            switch(p_attribute_modified->Attr_Data[0])
            {
              /* USER CODE BEGIN Service4_Char_5_attribute_modified */

              /* USER CODE END Service4_Char_5_attribute_modified */

              /* Disabled Indication management */
              case (!(COMSVC_Indication)):
                /* USER CODE BEGIN Service4_Char_5_Disabled_BEGIN */

                /* USER CODE END Service4_Char_5_Disabled_BEGIN */
                notification.EvtOpcode = IDS_IDSRCP_INDICATE_DISABLED_EVT;
                IDS_Notification(&notification);
                /* USER CODE BEGIN Service4_Char_5_Disabled_END */

                /* USER CODE END Service4_Char_5_Disabled_END */
                break;

              /* Enabled Indication management */
              case COMSVC_Indication:
                /* USER CODE BEGIN Service4_Char_5_COMSVC_Indication_BEGIN */

                /* USER CODE END Service4_Char_5_COMSVC_Indication_BEGIN */
                notification.EvtOpcode = IDS_IDSRCP_INDICATE_ENABLED_EVT;
                IDS_Notification(&notification);
                /* USER CODE BEGIN Service4_Char_5_COMSVC_Indication_END */

                /* USER CODE END Service4_Char_5_COMSVC_Indication_END */
                break;

              default:
                /* USER CODE BEGIN Service4_Char_5_default */

                /* USER CODE END Service4_Char_5_default */
                break;
            }
          }  /* if(p_attribute_modified->Attr_Handle == (IDS_Context.IDSRCPHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))*/

          else if(p_attribute_modified->Attr_Handle == (IDS_Context.IdsrcpCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;

            notification.EvtOpcode = IDS_IDSRCP_WRITE_EVT;
            /* USER CODE BEGIN Service4_Char_5_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */

            /* USER CODE END Service4_Char_5_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
            IDS_Notification(&notification);
          } /* if(p_attribute_modified->Attr_Handle == (IDS_Context.IdsrcpCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

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
          if(p_write_perm_req->Attribute_Handle == (IDS_Context.IdsrcpCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /*USER CODE BEGIN Service4_Char_5_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE */
            LOG_INFO_APP("ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE IDS_SRCP, data length: %d\r\n", p_write_perm_req->Data_Length);
        
            error_code = IDS_SRCP_CheckRequestValid(p_write_perm_req->Data, p_write_perm_req->Data_Length);
            if (error_code == 0x00)
            {
              aci_gatt_write_resp(p_write_perm_req->Connection_Handle,
                                  p_write_perm_req->Attribute_Handle,
                                  0x00,
                                  0x00,
                                  p_write_perm_req->Data_Length,
                                  p_write_perm_req->Data);
          
              LOG_INFO_APP("ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE IDS_SRCP >>> PERMITTED\r\n");
              IDS_SRCP_RequestHandler(p_write_perm_req->Data, p_write_perm_req->Data_Length);
            }
            else 
            {
              aci_gatt_write_resp(p_write_perm_req->Connection_Handle,
                                  p_write_perm_req->Attribute_Handle,
                                  0x01,
                                  error_code,
                                  p_write_perm_req->Data_Length,
                                  p_write_perm_req->Data);
              LOG_INFO_APP("ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE IDS_SRCP >>> NOT PERMITTED\r\n");
            }
            /*USER CODE END Service4_Char_5_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE*/
          } /*if(p_write_perm_req->Attribute_Handle == (IDS_Context.IdsrcpCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

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
          LOG_INFO_APP("ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE\r\n");
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
        case ACI_GATT_SERVER_CONFIRMATION_VSEVT_CODE:
        {
          LOG_INFO_APP("ACI_GATT_SERVER_CONFIRMATION_VSEVT_CODE IDS_SRCP\r\n");
          
          break;/* ACI_GATT_SERVER_CONFIRMATION_VSEVT_CODE */
        }
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

  /* USER CODE BEGIN Service4_EventHandler_2 */

  /* USER CODE END Service4_EventHandler_2 */

  return(return_value);
}/* end IDS_EventHandler */

/* Public functions ----------------------------------------------------------*/

/**
 * @brief  Service initialization
 * @param  None
 * @retval None
 */
void IDS_Init(void)
{
  Char_UUID_t  uuid;
  tBleStatus ret;
  uint8_t max_attr_record;

  /* USER CODE BEGIN SVCCTL_InitService4Svc_1 */

  /* USER CODE END SVCCTL_InitService4Svc_1 */

  /**
   *  Register the event handler to the BLE controller
   */
  SVCCTL_RegisterSvcHandler(IDS_EventHandler);

  /**
   * IDS
   *
   * Max_Attribute_Records = 1 + 2*5 + 1*no_of_char_with_notify_or_indicate_property + 1*no_of_char_with_broadcast_property
   * service_max_attribute_record = 1 for IDS +
   *                                2 for IDSC +
   *                                2 for IDS +
   *                                2 for IDAS +
   *                                2 for IDF +
   *                                2 for IDSRCP +
   *                                1 for IDSC configuration descriptor +
   *                                1 for IDS configuration descriptor +
   *                                1 for IDAS configuration descriptor +
   *                                1 for IDF configuration descriptor +
   *                                1 for IDSRCP configuration descriptor +
   *                              = 16
   * This value doesn't take into account number of descriptors manually added
   * In case of descriptors added, please update the max_attr_record value accordingly in the next SVCCTL_InitService User Section
   */
  max_attr_record = 16;

  /* USER CODE BEGIN SVCCTL_InitService */
  /* max_attr_record to be updated if descriptors have been added */

  /* USER CODE END SVCCTL_InitService */

  uuid.Char_UUID_16 = 0x183a;
  ret = aci_gatt_add_service(UUID_TYPE_16,
                             (Service_UUID_t *) &uuid,
                             PRIMARY_SERVICE,
                             max_attr_record,
                             &(IDS_Context.IdsSvcHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_service command: IDS, error code: 0x%x \n\r", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_service command: IDS \n\r");
  }

  /**
   * IDSC
   */
  uuid.Char_UUID_16 = 0x2b20;
  ret = aci_gatt_add_char(IDS_Context.IdsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeIdsc,
                          CHAR_PROP_READ | CHAR_PROP_INDICATE,
                          ATTR_PERMISSION_AUTHOR_READ | ATTR_PERMISSION_AUTHOR_WRITE,
                          GATT_DONT_NOTIFY_EVENTS,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(IDS_Context.IdscCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : IDSC, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : IDSC\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService4Char1 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService4Char1 */

  /**
   * IDS
   */
  uuid.Char_UUID_16 = 0x2b21;
  ret = aci_gatt_add_char(IDS_Context.IdsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeIds,
                          CHAR_PROP_READ | CHAR_PROP_INDICATE,
                          ATTR_PERMISSION_AUTHOR_READ | ATTR_PERMISSION_AUTHOR_WRITE,
                          GATT_DONT_NOTIFY_EVENTS,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(IDS_Context.IdsCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : IDS, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : IDS\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService4Char2 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService4Char2 */

  /**
   * IDAS
   */
  uuid.Char_UUID_16 = 0x2b22;
  ret = aci_gatt_add_char(IDS_Context.IdsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeIdas,
                          CHAR_PROP_READ | CHAR_PROP_INDICATE,
                          ATTR_PERMISSION_AUTHOR_READ | ATTR_PERMISSION_AUTHOR_WRITE,
                          GATT_DONT_NOTIFY_EVENTS,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(IDS_Context.IdasCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : IDAS, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : IDAS\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService4Char3 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService4Char3 */

  /**
   * IDF
   */
  uuid.Char_UUID_16 = 0x2b23;
  ret = aci_gatt_add_char(IDS_Context.IdsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeIdf,
                          CHAR_PROP_READ | CHAR_PROP_INDICATE,
                          ATTR_PERMISSION_AUTHOR_READ,
                          GATT_NOTIFY_ATTRIBUTE_WRITE,
                          0x10,
                          CHAR_VALUE_LEN_CONSTANT,
                          &(IDS_Context.IdfCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : IDF, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : IDF\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService4Char4 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService4Char4 */

  /**
   * IDSRCP
   */
  uuid.Char_UUID_16 = 0x2b24;
  ret = aci_gatt_add_char(IDS_Context.IdsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeIdsrcp,
                          CHAR_PROP_WRITE | CHAR_PROP_INDICATE,
                          ATTR_PERMISSION_AUTHOR_WRITE,
                          GATT_NOTIFY_ATTRIBUTE_WRITE | GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(IDS_Context.IdsrcpCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : IDSRCP, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : IDSRCP\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService4Char5 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService4Char5 */

  /* USER CODE BEGIN SVCCTL_InitService4Svc_2 */

  /* USER CODE END SVCCTL_InitService4Svc_2 */

  return;
}

/**
 * @brief  Characteristic update
 * @param  CharOpcode: Characteristic identifier
 * @param  Service_Instance: Instance of the service to which the characteristic belongs
 *
 */
tBleStatus IDS_UpdateValue(IDS_CharOpcode_t CharOpcode, IDS_Data_t *pData)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  /* USER CODE BEGIN Service4_App_Update_Char_1 */

  /* USER CODE END Service4_App_Update_Char_1 */

  switch(CharOpcode)
  {
    case IDS_IDSC:
      ret = aci_gatt_update_char_value(IDS_Context.IdsSvcHdle,
                                       IDS_Context.IdscCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value IDSC command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value IDSC command\n");
      }
      /* USER CODE BEGIN Service4_Char_Value_1*/

      /* USER CODE END Service4_Char_Value_1*/
      break;

    case IDS_IDS:
      ret = aci_gatt_update_char_value(IDS_Context.IdsSvcHdle,
                                       IDS_Context.IdsCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value IDS command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value IDS command\n");
      }
      /* USER CODE BEGIN Service4_Char_Value_2*/

      /* USER CODE END Service4_Char_Value_2*/
      break;

    case IDS_IDAS:
      ret = aci_gatt_update_char_value(IDS_Context.IdsSvcHdle,
                                       IDS_Context.IdasCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value IDAS command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value IDAS command\n");
      }
      /* USER CODE BEGIN Service4_Char_Value_3*/

      /* USER CODE END Service4_Char_Value_3*/
      break;

    case IDS_IDF:
      ret = aci_gatt_update_char_value(IDS_Context.IdsSvcHdle,
                                       IDS_Context.IdfCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value IDF command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value IDF command\n");
      }
      /* USER CODE BEGIN Service4_Char_Value_4*/

      /* USER CODE END Service4_Char_Value_4*/
      break;

    case IDS_IDSRCP:
      ret = aci_gatt_update_char_value(IDS_Context.IdsSvcHdle,
                                       IDS_Context.IdsrcpCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value IDSRCP command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value IDSRCP command\n");
      }
      /* USER CODE BEGIN Service4_Char_Value_5*/

      /* USER CODE END Service4_Char_Value_5*/
      break;

    default:
      break;
  }

  /* USER CODE BEGIN Service4_App_Update_Char_2 */

  /* USER CODE END Service4_App_Update_Char_2 */

  return ret;
}
