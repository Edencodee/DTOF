
#ifndef _SAPD_INTERFACE_H
#define _SAPD_INTERFACE_H

#include "spad_protocol.h"
#include "board.h"



void SPAD_Stop_Mode_Cmd(void);
void SPAD_Self_Test_Cmd(void);


FlagStatus SPAD_Start_Meas(void);
FlagStatus SPAD_Stop_Meas(void);

FlagStatus SPAD_Self_Test(void);
FlagStatus SPAD_Meas_Mode(ModeSettingCMD_TYPE meas_mode);
FlagStatus SPAD_Engineer_Mode(void);
#endif

