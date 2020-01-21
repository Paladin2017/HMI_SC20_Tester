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
#include "gpio.h"
#include "protocal.h"

static int push_data(uint8_t *pdata, int len);


#define THIS_UART	UART_NUM_2

#define CMD_M_READ  1
#define CMD_M_WRITE 3
#define CMD_M_ADC_INIT 7
#define CMD_M_ADC_READ 9

#define CMD_S_READ  2
#define CMD_S_WRITE 4
#define CMD_S_ADC_INIT  8
#define CMD_S_ADC_READ  10

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

void send_pack(uint8_t *pData, uint8_t len) {
  uart_write_bytes(THIS_UART, (char*)pData, len);
}

static void flush_buffer() {
  recv_head = recv_tail = 0;
}

static void send_gpio_write_reack(uint8_t pin) {
  uint8_t buff[8];
  uint8_t pack_buff[32];
  int send_len;
  buff[0] = CMD_S_WRITE;
  buff[1] = 0;
  buff[2] = pin;
  send_len = SetupPack(buff, 3, pack_buff);
  send_pack(pack_buff, send_len);
}

static void send_gpio_read_reack(uint8_t pin, uint8_t level) {
  uint8_t buff[8];
  uint8_t pack_buff[32];
  int send_len;
  buff[0] = CMD_S_READ;
  buff[1] = 0;
  buff[2] = pin;
  buff[3] = level;
  send_len = SetupPack(buff, 4, pack_buff);
  send_pack(pack_buff, send_len);
}

static void rx_task(void *arg)
{
    static const char *RX_TASK_TAG = "RX_TASK";
    uint8_t level;
    uint8_t* data = (uint8_t*) malloc(RX_BUF_SIZE+1);
    uint8_t* process_buff = (uint8_t*) malloc(512);
    while (1) {
        const int rxBytes = uart_read_bytes(THIS_UART, data, RX_BUF_SIZE, 50 / portTICK_RATE_MS);
        if (rxBytes > 0) {
          if(push_data(data, rxBytes) != rxBytes) {
            flush_buffer();
            continue;
          }
        }
        if(Parse(recv_buff, &recv_head, &recv_tail, sizeof(recv_buff), process_buff) > 0) {
          switch(process_buff[8]) {
            case CMD_M_READ:
              GpioWrite(process_buff[9], process_buff[10]);
              send_gpio_write_reack(process_buff[9]);
              break;

            case CMD_M_WRITE:
              level = GpioRead(process_buff[9]);
              send_gpio_read_reack(process_buff[9], level);
              break;

            case CMD_M_ADC_INIT:
              break;

            case CMD_M_ADC_READ:
              break;
          }
        }
    }
    free(data);
}

static int push_data(uint8_t *pdata, int len) {
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
    xTaskCreate(rx_task, "uart_rx_task", 8192*2, NULL, configMAX_PRIORITIES, NULL);
}
