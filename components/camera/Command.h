#ifndef _BT_COMMAND_H_
#define _BT_COMMAND_H_

#include "../../main/bluetooth.h"

typedef enum {
    M_REPORT_VERSIONS = 0x1,
    S_REPORT_VERSIONS,
    M_CAMERA_GET_AWB = 0x3,
    S_CAMERA_GET_AWB_ACK,
    M_CAMERA_SET_AWB = 0x5,
    S_CAMERA_SET_AWB_ACK,
    M_CAMERA_SET_ACE = 0x7,
    S_CAMERA_SET_ACE_ACK,
    M_CAMERA_SET_IMG_SIZE = 0x9,
    S_CAMERA_SET_IMG_SIZE_ACK,
    M_CAMERA_SET_QUALITY = 0xb,
    S_CAMERA_SET_QUALITY_ACK,
    M_CAMERA_GET_IMG = 0xd,
    S_CAMERA_IMG_ACK,
    M_UPDATE_MOUDLE = 0xf,
    S_UPDATRE_ACK,
    M_SET_BT_NAME = 0x11,
    S_SET_BT_NAME_ACK,
    M_REPORT_BT_NAME = 0x13,
    S_REPORT_BT_NAME_ACK,
    M_REPORT_BT_MAC = 0x15,
    S_REPORT_BT_MAC_ACK,
    M_SET_CAMERA_LIGHT = 0x17,
    S_SET_CAMERA_LIGHT_ACK,
    M_REPORT_CAMERA_LIGHT = 0x19,
    S_REPORT_CAMERA_LIGHT_ACK,
    M_REPORT_CAMERA_STATU = 0x1b,
    S_REPORT_CAMERA_STATU_ACK,

    S_CAMERA_INIT_FAIL = 0xfd,
    S_RECV_FAIL = 0xff,
}BT_CMD_E;

extern void bt_command_process(uint8_t EventID, uint8_t Opcode);
extern void bt_command_init();

#endif