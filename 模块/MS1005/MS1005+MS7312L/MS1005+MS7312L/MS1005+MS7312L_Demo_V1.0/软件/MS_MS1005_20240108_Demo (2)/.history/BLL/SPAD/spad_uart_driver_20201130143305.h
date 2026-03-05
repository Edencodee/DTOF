

#ifndef _SPAD_UART_DRIVER_H
#define _SPAD_UART_DRIVER_H

#include "board.h"

#include "spad_protocol.h"
#include "scan_msg_processing.h"
#include "drv_usart.h"

//uint8_t SPAD_Msg_Parsing(uint8_t* buf, SenPF* point, uint16_t data_len, size_t packet_size);
uint8_t SPAD_Msg_Parsing(uint8_t* buf, SenPF* point, uint16_t data_len, uint16_t workmode);
uint8_t SPAD_Msg_Parsing_V1_1(struct kfifo *fifo, SCAN_MSG_PACKET scan_msg, uint16_t workmode);

#endif



