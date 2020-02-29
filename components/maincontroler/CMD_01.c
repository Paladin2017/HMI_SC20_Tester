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
#include "../../main/protocal.h"
#include "../../main/uart.h"

/*  @brief  Command process
 *  @param  pPara:The pointer to the private process buffer
 *  @retval None
 */ 
void cmd_0100_process(void *pPara) {
}

/*  @brief  Specific command reack
 *  @param  pPara:The pointer to the private process buffer
 *  @retval None
 */
void cmd_0100_reack(void *pPara) {
  int send_len;
  uint8_t* tmp_buff = (uint8_t*) malloc(128);
  uint8_t* pack_buff = (uint8_t*) malloc(128);
  uint16_t i = 0;
  uint16_t j;
  char *string_gcode =  (char*) malloc(32);

  // EventID
  tmp_buff[i++] = EID_GCODE_RESP;

  // "ok"
  strcpy(string_gcode, "ok\r\n");
  for(j=0;j<strlen(string_gcode);j++)
  {
    tmp_buff[i++] = string_gcode[j];
    if(string_gcode[j] == 0)
      break;
  }
  
  send_len = SetupPack2(tmp_buff, i, pack_buff);

  uart_send_pack(pack_buff, send_len);

  free(pack_buff);
  free(tmp_buff);

}

