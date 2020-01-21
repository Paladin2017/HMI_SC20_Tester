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

int SetupPack(uint8_t *pSrcData, uint16_t SrcLen, uint8_t *pDesData) {
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
	for(j=0;j<(SrcLen-1);j++) {
		tmp_u16 = ((uint16_t)pSrcData[j] << 8) | pSrcData[j+1];
		checksum = checksum + tmp_u16;
	}
	
	
	pDesData[i++] = 0xaa;
	pDesData[i++] = 0xaa;
}

int Parse(uint8_t *pSourceData, uint16_t *Head, uint16_t *Tail, uint16_t buffer_size, uint8_t *pParseData) {
	uint8_t *pData = pSourceData;
	uint16_t tmp_tail = *Tail;
	uint16_t tmp_head = *Head;
	uint16_t tmp_len = (tmp_head + buffer_size - tmp_tail) % buffer_size;
	
	if(tmp_len > 8) {
		
	}
}