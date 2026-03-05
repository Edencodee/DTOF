
#ifndef _SPAD_PROTOCOL_H
#define _SPAD_PROTOCOL_H


#include "board.h"


//  SPAD 控制指令 **********************************************

#define SPAD_CMD_HEADER    0XCAAC
#define SPAD_PARM_HEADER   0XB44B

#pragma pack(1)
typedef struct {
	uint16_t header;
	uint8_t  root;
	uint8_t  leaf;
	uint8_t  parm_size;
	uint8_t  crc;
} SPAD_CMD_ISTR;
#pragma pack()
#pragma pack(1)
typedef struct {
	uint16_t header;
	uint8_t* p_parm;
	uint8_t  parm_size;
	uint8_t  crc;
} SPAD_PARM_ISTR;

#pragma pack()
//cmd ************************************************
/*  各指令说明
编号	指令	                            说明
1	停止报点指令     (Stop Cmd)	            关闭 MCU 报点模式。
2	启动报点指令     (Start Cmd)	        启动 MCU 报点模式。
3	自我测试指令     (Self-test Cmd)	    芯片自我测试
4	工程模式指令     (Engineering mode Cmd)	设定工程模式
5	设定报点模式指令 (Mode-setting Cmd)	    设定报点模式
6	韧体版本指令     (Version Cmd)	        读取韧体版本
*/
//cmd enum : root
typedef enum {
	Basic_Setting_Cmd = 0x00,
	Self_Test_Cmd     = 0x02,
	Engineer_Mode_Cmd = 0x03,
	Mode_Setting_Cmd  = 0x05,
	Version_Mode_Cmd  = 0x06,
} SPAD_CMD_TYPE;

//basic setting cmd
/*  基本设定指令(Basic_Setting_Cmd)列表
编号	Root	Leaf	PARM size
1	    0x00  0x00~0x01	  0x00
说明:
(1)	root 指令：启动/关闭 MCU 报点模式。
(2)	leaf 指令：
0x00：停止报点，进入等待(idle)模式。
0x01：启动报点。
【注】操作其他指令前需先下达本指令之停止报点。
为避免后续指令传输失败，下达停止报点指令后，建议确认 UART 是否继续
传输报点数据，若未停止传输，需重新下达停止指令。

*/
//leaf enum
typedef enum {
	STOP_MODE  = 0x00,
	START_MODE = 0x01,
} BasicSettingCMD_TYPE;
//

//mode setting cmd
/*  运行模式设定指令(Mode_Setting_Cmd)列表
说明
(1)	root 指令：0x05 读取/设定报点模式
(2)	Leaf 指令：
0x00：读取报点模式
0x01：修改报点模式为标准 1.8KHz
0x02：修改报点模式为标准 3.6KHz
0x03：修改报点模式为标准 7.2KHz
0x04：修改报点模式为标准 14.4KHz
0x22：修改报点模式为双回波 7.2KHz
(3) PARM size: 0x00;

*/

// leaf enum
typedef enum {
	READ_RPT_MODE    = 0X00,
	MODIFY_RPT_1_8K  = 0x01,
	MODIFY_RPT_3_6K  = 0x02,
	MODIFY_RPT_7_2K  = 0x03,
	MODIFY_RPT_14_4K = 0x04,
	MODIFY_RPT_2ECHO = 0x22
} ModeSettingCMD_TYPE;
//

//engineer mode cmd
/*  工程报点模式指令(Engineering Mode_Cmd)列表
编号  Root	Leaf  PARM size	说明
1	  0x03	0xa0	0	    设定报点模式为工程模式

*/
#define ENGINEERIONG_MODE    0xA0

//version mode cmd
/*  韧体版本指令(Version_Mode_Cmd)列表
编号  Root	Leaf  PARM size	说明
1	  0x06	0x00  0x00	    读取韧体版本码

*/
#define VERSION_MODE         0x00

#define SELF_TEST_MODE         0x00


// SPAD
typedef struct {
	uint16_t dist;
	uint16_t engy;
} SenPF; //Sensing Point Frame

#define SPAD_PACKET_LIDAR_HEADER   0xFA

#pragma pack(push, 1)
typedef struct {
	uint8_t header;
	SenPF   data[1];
	uint8_t crc;
} PacketF_1CH_LiDAR;

typedef struct {
	uint8_t header;
	SenPF   data[2];
	uint8_t crc;
} PacketF_2CH_LiDAR;

typedef struct {
	uint8_t header;
	SenPF   data;
	int16_t temp;
	uint16_t snr;
	uint8_t crc;
} PacketF_ENGN_LiDAR;
#pragma pack(pop)

#define  PackSize1   sizeof(PacketF_1CH_LiDAR)
#define  PackSize2   sizeof(PacketF_2CH_LiDAR)

void SPAD_CMD_ISTR_FRAME(
    SPAD_CMD_ISTR *cmd,
    SPAD_CMD_TYPE cmd_type,
    uint16_t cmd_mode);

typedef struct _spad_msg_packet {
	uint32_t *packet_buf;
	size_t    packet_size;
	SenPF     *scan_point;

	void (*getpoint)(struct _spad_msg_packet *msg);
} SPAD_MSG_PACKET;



#endif


