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
#include "esp_bt_device.h"
#include "string.h"
#include "Command.h"
#include "../../main/protocal.h"
#include "../../main/uart.h"

/*  @brief  Command process
 *  @param  pPara:The pointer to the private process buffer
 *  @retval None
 */ 
void cmd_0D07_process(void *pPara) {
  
}

/*  @brief  Specific command reack
 *  @param  pPara:The pointer to the private process buffer
 *  @retval None
 */
void cmd_0D07_reack(void *pPara) {
  uint16_t i = 0;
  uint16_t j;
  int send_len;
  esp_err_t ret;
  uint8_t* tmp_buff = (uint8_t*) malloc(32);
  uint8_t* mac = (uint8_t*) malloc(32);
  uint8_t* pack_buff = (uint8_t*) malloc(128);

  for(j=0;j<10;j++) {
    ret = esp_read_mac(mac, ESP_MAC_BT);
    if(ret == ESP_OK)
      break;
  }
  
  // EventID
  tmp_buff[i++] = EID_LAS_CAM_OP_RESP;

  // operation code
  tmp_buff[i++] = 0x07;

  // Result
  if(ret != ESP_OK)
    tmp_buff[i++] = 2;
  else
    tmp_buff[i++] = 0;
  // Mac
  
  for (int j = 0; j < 6; j++) {
    tmp_buff[i++] = mac[j];
  }

  send_len = SetupPack(tmp_buff, i, pack_buff);

  uart_send_pack(pack_buff, send_len);
  free(pack_buff);
  free(tmp_buff);
}

