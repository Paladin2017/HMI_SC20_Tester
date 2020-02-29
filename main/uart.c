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

#include "../components/maincontroler/Command.h"

static int uart_push_data(uint8_t *pdata, int len);

static const char* TAG = "Uart2";

#define THIS_UART	UART_NUM_2

static const int RX_BUF_SIZE = 512;

#define TXD_PIN (GPIO_NUM_17)
#define RXD_PIN (GPIO_NUM_16)

uint8_t uart_recv_buff[2048];
uint16_t recv_read_len;
uint16_t uart_recv_head;
uint16_t uart_recv_tail;

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
  uart_recv_head = uart_recv_tail = 0;
}

static void uart_rx_task(void *arg)
{
    static const char *RX_TASK_TAG = "RX_TASK";
    uint8_t* data = (uint8_t*) malloc(512+1);
    uint8_t* process_buff = (uint8_t*) malloc(512);
    char *string = (char*) malloc(128);
    uint8_t event_id;
    uint8_t opcode;
    uart_flush_buffer();
    while (1) {
      int rxBytes = uart_read_bytes(THIS_UART, data, RX_BUF_SIZE, 50 / portTICK_RATE_MS);
      if (rxBytes > 0) {
        //ESP_LOGI(TAG, "Data Len:%02X", rxBytes);
        string[0] = 0;
        if(uart_push_data(data, rxBytes) != rxBytes) {
          ESP_LOGI(TAG, "Flush");
          uart_flush_buffer();
          continue;
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
    next_head = (uart_recv_head + 1) % sizeof(uart_recv_buff);
    if(next_head == uart_recv_tail)
      break;
    uart_recv_buff[uart_recv_head] = pdata[i];
    uart_recv_head = next_head;
    data_pushed++;
  }
  return data_pushed;
}

void uart_init(void)
{
    init();
    xTaskCreate(uart_rx_task, "uart_rx_task", 8192*2, NULL, configMAX_PRIORITIES, NULL);
}
