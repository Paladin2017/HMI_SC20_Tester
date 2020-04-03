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

/*  @brief  Request update packs
 *  @param  pack_index:The pack index
 *  @retval None
 */
void UpdateRequestPack(uint16_t pack_index) {
  uint16_t i = 0;
  uint16_t j;
  int send_len;
  esp_err_t ret;
  uint8_t* tmp_buff = (uint8_t*) malloc(32);
  uint8_t* pack_buff = (uint8_t*) malloc(128);

  tmp_buff[i++] = EID_UPGRADE_RESP;
  tmp_buff[i++] = 1;
  tmp_buff[i++] = (pack_index >> 8);
  tmp_buff[i++] = (pack_index);

  send_len = SetupPack2(tmp_buff, i, pack_buff);

  uart_send_pack(pack_buff, send_len);
  free(pack_buff);
  free(tmp_buff);
}

/*  @brief  Request firmware version
 *  @param  Version:The version in string
 *  @retval None
 */
void UpdateFirmwareVersion(char *Version) {
  uint16_t i = 0;
  uint16_t j;
  int send_len;
  esp_err_t ret;
  uint8_t* tmp_buff = (uint8_t*) malloc(64);
  uint8_t* pack_buff = (uint8_t*) malloc(128);

  tmp_buff[i++] = EID_UPGRADE_RESP;
  tmp_buff[i++] = 3;
  // igore the front 10 characters 'Snapmaker_'
  for(int j=0;j<32;j++) {
    tmp_buff[i++] = Version[j];
    if(Version[j] == 0) break;
  }

  send_len = SetupPack2(tmp_buff, i, pack_buff);

  uart_send_pack(pack_buff, send_len);
  free(pack_buff);
  free(tmp_buff);
}


/*  @brief  Command process
 *  @param  pPara:The pointer to the private process buffer
 *  @retval None
 */
void cmd_A900_process(void *pPara) {
  GeneralReack(EID_UPGRADE_RESP, 0x00, 0);
}

/*  @brief  Specific command reack
 *  @param  pPara:The pointer to the private process buffer
 *  @retval None
 */
void cmd_A900_reack(void *pPara) {
  UpdateRequestPack(9999);
}

/*  @brief  Command process
 *  @param  pPara:The pointer to the private process buffer
 *  @retval None
 */
void cmd_A901_process(void *pPara) {
}

/*  @brief  Specific command reack
 *  @param  pPara:The pointer to the private process buffer
 *  @retval None
 */
void cmd_A901_reack(void *pPara) {
  UpdateRequestPack(9999);
}

/*  @brief  Command process
 *  @param  pPara:The pointer to the private process buffer
 *  @retval None
 */
void cmd_A902_process(void *pPara) {
}

/*  @brief  Specific command reack
 *  @param  pPara:The pointer to the private process buffer
 *  @retval None
 */
void cmd_A902_reack(void *pPara) {
  GeneralReack(EID_UPGRADE_RESP, 0x02, 0);
}

/*  @brief  Command process
 *  @param  pPara:The pointer to the private process buffer
 *  @retval None
 */
void cmd_A903_process(void *pPara) {
}

/*  @brief  Specific command reack
 *  @param  pPara:The pointer to the private process buffer
 *  @retval None
 */
void cmd_A903_reack(void *pPara) {
  UpdateFirmwareVersion("V0.0.0.1");
}


