#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "Command.h"
#include "Cmd_17.h"
#include "../../main/protocal.h"

/**
  * @brief  Cmd1700 process
  * @retval None
  */
void Cmd_1700_Process(void *pPara) {
    
}

/**
  * @brief  Cmd1700 reack
  * @retval None
  */
void Cmd_1700_Reack(void *pPara) {
  bt_send_protocal_data(S_SET_CAMERA_LIGHT_ACK, 0, NULL, 0);
}

