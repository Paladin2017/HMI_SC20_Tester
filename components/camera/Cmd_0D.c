#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "Command.h"
#include "Cmd_0D.h"
#include "../../main/protocal.h"
#include "Image_Data.h"


/**
  * @brief  Cmd0E process, take picture
  * @param  pData:The buffer of the data
  * @param  Len:The length of the data
  * @retval None
  */
void Cmd_0D01_Process(void *pPara) {
    uint8_t const *jpg_data_ptr;
    uint32_t data_index;
    uint32_t byte_to_send;
    uint32_t data_size;
    uint32_t max_send_len = 512;
    uint8_t* tmp_Buff = (uint8_t*) malloc(512);
    
    // Step1 Start Pack
    bt_send_protocal_data(S_CAMERA_IMG_ACK, 0, NULL, 0);

    //Step2 Data Pack
    data_index = 0;
    jpg_data_ptr = image_data;
    data_size = sizeof(image_data);
    // Copy data
    while(data_size > 0) {
        if(data_size >= max_send_len)
            byte_to_send = max_send_len;
        else
            byte_to_send = data_size;
        bt_send_protocal_data(S_CAMERA_IMG_ACK, 1, (uint8_t*)&jpg_data_ptr[data_index], byte_to_send);
        data_index += byte_to_send;
        data_size = data_size - byte_to_send;
    }
    //Step3 End Pack
    tmp_Buff[0] = S_CAMERA_IMG_ACK;
    tmp_Buff[1] = 2;
    bt_send_protocal_data(S_CAMERA_IMG_ACK, 2, NULL, 0);
}

