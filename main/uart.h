#ifndef _UART_H_
#define _UART_H_

extern void uart_init(void);
extern void uart_send_pack(uint8_t *pData, int len);

extern uint8_t uart_recv_buff[2048];
extern uint16_t recv_read_len;
extern uint16_t uart_recv_head;
extern uint16_t uart_recv_tail;

#endif
