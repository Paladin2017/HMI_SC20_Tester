#include "Cmd_0E.h"

/**
  * @brief  Cmd0E process, take picture
  * @param  pData:The buffer of the data
  * @param  Len:The length of the data
  * @retval None
  */
void Cmd_0E(void *pPara) {
    uint8_t *jpg_data_ptr;
    uint32_t data_index;
    uint32_t byte_to_send;
    uint32_t data_size;
    uint32_t max_send_len = SPP_PACK_MAX_SIZE - sizeof(SendPackStruct_S);
    uint8_t* tmp_Buff = (uint8_t*) malloc(512);
    uint8_t* send_Buff = (uint8_t*) malloc(512);
    int send_len;
    
    ESP_LOGI(TAG, "report img start");
    // Step1 Start Pack
    bt_send_pack(S_CAMERA_IMG_ACK, 0, NULL, 0);

    ESP_LOGI(TAG, "report img data");
    //Step2 Data Pack
    data_index = 0;
    // Copy data
    while(data_size > 0) {
        if(data_size >= max_send_len)
            byte_to_send = max_send_len;
        else
            byte_to_send = data_size;
        tmp_Buff[0] = S_CAMERA_IMG_ACK;
        tmp_Buff[1] = 1;
        for(int i=0;i<byte_to_send;i++)
          tmp_Buff[i+2] = jpg_data_ptr[data_index + i];
        //bt_sendprotocoldata(S_CAMERA_IMG_ACK, 1, &jpg_data_ptr[data_index], byte_to_send);
        send_len = SetupPack(tmp_Buff, data_index, send_Buff);
        data_index += byte_to_send;
        data_size = data_size - byte_to_send;
        bt_send_pack(send_Buff, send_len);
    }
    ESP_LOGI(TAG, "report img end");
    //Step3 End Pack
    tmp_Buff[0] = S_CAMERA_IMG_ACK;
    tmp_Buff[1] = 2;
    bt_sendprotocoldata(S_CAMERA_IMG_ACK, 2, NULL, 0);
}

