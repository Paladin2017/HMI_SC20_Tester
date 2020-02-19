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
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"

static const char* TAG = "Protocal";

/** @brief  Setup a pack of data use the protocal
  * @param  pSourceData:The buffer of the source buffer
  * @param  SrcLen:The length of the source data
  * @param  pDesData:The buffer of the destination buffer
  * @retval The Lenght of the pack
*/
int SetupPack(uint8_t *pSourceData, uint16_t SrcLen, uint8_t *pDesData) {
	int i = 0;
	int j;
	uint32_t checksum;
	uint16_t tmp_u16;
	
	pDesData[i++] = 0xaa;
	pDesData[i++] = 0x55;
	pDesData[i++] = (uint8_t)(SrcLen >> 8);
	pDesData[i++] = (uint8_t)(SrcLen);;
	pDesData[i++] = 0x00;
	pDesData[i++] = pDesData[2] ^ pDesData[3];
	
	// Calculator check sum
	checksum = 0;
	for(j=0;j<(SrcLen-1);j=j+2) {
		tmp_u16 = ((uint16_t)pSourceData[j] << 8) | pSourceData[j+1];
		checksum = checksum + tmp_u16;
	}
  if((SrcLen % 2) != 0)
    checksum += pSourceData[SrcLen - 1];
  while(checksum > 0xffff)
    checksum = ((checksum >> 16) & 0xffff) + (checksum & 0xffff);
  checksum = (~checksum) & 0xffff;
	pDesData[i++] = (checksum >> 8) & 0xff;
	pDesData[i++] = checksum & 0xff;

  // Copy data
  for(j=0;j<SrcLen;j++)
    pDesData[i++] = pSourceData[j];

  return i;
}

/** @brief  Parse a pack of data use the protocal, and throw out any error datas
  * @param  Head:The read head of the income ring buffer
  * @param  Tail:The read tail of the income ring buffer
  * @param  buffer_size:The buffer size of the income ring buffer
  * @param  pParseData:The buffer where the parsed data to be stored
  * @retval The Lenght of the parsed data within the protocal head(8)
*/
int Parse(uint8_t *pSourceData, uint16_t *Head, uint16_t *Tail, uint16_t buffer_size, uint8_t *pParseData) {
  int i;
  int tmp_tail_backup;
  uint32_t calculate_checksum;
  uint32_t src_checksum;
	uint8_t *pData = pSourceData;
	uint16_t tmp_tail = *Tail;
	uint16_t tmp_head = *Head;
  uint8_t cmd_len_low;
  uint8_t cmd_len_high;
  uint16_t cmd_len;
  uint16_t tmp_u16;
	uint16_t tmp_len = (tmp_head + buffer_size - tmp_tail) % buffer_size;
	
	while(tmp_len > 8) {
		if(pData[tmp_tail] != 0xaa) {
      ESP_LOGI(TAG, "Data:%02X", pData[tmp_tail]);
      tmp_tail = (tmp_tail + 1) % buffer_size;
      tmp_len--;
      *Tail = tmp_tail;
      continue;
		}

    if(pData[(tmp_tail + 1) % buffer_size] != 0x55) {
      tmp_tail = (tmp_tail + 2) % buffer_size;
      tmp_len = tmp_len - 2;
      *Tail = tmp_tail;
      continue;
		}
    // Get command length bytes
    cmd_len_high = pData[(tmp_tail + 2) % buffer_size];
    cmd_len_low = pData[(tmp_tail + 3) % buffer_size];

    // Verify command length
    if((cmd_len_low ^ cmd_len_high) != pData[(tmp_tail + 5) % buffer_size]) {
      tmp_tail = (tmp_tail + 2) % buffer_size;
      tmp_len = tmp_len - 2;
      *Tail = tmp_tail;
      continue;
    }

    // Verify if enough datas are there can be fetch
    cmd_len = ((uint16_t)cmd_len_high << 8) | cmd_len_low;
    if(cmd_len > 256) {
      tmp_tail = (tmp_tail + 2) % buffer_size;
      tmp_len = tmp_len - 2;
      *Tail = tmp_tail;
      continue;
    }
    if((cmd_len + 8) > tmp_len)
      return;

    // Copy datas to the destination buffer
    tmp_tail_backup = tmp_tail;
    for(i=0;i<cmd_len + 8;i++) {
      pParseData[i] = pData[tmp_tail_backup];
      tmp_tail_backup = (tmp_tail_backup + 1) % buffer_size;
    }

    // Verify datas
    calculate_checksum = 0;
    for(i=8;i<(cmd_len + 8 - 1);i=i+2) {
      tmp_u16 = ((uint16_t)pParseData[i] << 8) | pParseData[i + 1];
		  calculate_checksum = calculate_checksum + tmp_u16;
    }
    if((cmd_len % 2) != 0)
      calculate_checksum = calculate_checksum + pParseData[cmd_len + 8 - 1];
    while(calculate_checksum > 0xffff)
      calculate_checksum = ((calculate_checksum >> 16) & 0xffff) + (calculate_checksum & 0xffff);
    calculate_checksum = (~calculate_checksum) & 0xffff;
    src_checksum = ((uint16_t)pParseData[6] << 8) | pParseData[7];
    if(src_checksum != calculate_checksum) {
      tmp_tail = (tmp_tail + 2) % buffer_size;
      tmp_len = tmp_len - 2;
      *Tail = tmp_tail;
      ESP_LOGI(TAG, "Verify fail");
      continue;
    }

    // Update tails
    *Tail = tmp_tail_backup;

    return (cmd_len + 8);
    
	}
  return 0;
}