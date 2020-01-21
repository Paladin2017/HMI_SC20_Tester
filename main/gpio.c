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

static void GpioInit(gpio_num_t PortPin, int Mode);

/** @brief  Write level to gpio pin
  * @param  PortPin:Witch pin need to be write
  * @param  Level:The level to be set
  * @retval None
*/
void GpioWrite(gpio_num_t PortPin, uint8_t Level) {
  GpioInit(PortPin, GPIO_MODE_OUTPUT);
  gpio_set_level(PortPin, (uint32_t)Level);
  vTaskDelay(50 / portTICK_PERIOD_MS);
}

/** @brief  Write level to gpio pin
  * @param  PortPin:Witch pin need to be read
  * @retval The level of the pin, 0 for low, 1 for high
*/
uint8_t GpioRead(gpio_num_t PortPin) {
  int iolevel;
  GpioInit(PortPin, GPIO_MODE_INPUT);
  vTaskDelay(50 / portTICK_PERIOD_MS);
  iolevel = gpio_get_level(PortPin);
  if(iolevel == 0)
    return 0;
  else
    return 1;
}

/** @brief  Initilize the gpio pin to the specific mode, for input will enable internal pull up
  * @param  PortPin:Witch pin need to be initilized
  * @param  Mode:Mode to be set. etc GPIO_MODE_OUTPUT or GPIO_MODE_INPUT
  * @retval The level of the pin, 0 for low, 1 for high
*/
void GpioInit(gpio_num_t PortPin, int Mode) {
  gpio_config_t io_conf;

  //disable interrupt
  io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
  //bit mask of the pins that you want to set,e.g.GPIO18/19
  io_conf.pin_bit_mask = (1ULL << PortPin);

  if(Mode == GPIO_MODE_OUTPUT) {
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
  }
  else if(Mode == GPIO_MODE_INPUT) {
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 1;
    //configure GPIO with the given settings
  }
  gpio_config(&io_conf);
}

