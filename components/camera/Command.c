/* UART asynchronous example, that uses separate RX and TX tasks

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "../../main/Typedef.h"
#include "esp_system.h"
#include "esp_log.h"
#include "string.h"
#include "Command.h"
#include "Cmd_03.h"
#include "Cmd_05.h"
#include "Cmd_0D.h"
#include "Cmd_17.h"
#include "Cmd_19.h"

static const char* TAG = "BT command";

static uint8_t command_table_count;
static uint8_t process_buff[1024];

static const strHMICommand bt_command_table[] = {
  {M_CAMERA_GET_IMG,        0x00, Cmd_0D00_Process, NULL},
  {M_CAMERA_GET_AWB,        0x00, Cmd_0300_Process, Cmd_0300_Reack},
  {M_CAMERA_SET_AWB,        0x00, Cmd_0500_Process, Cmd_0500_Reack},
  {M_SET_CAMERA_LIGHT,      0x00, Cmd_1700_Process, Cmd_1700_Reack},
  {M_REPORT_CAMERA_LIGHT,   0x00, Cmd_1900_Process, Cmd_1900_Reack},
};

void bt_command_init() {
  command_table_count = sizeof(bt_command_table) / sizeof(bt_command_table[0]);
}

void bt_command_process(uint8_t EventID, uint8_t Opcode) {
  int i;
  ESP_LOGI(TAG, "Event id:%02X, Opcode:%02X", EventID, Opcode);
  for(i=0;i<command_table_count;i++) {
    if((bt_command_table[i].EventID == EventID) && (bt_command_table[i].EventOpcode == Opcode)) {
      bt_command_table[i].pProcessFun(process_buff);
      if(bt_command_table[i].pReackFun != NULL)
        bt_command_table[i].pReackFun(process_buff);
      break;
    }
  }
}
