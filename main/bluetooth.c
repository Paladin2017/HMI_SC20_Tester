/* UART asynchronous example, that uses separate RX and TX tasks

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"
#include "string.h"
#include "protocal.h"

#include "time.h"
#include "sys/time.h"

#define SPP_TAG "HMI_TESTER"
#define SPP_SERVER_NAME "HMI_BT"
#define EXAMPLE_DEVICE_NAME "ESP_HMI_HOST"

static int bt_push_data(uint8_t *pdata, int len);

typedef enum {
    M_REPORT_VERSIONS = 0x1,
    S_REPORT_VERSIONS,
    M_CAMERA_GET_AWB = 0x3,
    S_CAMERA_GET_AWB_ACK,
    M_CAMERA_SET_AWB = 0x5,
    S_CAMERA_SET_AWB_ACK,
    M_CAMERA_SET_ACE = 0x7,
    S_CAMERA_SET_ACE_ACK,
    M_CAMERA_SET_IMG_SIZE = 0x9,
    S_CAMERA_SET_IMG_SIZE_ACK,
    M_CAMERA_SET_QUALITY = 0xb,
    S_CAMERA_SET_QUALITY_ACK,
    M_CAMERA_GET_IMG = 0xd,
    S_CAMERA_IMG_ACK,
    M_UPDATE_MOUDLE = 0xf,
    S_UPDATRE_ACK,
    M_SET_BT_NAME = 0x11,
    S_SET_BT_NAME_ACK,
    M_REPORT_BT_NAME = 0x13,
    S_REPORT_BT_NAME_ACK,
    M_REPORT_BT_MAC = 0x15,
    S_REPORT_BT_MAC_ACK,
    M_SET_CAMERA_LIGHT = 0x17,
    S_SET_CAMERA_LIGHT_ACK,
    M_REPORT_CAMERA_LIGHT = 0x19,
    S_REPORT_CAMERA_LIGHT_ACK,
    M_REPORT_CAMERA_STATU = 0x1b,
    S_REPORT_CAMERA_STATU_ACK,

    S_CAMERA_INIT_FAIL = 0xfd,
    S_RECV_FAIL = 0xff,
}BT_CMD_E;


uint8_t bt_recv_buff[1024];
uint16_t bt_recv_read_len;
uint16_t bt_recv_head;
uint16_t bt_recv_tail;

/**
  * @brief  Send data via bluetooth
  * @param  pData:The buffer of the data
  * @param  Len:The length of the data
  * @retval None
  */
void bt_send_pack(uint8_t *pData, int Len) {

}

/**
  * @brief  Flush the receive buffer of the bluetooth
  * @retval None
  */
static void bt_flush_buffer() {
  bt_recv_head = bt_recv_tail = 0;
}

/**
  * @brief  Packup a pack data 
  * @param  EventID:The EventID
  * @param  OpCode:The operation code
  * @param  pData:The buffer of the source data
  * @param  Len:The length of the source data
  * @retval None
  */
void bt_send_protocal_data(uint8_t EventID, uint8_t OpCode, uint8_t *pData, int Len) {
  uint8_t* pack_buff = (uint8_t*) malloc(Len + 2);
  uint8_t* send_buff = (uint8_t*) malloc(Len + 10);
  int send_len;
  pack_buff[0] = EventID;
  pack_buff[1] = OpCode;
  memcpy((byte*)&pack_buff[2], pData, Len);
  send_len = SetupPack(pack_buff, Len + 2, send_buff);
  bt_send_pack(send_buff, send_len);
}

/**
  * @brief  Bluetooth receive data process thread
  * @retval None
  */
static void bt_rx_task(void *arg)
{
    static const char *RX_TASK_TAG = "BT_RX_TASK";
    uint8_t eventId;
    uint8_t* process_buff = (uint8_t*) malloc(512);
    while (1) {
        if(Parse(bt_recv_buff, &bt_recv_head, &bt_recv_tail, sizeof(bt_recv_buff), process_buff) > 0) {
          eventId = process_buff[8];
          switch(eventId) {
            
            //设置相机曝光时间
            case M_CAMERA_SET_ACE:
                Cmd_CameraSetAEC((parse_buff[10] << 8) | parse_buff[11]);
                break;
            // 设置图像大小
            case M_CAMERA_SET_IMG_SIZE:
                Cmd_CameraSetImgSize(parse_buff[10]);
                break;
            // 设置图像质量
            case M_CAMERA_SET_QUALITY:
                Cmd_CameraSetQuality(parse_buff[10]);
                break;

            // Take a image
            case M_CAMERA_GET_IMG:
                Cmd_ReportImg(&parse_buff[8], len);
                break;
            case M_REPORT_BT_MAC:
                report_bt_mac();
                break;
            case M_SET_CAMERA_LIGHT:
                Cmd_CameraSetLight(&parse_buff[8], len);
                break;
            case M_REPORT_CAMERA_LIGHT:
                Cmd_CameraReportLight(&parse_buff[8], len);
                break;
            case M_REPORT_CAMERA_STATU:
                Cmd_CameraReportStatu();
                break;
          }
        }
    }
    free(process_buff);
}

static int bt_push_data(uint8_t *pdata, int len) {
  int i;
  int data_pushed;
  int next_head;
  
  data_pushed = 0;
  for(i=0;i<len;i++) {
    next_head = bt_recv_head + 1 % sizeof(bt_recv_buff);
    if(next_head == bt_recv_tail)
      break;
    bt_recv_buff[bt_recv_head] = pdata[i];
    bt_recv_head = next_head;
    data_pushed++;
  }
  return data_pushed;
}

void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
    switch (event) {
    case ESP_BT_GAP_AUTH_CMPL_EVT:{
        if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS) {
            ESP_LOGI(SPP_TAG, "authentication success: %s", param->auth_cmpl.device_name);
            esp_log_buffer_hex(SPP_TAG, param->auth_cmpl.bda, ESP_BD_ADDR_LEN);
        } else {
            ESP_LOGE(SPP_TAG, "authentication failed, status:%d", param->auth_cmpl.stat);
        }
        break;
    }
    case ESP_BT_GAP_PIN_REQ_EVT:{
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_PIN_REQ_EVT min_16_digit:%d", param->pin_req.min_16_digit);
        if (param->pin_req.min_16_digit) {
            ESP_LOGI(SPP_TAG, "Input pin code: 0000 0000 0000 0000");
            esp_bt_pin_code_t pin_code = {0};
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 16, pin_code);
        } else {
            ESP_LOGI(SPP_TAG, "Input pin code: 1234");
            esp_bt_pin_code_t pin_code;
            pin_code[0] = '1';
            pin_code[1] = '2';
            pin_code[2] = '3';
            pin_code[3] = '4';
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 4, pin_code);
        }
        break;
    }

#if (CONFIG_BT_SSP_ENABLED == true)
    case ESP_BT_GAP_CFM_REQ_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_CFM_REQ_EVT Please compare the numeric value: %d", param->cfm_req.num_val);
        esp_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
        break;
    case ESP_BT_GAP_KEY_NOTIF_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_KEY_NOTIF_EVT passkey:%d", param->key_notif.passkey);
        break;
    case ESP_BT_GAP_KEY_REQ_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_KEY_REQ_EVT Please enter passkey!");
        break;
#endif

    default: {
        ESP_LOGI(SPP_TAG, "event: %d", event);
        break;
    }
    }
    return;
}

static void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    switch (event) {
    case ESP_SPP_INIT_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_INIT_EVT");
        esp_bt_dev_set_device_name(EXAMPLE_DEVICE_NAME);
        esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
        esp_spp_start_srv(sec_mask,role_slave, 0, SPP_SERVER_NAME);
        break;
    case ESP_SPP_DISCOVERY_COMP_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_DISCOVERY_COMP_EVT");
        break;
    case ESP_SPP_OPEN_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_OPEN_EVT");
        break;
    case ESP_SPP_CLOSE_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CLOSE_EVT");
        break;
    case ESP_SPP_START_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_START_EVT");
        break;
    case ESP_SPP_CL_INIT_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CL_INIT_EVT");
        break;
    case ESP_SPP_DATA_IND_EVT:
        bt_push_data(param->data_ind.data, param->data_ind.len);
        break;
    case ESP_SPP_CONG_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CONG_EVT");
        break;
    case ESP_SPP_WRITE_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_WRITE_EVT");
        break;
    case ESP_SPP_SRV_OPEN_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_SRV_OPEN_EVT");
        gettimeofday(&time_old, NULL);
        break;
    default:
        break;
    }
}

void bluetooth_init(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    if ((ret = esp_bt_controller_init(&bt_cfg)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s initialize controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s enable controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bluedroid_init()) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s initialize bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bluedroid_enable()) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s enable bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bt_gap_register_callback(esp_bt_gap_cb)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s gap register failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_spp_register_callback(esp_spp_cb)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s spp register failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_spp_init(ESP_SPP_MODE_CB)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s spp init failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

#if (CONFIG_BT_SSP_ENABLED == true)
    /* Set default parameters for Secure Simple Pairing */
    esp_bt_sp_param_t param_type = ESP_BT_SP_IOCAP_MODE;
    esp_bt_io_cap_t iocap = ESP_BT_IO_CAP_IO;
    esp_bt_gap_set_security_param(param_type, &iocap, sizeof(uint8_t));
#endif

    /*
     * Set default parameters for Legacy Pairing
     * Use variable pin, input pin code when pairing
     */
    esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_VARIABLE;
    esp_bt_pin_code_t pin_code;
    esp_bt_gap_set_pin(pin_type, 0, pin_code);

    xTaskCreate(bt_rx_task, "bt_rx_task", 8192*2, NULL, configMAX_PRIORITIES, NULL);
}
