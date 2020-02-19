#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "Command.h"
#include "Cmd_03.h"
#include "../../main/protocal.h"

/**
  * @brief  Cmd0300 process
  * @retval None
  */
void Cmd_0300_Process(void *pPara) {
    
}

/**
  * @brief  Cmd0300 reack
  * @retval None
  */
void Cmd_0300_Reack(void *pPara) {
  bt_send_protocal_data(S_CAMERA_GET_AWB_ACK, 0, NULL, 0);
}

