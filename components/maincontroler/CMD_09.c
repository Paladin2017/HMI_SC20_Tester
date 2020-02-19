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
void cmd_090A_process(void *pPara) {
}

/*  @brief  Specific command reack
 *  @param  pPara:The pointer to the private process buffer
 *  @retval None
 */
void cmd_090A_reack(void *pPara) {
  GeneralReack(0x09, 0x0A, 0);
}

/*  @brief  Command process
 *  @param  pPara:The pointer to the private process buffer
 *  @retval None
 */ 
void cmd_090B_process(void *pPara) {
}

/*  @brief  Specific command reack
 *  @param  pPara:The pointer to the private process buffer
 *  @retval None
 */
void cmd_090B_reack(void *pPara) {
  GeneralReack(0x09, 0x0B, 0);
}

/*  @brief  Command process
 *  @param  pPara:The pointer to the private process buffer
 *  @retval None
 */ 
void cmd_090C_process(void *pPara) {
}

/*  @brief  Specific command reack
 *  @param  pPara:The pointer to the private process buffer
 *  @retval None
 */
void cmd_090C_reack(void *pPara) {
  GeneralReack(0x09, 0x0C, 0);
}

/*  @brief  Command process
 *  @param  pPara:The pointer to the private process buffer
 *  @retval None
 */ 
void cmd_090D_process(void *pPara) {
}

/*  @brief  Specific command reack
 *  @param  pPara:The pointer to the private process buffer
 *  @retval None
 */
void cmd_090D_reack(void *pPara) {
  GeneralReack(0x09, 0x0D, 0);
}

/*  @brief  Command process
 *  @param  pPara:The pointer to the private process buffer
 *  @retval None
 */ 
void cmd_0914_process(void *pPara) {
}

/*  @brief  Specific command reack
 *  @param  pPara:The pointer to the private process buffer
 *  @retval None
 */
void cmd_0914_reack(void *pPara) {
  int32_t int32Value;
  uint32_t u32Value;
  uint16_t i;
  int send_len;
  uint8_t* tmp_buff = (uint8_t*) malloc(128);
  uint8_t* pack_buff = (uint8_t*) malloc(128);
  
  i = 0;

  //EventID
  tmp_buff[i++] = EID_SETTING_RESP;

  //获取尺寸
  tmp_buff[i++] = 20;
  tmp_buff[i++] = 0;

  //Machine size type
  tmp_buff[i++] = 3;

  //Size
  u32Value = (uint32_t) (350 * 1000);
  BITS32_TO_BYTES(u32Value, tmp_buff, i);
  u32Value = (uint32_t) (350 * 1000);
  BITS32_TO_BYTES(u32Value, tmp_buff, i);
  u32Value = (uint32_t) (350 * 1000);
  BITS32_TO_BYTES(u32Value, tmp_buff, i);

  //Home Dir
  int32Value = (int32_t) (-1);
  BITS32_TO_BYTES(int32Value, tmp_buff, i);
  int32Value = (int32_t) (1);
  BITS32_TO_BYTES(int32Value, tmp_buff, i);
  int32Value = (int32_t) (1);
  BITS32_TO_BYTES(int32Value, tmp_buff, i);

  //Dir
  int32Value = -1 == true?(int32_t)1:(int32_t)-1;
  BITS32_TO_BYTES(int32Value, tmp_buff, i);
  int32Value = 1 == true?(int32_t)1:(int32_t)-1;
  BITS32_TO_BYTES(int32Value, tmp_buff, i);
  int32Value = 1 == true?(int32_t)1:(int32_t)-1;
  BITS32_TO_BYTES(int32Value, tmp_buff, i);

  //Offset
  int32Value = (int32_t) (-17 * 1000.0f);
  BITS32_TO_BYTES(int32Value, tmp_buff, i);
  int32Value = (int32_t) (0 *1000.0f);
  BITS32_TO_BYTES(int32Value, tmp_buff, i);
  int32Value = (int32_t) (0 *1000.0f);
  BITS32_TO_BYTES(int32Value, tmp_buff, i);

  send_len = SetupPack(tmp_buff, i, pack_buff);

  uart_send_pack(pack_buff, send_len);

  free(pack_buff);
  free(tmp_buff);
}

