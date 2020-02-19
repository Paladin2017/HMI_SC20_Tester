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
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"
#include "esp_vfs.h"
#include "string.h"
#include "protocal.h"

#include "../components/camera/Command.h"

#include "time.h"
#include "sys/time.h"

static const char* TAG = "BT";

#define SPP_TAG "HMI_TESTER"
#define SPP_SERVER_NAME "HMI_BT"
#define EXAMPLE_DEVICE_NAME "ESP_HMI_HOST"
#define SPP_DATA_LEN 100

#define SPP_PACK_MAX_SIZE 496
static int bt_push_data(uint8_t *pdata, int len);

uint8_t bt_recv_buff[1024];
uint16_t bt_recv_read_len;
uint16_t bt_recv_head;
uint16_t bt_recv_tail;
int spp_file_handler = -1;
bool ssp_send_pack_complete = false;
static uint8_t spp_data[SPP_DATA_LEN];
static xQueueHandle process_data_push_queue = NULL;

/**
  * @brief  Send data via bluetooth
  * @param  pData:The buffer of the data
  * @param  Len:The length of the data
  * @retval true for success, false for fail
  */
bool bt_send_pack(uint8_t *pData, int Len) {
  int cur_byte_sent;
  int byte_sent;
  int byte_to_send;
  int send_index;
  uint32_t total_send;

  ESP_LOGI(TAG, "BT Data step1");
  
  if(spp_file_handler < 0)
    return false;

  total_send = Len;
  byte_sent = 0;
  send_index = 0;

  ESP_LOGI(TAG, "BT Data step2");
  while(Len > 0) {
    if(Len >= SPP_PACK_MAX_SIZE) 
      byte_to_send = SPP_PACK_MAX_SIZE;
    else
      byte_to_send = Len;
    ESP_LOGI(TAG, "BT send data:%d", byte_to_send);
    do {
      cur_byte_sent = write (spp_file_handler, &pData[send_index], byte_to_send);
      if(cur_byte_sent == byte_to_send) {
        byte_sent += cur_byte_sent;
        break;
      }
      vTaskDelay( 20 / portTICK_RATE_MS );
    }while(1);
    
    Len = Len - byte_to_send;
    send_index = send_index + byte_to_send;
  }
  //ESP_LOGI(SPP_TAG, "Send Len = %d", byte_sent);
  return (byte_sent == total_send)?true:false;
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
  memcpy((uint8_t*)&pack_buff[2], pData, Len);
  send_len = SetupPack(pack_buff, Len + 2, send_buff);
  bt_send_pack(send_buff, send_len);
  free(pack_buff);
  free(send_buff);
}

/**
  * @brief  Bluetooth receive data process thread
  * @retval None
  */
static void bt_data_process_thread(void *arg)
{
  int len;
  static const char *RX_TASK_TAG = "BT_RX_TASK";
  uint8_t eventId;
  uint8_t opcode;
  uint8_t* process_buff = (uint8_t*) malloc(512);
  while (1) {
    xQueueReceive(process_data_push_queue, &len, 300 / portTICK_RATE_MS);
    if(Parse(bt_recv_buff, &bt_recv_head, &bt_recv_tail, sizeof(bt_recv_buff), process_buff) > 0) {
      eventId = process_buff[8];
      opcode = process_buff[9];
      bt_command_process(eventId, opcode);
    }
  }
  free(process_buff);
}

/**
  * @brief  Push datas to the ring buffer 
  * @param  pData:The Data to be pushed
  * @param  Len:The length of the data
  * @retval The amount of the data have been pushed
  */
static int bt_push_data(uint8_t *pData, int Len) {
  int i;
  int data_pushed;
  int next_head;
  
  data_pushed = 0;
  for(i=0;i<Len;i++) {
    next_head = bt_recv_head + 1 % sizeof(bt_recv_buff);
    if(next_head == bt_recv_tail)
      break;
    bt_recv_buff[bt_recv_head] = pData[i];

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

/**
  * @brief  Bluetooth receive thread
  * @param  param
  * @retval None
  */
static void spp_read_thread(void * param)
{
    int push_size;
    int size = 0;
    int fd = (int)param;
    spp_file_handler = fd;
    do {
        size = read (fd, spp_data, SPP_DATA_LEN);
        if (size == -1) {
          ESP_LOGI(TAG, "Close bt");
          break;
        }
        
        if (size == 0) {
          /*read fail due to there is no data, retry after 1s*/
          vTaskDelay(300 / portTICK_PERIOD_MS);
        }
        else if(size > 0) {
          push_size = bt_push_data(spp_data, size);
          xQueueSend(process_data_push_queue, (void*)&push_size, 10 / portTICK_RATE_MS);
        }
    } while (1);
    vTaskDelete(NULL);
}


static void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    switch (event) {
    case ESP_SPP_INIT_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_INIT_EVT");
        esp_bt_dev_set_device_name(EXAMPLE_DEVICE_NAME);
        esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
        esp_spp_start_srv(ESP_SPP_SEC_AUTHENTICATE,ESP_SPP_ROLE_SLAVE, 0, SPP_SERVER_NAME);
        break;
    case ESP_SPP_DISCOVERY_COMP_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_DISCOVERY_COMP_EVT");
        break;
    case ESP_SPP_OPEN_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_OPEN_EVT");
        break;
    case ESP_SPP_CLOSE_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CLOSE_EVT");
        spp_file_handler = -1;
        break;
    case ESP_SPP_START_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_START_EVT");
        break;
    case ESP_SPP_CL_INIT_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CL_INIT_EVT");
        break;
    case ESP_SPP_DATA_IND_EVT:
        break;
    case ESP_SPP_CONG_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CONG_EVT");
        break;
    case ESP_SPP_WRITE_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_WRITE_EVT");
        break;
    case ESP_SPP_SRV_OPEN_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_SRV_OPEN_EVT");
        xTaskCreate(spp_read_thread, "bt receive thread", 8192, param->srv_open.fd, configMAX_PRIORITIES, NULL);
        break;
    default:
        break;
    }
}

/**
  * @brief  Initialize the bluetooth and data process thread
  * @retval None
  */
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

  esp_spp_vfs_register();
  
  if ((ret = esp_spp_init(ESP_SPP_MODE_VFS)) != ESP_OK) {
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

  process_data_push_queue = xQueueCreate(1, sizeof(int));
  xTaskCreate(bt_data_process_thread, "bt_data_process", 8192, NULL, configMAX_PRIORITIES, NULL);
}
