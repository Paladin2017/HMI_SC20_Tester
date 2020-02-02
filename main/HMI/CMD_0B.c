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


#define BITS32_TO_BYTES(u32bit, buff, index) do { \
    buff[index++] = (uint8_t)(u32bit >> 24); \
    buff[index++] = (uint8_t)(u32bit >> 16); \
    buff[index++] = (uint8_t)(u32bit >> 8); \
    buff[index++] = (uint8_t)(u32bit); \
    }while(0)

/*  @brief  Command process
 *  @param  pPara:The pointer to the private process buffer
 *  @retval None
 */ 
void cmd_0B01_process(void *pPara) {
}

/*  @brief  Specific command reack
 *  @param  pPara:The pointer to the private process buffer
 *  @retval None
 */
void cmd_0B01_reack(void *pPara) {
  float fValue;
  int32_t tmp;
  uint16_t i = 0;
  int send_len;
  uint8_t* status_buff = (uint8_t*) malloc(128);
  uint8_t* pack_buff = (uint8_t*) malloc(128);

  // EventID
  status_buff[i++] = EID_MOVEMENT_RESP;

  // operation code
  status_buff[i++] = 0x01;

  // Result
  status_buff[i++] = 0;

  send_len = SetupPack(status_buff, i, pack_buff);

  uart_send_pack(pack_buff, send_len);
  free(pack_buff);
  free(status_buff);
}

/*  @brief  Command process
 *  @param  pPara:The pointer to the private process buffer
 *  @retval None
 */ 
void cmd_0B02_process(void *pPara) {
}

/*  @brief  Specific command reack
 *  @param  pPara:The pointer to the private process buffer
 *  @retval None
 */
void cmd_0B02_reack(void *pPara) {
  float fValue;
  int32_t tmp;
  uint16_t i = 0;
  int send_len;
  uint8_t* status_buff = (uint8_t*) malloc(128);
  uint8_t* pack_buff = (uint8_t*) malloc(128);

  // EventID
  status_buff[i++] = EID_MOVEMENT_RESP;

  // operation code
  status_buff[i++] = 0x02;

  // Result
  status_buff[i++] = 0;

  send_len = SetupPack(status_buff, i, pack_buff);

  uart_send_pack(pack_buff, send_len);
  free(pack_buff);
  free(status_buff);
}

/*  @brief  Command process
 *  @param  pPara:The pointer to the private process buffer
 *  @retval None
 */ 
void cmd_0B03_process(void *pPara) {
}

/*  @brief  Specific command reack
 *  @param  pPara:The pointer to the private process buffer
 *  @retval None
 */
void cmd_0B03_reack(void *pPara) {
  float fValue;
  int32_t tmp;
  uint16_t i = 0;
  int send_len;
  uint8_t* status_buff = (uint8_t*) malloc(128);
  uint8_t* pack_buff = (uint8_t*) malloc(128);

  // EventID
  status_buff[i++] = EID_MOVEMENT_RESP;

  // operation code
  status_buff[i++] = 0x03;

  // Result
  status_buff[i++] = 0;

  send_len = SetupPack(status_buff, i, pack_buff);

  uart_send_pack(pack_buff, send_len);
  free(pack_buff);
  free(status_buff);
}

