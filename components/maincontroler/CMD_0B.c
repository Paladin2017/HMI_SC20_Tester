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
void cmd_0B01_process(void *pPara) {
}

/*  @brief  Specific command reack
 *  @param  pPara:The pointer to the private process buffer
 *  @retval None
 */
void cmd_0B01_reack(void *pPara) {
  GeneralReack(EID_MOVEMENT_RESP, 0x01, 0);
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
  GeneralReack(EID_MOVEMENT_RESP, 0x02, 0);
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
  GeneralReack(EID_MOVEMENT_RESP, 0x03, 0);
}

/*  @brief  Command process
 *  @param  pPara:The pointer to the private process buffer
 *  @retval None
 */ 
void cmd_0B04_process(void *pPara) {
}

/*  @brief  Specific command reack
 *  @param  pPara:The pointer to the private process buffer
 *  @retval None
 */
void cmd_0B04_reack(void *pPara) {
  GeneralReack(EID_MOVEMENT_RESP, 0x04, 0);
}


