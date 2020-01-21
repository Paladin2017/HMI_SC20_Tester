#ifndef _GPIO_H_
#define _GPIO_H_

extern void GpioWrite(gpio_num_t PortPin, uint8_t Level);
extern uint8_t GpioRead(gpio_num_t PortPin);

#endif
