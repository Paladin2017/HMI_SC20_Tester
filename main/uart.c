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
#include "protocal.h"

static int push_data(uint8_t *pdata, int len);


#define THIS_UART	UART_NUM_2

static const int RX_BUF_SIZE = 512;

#define TXD_PIN (GPIO_NUM_17)
#define RXD_PIN (GPIO_NUM_16)

uint8_t recv_buff[1024];
uint16_t recv_read_len;
uint16_t recv_head;
uint16_t recv_tail;

void init(void) {
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    // We won't use a buffer for sending data.
    uart_driver_install(THIS_UART, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(THIS_UART, &uart_config);
    uart_set_pin(THIS_UART, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

void uart_send_pack(uint8_t *pData, int len) {
  if(len > 0)
    uart_write_bytes(THIS_UART, (char*)pData, len);
}

static void uart_flush_buffer() {
  recv_head = recv_tail = 0;
}

static void uart_rx_task(void *arg)
{
    static const char *RX_TASK_TAG = "RX_TASK";
    uint8_t* data = (uint8_t*) malloc(512+1);
    uint8_t* process_buff = (uint8_t*) malloc(512);
    while (1) {
        const int rxBytes = uart_read_bytes(THIS_UART, data, RX_BUF_SIZE, 50 / portTICK_RATE_MS);
        if (rxBytes > 0) {
          if(uart_push_data(data, rxBytes) != rxBytes) {
            uart_flush_buffer();
            continue;
          }
        }
        if(Parse(recv_buff, &recv_head, &recv_tail, sizeof(recv_buff), process_buff) > 0) {
          switch(process_buff[8]) {
            
          }
        }
    }
    free(process_buff);
    free(data);
}

static int uart_push_data(uint8_t *pdata, int len) {
  int i;
  int data_pushed;
  int next_head;
  
  data_pushed = 0;
  for(i=0;i<len;i++) {
    next_head = recv_head + 1 % sizeof(recv_buff);
    if(next_head == recv_tail)
      break;
    recv_buff[recv_head] = pdata[i];
    recv_head = next_head;
    data_pushed++;
  }
  return data_pushed;
}

void uart_init(void)
{
    init();
    xTaskCreate(uart_rx_task, "uart_rx_task", 8192*2, NULL, configMAX_PRIORITIES, NULL);
}
