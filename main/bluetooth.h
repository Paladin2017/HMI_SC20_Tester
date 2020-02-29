#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_

extern void bluetooth_init(void);
extern void bt_send_protocal_data(uint8_t EventID, uint8_t OpCode, uint8_t * pData, int Len);

extern uint8_t bt_recv_buff[1024];
extern uint16_t bt_recv_read_len;
extern uint16_t bt_recv_head;
extern uint16_t bt_recv_tail;

#endif
