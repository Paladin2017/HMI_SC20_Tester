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
#include "bluetooth.h"
#include "uart.h"

#include "../components/camera/Command.h"
#include "../components/maincontroler/Command.h"

#include "time.h"
#include "sys/time.h"

static const char* TAG = "Process Data";
/**
  * @brief  Bluetooth receive data process thread
  * @retval None
  */
static void bt_data_process_thread(void *arg)
{
  int len;
  uint8_t event_Id;
  uint8_t opcode;
  uint8_t* process_buff = (uint8_t*) malloc(512);
  while (1) {
    //xQueueReceive(process_data_push_queue, &len, 300 / portTICK_RATE_MS);
    vTaskDelay( 10 / portTICK_RATE_MS );
    if(Parse(bt_recv_buff, &bt_recv_head, &bt_recv_tail, sizeof(bt_recv_buff), process_buff) > 0) {
      event_Id = process_buff[8];
      opcode = process_buff[9];
      bt_command_process(event_Id, opcode);
    }

    if(Parse(uart_recv_buff, &uart_recv_head, &uart_recv_tail, sizeof(uart_recv_buff), process_buff) > 0) {
      event_Id = process_buff[8];
      opcode = process_buff[9];
      hmi_command_process(event_Id, opcode);
    }
  }
  free(process_buff);
}

/**
  * @brief  Initialize threads
  * @retval None
  */
void data_process_init(void) {
  xTaskCreate(bt_data_process_thread, "bt_data_process", 8192 * 2, NULL, configMAX_PRIORITIES, NULL);
}