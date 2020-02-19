#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "Command.h"
#include "Cmd_19.h"
#include "../../main/protocal.h"

/**
  * @brief  Cmd1900 process
  * @retval None
  */
void Cmd_1900_Process(void *pPara) {
    
}

/**
  * @brief  Cmd1900 reack
  * @retval None
  */
void Cmd_1900_Reack(void *pPara) {
  bt_send_protocal_data(S_REPORT_CAMERA_LIGHT_ACK, 0, NULL, 0);
}

