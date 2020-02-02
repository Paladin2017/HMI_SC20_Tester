#ifndef _UART_H_
#define _UART_H_

extern void uart_init(void);
extern void uart_send_pack(uint8_t *pData, int len);

#endif
