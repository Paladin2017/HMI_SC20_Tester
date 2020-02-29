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
void cmd_0701_process(void *pPara) {
  
}

/*  @brief  Specific command reack
 *  @param  pPara:The pointer to the private process buffer
 *  @retval None
 */
void cmd_0701_reack(void *pPara) {
  float fValue;
  int32_t tmp;
  uint32_t LaserPower = 1;
  uint16_t i = 0;
  int16_t T0, TB, T0S, TBS;
  uint16_t RPM;
  int send_len;
  uint8_t* status_buff = (uint8_t*) malloc(128);
  uint8_t* pack_buff = (uint8_t*) malloc(128);

  // EventID
  status_buff[i++] = EID_STATUS_RESP;

  // operation code
  status_buff[i++] = 0x01;

  // current logical position
  tmp = (int32_t) (0);
  BITS32_TO_BYTES(tmp, status_buff, i);
  tmp = (int32_t) (0);
  BITS32_TO_BYTES(tmp, status_buff, i);
  tmp = (int32_t) (0);
  BITS32_TO_BYTES(tmp, status_buff, i);
  tmp = (int32_t) (0);
  BITS32_TO_BYTES(tmp, status_buff, i);

  // temperature
  //if(current_temperature[0] >= 0)
  T0 = 25;
  T0S = 0;

  //if(current_temperature_bed >= 0)
  TB = 25;
  TBS = 0;
  status_buff[i++] = (uint8_t) (TB >> 8);
  status_buff[i++] = (uint8_t) (TB);
  status_buff[i++] = (uint8_t) (TBS >> 8);
  status_buff[i++] = (uint8_t) (TBS);
  status_buff[i++] = (uint8_t) (T0 >> 8);
  status_buff[i++] = (uint8_t) (T0);
  status_buff[i++] = (uint8_t) (T0S >> 8);
  status_buff[i++] = (uint8_t) (T0S);

  //FeedRate
  fValue = 0;
  status_buff[i++] = (uint8_t)(((uint16_t)fValue)>>8);
  status_buff[i++] = (uint8_t)((uint16_t)fValue);

  // LaserPower
  BITS32_TO_BYTES(LaserPower, status_buff, i);

  // RPM of CNC
  RPM = 0;
  status_buff[i++] = 0;
  status_buff[i++] = 0;
  status_buff[i++] = (uint8_t) (RPM >> 8);
  status_buff[i++] = (uint8_t) (RPM);

  // current status
  status_buff[i++] = 0;

  // add-on status
  status_buff[i++] = 0;

  // executor type
  //status_buff[i++] = 0x96;
  status_buff[i++] = 0x03;

  send_len = SetupPack2(status_buff, i, pack_buff);

  uart_send_pack(pack_buff, send_len);

  free(pack_buff);
  free(status_buff);
}

/*  @brief  Command process
 *  @param  pPara:The pointer to the private process buffer
 *  @retval None
 */ 
void cmd_0702_process(void *pPara) {
  
}

/*  @brief  Specific command reack
 *  @param  pPara:The pointer to the private process buffer
 *  @retval None
 */
void cmd_0702_reack(void *pPara) {
  int32_t tmp;
  uint16_t i = 0;
  int send_len;
  uint8_t* status_buff = (uint8_t*) malloc(128);
  uint8_t* pack_buff = (uint8_t*) malloc(128);

  // EventID
  status_buff[i++] = EID_STATUS_RESP;

  // operation code
  status_buff[i++] = 0x02;

  // Falut flag
  tmp = (int32_t) (0);
  BITS32_TO_BYTES(tmp, status_buff, i);

  // Data source
  status_buff[i++] = 3;

  // Data position
  tmp = (int32_t) (0);
  BITS32_TO_BYTES(tmp, status_buff, i);

  send_len = SetupPack2(status_buff, i, pack_buff);

  uart_send_pack(pack_buff, send_len);

  free(pack_buff);
  free(status_buff);
}

/*  @brief  Command process
 *  @param  pPara:The pointer to the private process buffer
 *  @retval None
 */ 
void cmd_070E_process(void *pPara) {
  
}

/*  @brief  Specific command reack
 *  @param  pPara:The pointer to the private process buffer
 *  @retval None
 */
void cmd_070E_reack(void *pPara) {
  int32_t tmp;
  uint16_t i = 0;
  int send_len;
  uint8_t* temp_buff = (uint8_t*) malloc(128);
  uint8_t* pack_buff = (uint8_t*) malloc(128);

  // EventID
  temp_buff[i++] = EID_STATUS_RESP;

  // operation code
  temp_buff[i++] = 0x0E;

  temp_buff[i++] = 0;

  // number
  temp_buff[i++] = 0;
  // state
  temp_buff[i++] = 0;

  BITS32_TO_BYTES(0, temp_buff, i);
  BITS32_TO_BYTES(0, temp_buff, i);
  BITS32_TO_BYTES(0, temp_buff, i);

  send_len = SetupPack2(temp_buff, i, pack_buff);

  uart_send_pack(pack_buff, send_len);

  free(pack_buff);
  free(temp_buff);
}

