


#include "spad_protocol.h"



/*功能描述：获取SPAD命令指令帧校验位
**输入参数：cmd：存放命令帧首地址
**输出参数：命令帧校验位
**备注说明：v1.0
**/
uint8_t Get_CMD_ISTR_XOR(SPAD_CMD_ISTR *cmd)
{
	return (cmd->root ^ cmd->leaf ^ cmd->parm_size);
}
/*功能描述：获取SPAD参数指令帧校验位
**输入参数：cmd：存放命令帧首地址
**输出参数：命令帧校验位
**备注说明：v1.0
**/
uint8_t Get_PARM_ISTR_XOR(SPAD_PARM_ISTR *parm, size_t size)
{
	uint8_t crc = 0;

	for (int i = 0; i < size; i++) {
		crc ^= parm->p_parm[i];
	}
	//uint8

	return crc;
}
void Basic_Set_CMD(SPAD_CMD_ISTR *cmd, BasicSettingCMD_TYPE cmd_mode)
{
	cmd->header    = SPAD_CMD_HEADER;
	cmd->root      = Mode_Setting_Cmd;
	cmd->leaf      = cmd_mode;
	cmd->parm_size = 0x00;
	cmd->crc       = Get_CMD_ISTR_XOR(cmd);
}
void OPR_Mode_Set_CMD(SPAD_CMD_ISTR *cmd, ModeSettingCMD_TYPE cmd_mode)
{
	cmd->header    = SPAD_CMD_HEADER;
	cmd->root      = Mode_Setting_Cmd;
	cmd->leaf      = cmd_mode;
	cmd->parm_size = 0x00;
	cmd->crc       = Get_CMD_ISTR_XOR(cmd);
}

/*功能描述：配置SPAD命令指令帧
**输入参数：cmd：存放命令帧首地址
**输入参数：cmd_type：命令帧类型
**输入参数：cmd_code：命令帧副脚本
**输出参数：无
**备注说明：v1.0
**/
void SPAD_CMD_ISTR_FRAME(
    SPAD_CMD_ISTR *cmd,
    SPAD_CMD_TYPE cmd_type,
    uint16_t cmd_mode)
{
	cmd->header    = SPAD_CMD_HEADER;
	cmd->root      = cmd_type;
	cmd->leaf      = cmd_mode;
	cmd->parm_size = 0x00;
	cmd->crc       = Get_CMD_ISTR_XOR(cmd);
}







