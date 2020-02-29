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
#include "uart.h"
#include "bluetooth.h"
#include "DataProcess.h"
#include "../components/maincontroler/Command.h"
#include "../components/camera/Command.h"

void app_main(void)
{
  bt_command_init();
  hmi_command_init();
  data_process_init();
  uart_init();
  bluetooth_init();
}
