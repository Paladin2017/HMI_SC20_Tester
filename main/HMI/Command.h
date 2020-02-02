#ifndef _HMI_COMMAND_H_
#define _HMI_COMMAND_H_

// event IDs
// gcode from PC
#define EID_GCODE_REQ         1
#define EID_GCODE_RESP        2
// gcode from file
#define EID_FILE_GCODE_REQ    3
#define EID_FILE_GCODE_RESP   4
// file operation
#define EID_FILE_OP_REQ       5
#define EID_FILE_OP_RESP      6
// status query
#define EID_STATUS_REQ        7
#define EID_STATUS_RESP       8
// settings operation
#define EID_SETTING_REQ       9
#define EID_SETTING_RESP      0xa
// movement command
#define EID_MOVEMENT_REQ      0xb
#define EID_MOVEMENT_RESP     0xc
// laser&camera opration
#define EID_LAS_CAM_OP_REQ    0xd
#define EID_LAS_CAM_OP_RESP   0xe
// upgrade command
#define EID_UPGRADE_REQ       0xa9
#define EID_UPGRADE_RESP      0xaa
// Add-on operation
#define EID_ADDON_OP_REQ      0x11
#define EID_ADDON_OP_RESP     0x12
// laser calibration
#define EID_LASER_CALIBRATE_REQ   0x13
#define EID_LASER_CALIBRATE_RESP  0x14

#endif