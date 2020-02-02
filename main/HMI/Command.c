/* UART asynchronous example, that uses separate RX and TX tasks

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "string.h"
#include "Command.h"

static uint8_t command_table_count;
static uint8_t process_buff[1024];

static const strHMICommand hmi_command_table[] = {
  {EID_STATUS_REQ,      0x01, cmd_0701_process, cmd_0701_reack},
  {EID_MOVEMENT_REQ,    0x01, cmd_0B01_process, cmd_0B01_reack},
  {EID_LAS_CAM_OP_REQ,  0x07, cmd_0D07_process, cmd_0D07_reack}
};

void command_init() {
  command_table_count = sizeof(hmi_command_table) / sizeof(hmi_command_table[0]);
}

void command_process(uint8_t EventID, uint8_t Opcode) {
  int i;
  for(i=0;i<command_table_count;i++) {
    if((hmi_command_table[i].EventID == EventID) && (hmi_command_table[i].EventOpcode == Opcode)) {
      hmi_command_table[i].pProcessFun(process_buff);
      hmi_command_table[i].pReackFun(process_buff);
    }
  }
}
