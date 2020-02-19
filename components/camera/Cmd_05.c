#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "Command.h"
#include "Cmd_05.h"
#include "../../main/protocal.h"

/**
  * @brief  Cmd0500 process
  * @retval None
  */
void Cmd_0500_Process(void *pPara) {
    
}

/**
  * @brief  Cmd0500 reack
  * @retval None
  */
void Cmd_0500_Reack(void *pPara) {
  bt_send_protocal_data(S_CAMERA_SET_AWB_ACK, 0, NULL, 0);
}
