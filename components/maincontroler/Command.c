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
#include "../../main/protocal.h"
#include "../../main/uart.h"
#include "Command.h"
#include "CMD_01.h"
#include "CMD_07.h"
#include "CMD_09.h"
#include "CMD_0B.h"
#include "CMD_0D.h"
#include "CMD_A9.h"


static const char* TAG = "Main Controler";

static uint8_t command_table_count;
static uint8_t process_buff[1024];

static const strHMICommand hmi_command_table[] = {
  {EID_GCODE_REQ,       0x00, cmd_0100_process, cmd_0100_reack},
  {EID_STATUS_REQ,      0x01, cmd_0701_process, cmd_0701_reack},
  {EID_STATUS_REQ,      0x0E, cmd_070E_process, cmd_070E_reack},
  {EID_MOVEMENT_REQ,    0x01, cmd_0B01_process, cmd_0B01_reack},
  {EID_MOVEMENT_REQ,    0x02, cmd_0B02_process, cmd_0B02_reack},
  {EID_MOVEMENT_REQ,    0x03, cmd_0B03_process, cmd_0B03_reack},
  {EID_MOVEMENT_REQ,    0x04, cmd_0B04_process, cmd_0B04_reack},
  {EID_SETTING_REQ,     0x0a, cmd_090A_process, cmd_090A_reack},
  {EID_SETTING_REQ,     0x0b, cmd_090B_process, cmd_090B_reack},
  {EID_SETTING_REQ,     0x0c, cmd_090C_process, cmd_090C_reack},
  {EID_SETTING_REQ,     0x0d, cmd_090D_process, cmd_090D_reack},
  {EID_SETTING_REQ,     0x14, cmd_0914_process, cmd_0914_reack},
  {EID_LAS_CAM_OP_REQ,  0x07, cmd_0D07_process, cmd_0D07_reack},
  {EID_UPGRADE_REQ,     0x00, cmd_A900_process, cmd_A900_reack},
  {EID_UPGRADE_REQ,     0x01, cmd_A901_process, cmd_A901_reack},
  {EID_UPGRADE_REQ,     0x02, cmd_A902_process, cmd_A902_reack},
  {EID_UPGRADE_REQ,     0x03, cmd_A903_process, cmd_A903_reack}
};

void GeneralReack(uint8_t EventID, uint8_t Opcode, uint8_t Result) {
  uint16_t i = 0;
  int send_len;
  uint8_t* tmp_buff = (uint8_t*) malloc(32);
  uint8_t* pack_buff = (uint8_t*) malloc(128);

  vTaskDelay( 200 / portTICK_RATE_MS );

  // EventID
  tmp_buff[i++] = EventID;

  // operation code
  tmp_buff[i++] = Opcode;

  // Result
  tmp_buff[i++] = Result;

  send_len = SetupPack2(tmp_buff, i, pack_buff);

  uart_send_pack(pack_buff, send_len);
  free(pack_buff);
  free(tmp_buff);
}

void hmi_command_init() {
  command_table_count = sizeof(hmi_command_table) / sizeof(hmi_command_table[0]);
}

void hmi_command_process(uint8_t EventID, uint8_t Opcode) {
  int i;
  if((EventID != 0x07) && (Opcode != 0x01))
    ESP_LOGI(TAG, "Event id:%02X, Opcode:%02X", EventID, Opcode);
  for(i=0;i<command_table_count;i++) {
    if((hmi_command_table[i].EventID == EventID) && (hmi_command_table[i].EventOpcode == Opcode)) {
      hmi_command_table[i].pProcessFun((void*)process_buff);
      if(hmi_command_table[i].pReackFun != NULL)
        hmi_command_table[i].pReackFun((void*)process_buff);
      break;
    }
  }
}
