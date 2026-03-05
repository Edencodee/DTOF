#include "ymodem.h"
#include "main.h"

#define SOH	0x01  // start of 128-byte pData packet
#define STX	0x02  // start of 1024-byte pData packet
#define EOT	0x04  // end of transmission
#define ACK	0x06  // acknowledge
#define NAK	0x15  // negative acknowledge
#define CA	0x18  // two of these in succession aborts transfer
#define C	0x43  // 'C' == 0x43, request 16-bit CRC

#define ABORT1	0x41  // 'A' == 0x41, abort by user
#define ABORT2	0x61  // 'a' == 0x61, abort by user

#define PAK_IDX			1
#define PAK_IDX_COMP	2
#define PAK_HDR			3
#define PAK_CRC			2
#define PAK_OVER_HEAD	(PAK_HDR + PAK_CRC)
#define PAK_SIZE		128
#define PAK_1K_SIZE		1024

#define MAX_ERR 10
#define TIMEOUT	1000

typedef enum {
	kYmodemCmdIdel,
	kYmodemCmdStart,
	kYmodemCmdEnd,
	kYmodemCmdData,
	kYmodemCmdEOT,
	kYmodemCmdCA,
	kYmodemCmdAbort
} ymodem_cmd_t;

//#define IS_AF(c)		((c >= 'A') && (c <= 'F'))
//#define IS_af(c)		((c >= 'a') && (c <= 'f'))
#define IS_09(c)		((c >= '0') && (c <= '9'))
//#define ISVALIDHEX(c)	IS_AF(c) || IS_af(c) || IS_09(c)
//#define CONVERT2HEX(c)	(IS_09(c) ? (c - '0') : (IS_AF(c) ? (c - 'A'+10) : (c - 'a'+10)))
#define CONVERT2DEC(c)	(c - '0')

uint32_t g_fSize = 0;
uint8_t g_fileNameBuf[128] = {0};

__IO uint32_t g_length = 0;
__IO uint32_t g_address = APP_ADDR;
__IO uint32_t g_pSize = 0;
uint8_t g_buffer[PAGE_SIZE];
uint8_t g_packData[PAK_1K_SIZE + PAK_OVER_HEAD];

static int32_t Str2Hex(const uint8_t* str, uint32_t *hex) {
	uint32_t res = 0;
	for(int i = 0; (str[i] != 0) && (str[i] != ' ')/*i < size*/; i++) {
		if(!(IS_09(str[i])/*ISVALIDHEX(str[i])*/)) {
			return -1;
		}
//		res = (res << 4) + CONVERT2HEX(str[i]);
		res = res * 10 + CONVERT2DEC(str[i]);
	}
	*hex = res;
	return 0;
}

static uint16_t UpdateCRC16(uint16_t crc, uint8_t val) {
	for(uint32_t i = 8; i > 0; i--) {
		if(crc & 0x8000) {
			crc <<= 1;
			crc += ((val & 0x80) != 0);
			crc ^= 0x1021;
		} else {
			crc <<= 1;
			crc += ((val & 0x80) != 0);
		}
		val <<= 1;
	}

	return crc;
}

static uint16_t CalCRC16(const uint8_t* p, size_t len) {
	uint32_t crc = 0;

	for(size_t i = 0; i < len; i++) {
		crc = UpdateCRC16(crc, p[i]);
	}

	crc = UpdateCRC16(crc, 0);
	crc = UpdateCRC16(crc, 0);

	return crc;
}

static void Buffer2Flash() {
	uint16_t pageRemain = g_length % PAGE_SIZE;
	if(pageRemain != 0) {
		memset(g_buffer + g_length, 0xff, PAGE_SIZE - pageRemain);
	}

	static uint32_t s_sectorNum = FMC_WRONG_SECTOR_NUM;
	uint32_t sectorNum = FLASH_InfoGet(g_address).sector_num;
	if(s_sectorNum != sectorNum) { // this sector not erase
		FLASH_UserErase(sectorNum);
		s_sectorNum = sectorNum;
	}
	FLASH_UserWrite(g_address, (uint32_t*)g_buffer, PAGE_SIZE / 4);

	g_address += PAGE_SIZE;
	g_length = 0;
}

static void Receive2Buffer(uint8_t* buf, uint16_t len) {
	memcpy(&g_buffer[g_length], buf, len);

	g_length += len;
	
	if(g_length >= PAGE_SIZE) {
		Buffer2Flash();
	}
}

static int32_t ReceivePacket(uint8_t *pData, uint32_t timeout, ymodem_cmd_t* cmd) {
	uint16_t crc;

	uint8_t *pFileSize;
	uint8_t *pFileName;
	uint8_t nameLen = 0;

	if(UART_RecvData(BOOT_USARTx, pData, -1, timeout) < 0) {
		return -1;
	}

	switch(pData[0]) {
	case SOH: {
		g_pSize = PAK_SIZE;

		if(pData[PAK_IDX] != (pData[PAK_IDX_COMP] ^ 0xff)) {
			return -1;
		}

		crc = CalCRC16(pData + PAK_HDR, g_pSize);
		if(crc != (uint16_t)((pData[g_pSize + 3] << 8) | pData[g_pSize + 4])) {
			return -1;
		}

		if(pData[PAK_IDX] == 0) {
			if(pData[PAK_HDR] != 0) {
				// get fileName & fileSize
				pFileName = pData + PAK_HDR;
//				while((fileName[nameLen++] != 0) && (nameLen < 128)) {
				while(pFileName[nameLen++] != 0) {
					if(nameLen >= 128) {
						return -1;
					}
				}
				memcpy(g_fileNameBuf, pFileName, nameLen - 1);
				pFileSize = pFileName + nameLen;
				Str2Hex(pFileSize, &g_fSize);

				// Test the size of the image to be sent
				// Image size is greater than Flash size
				if(g_fSize > FMC_SIZE_IN_BYTES) {
					UART_SendChar(BOOT_USARTx, CA, timeout);
					UART_SendChar(BOOT_USARTx, CA, timeout);
					return -1;
				}

				// ack contiune
				UART_SendChar(BOOT_USARTx, ACK, timeout);
				UART_SendChar(BOOT_USARTx, C, timeout);

				*cmd = kYmodemCmdStart;
			} else {
				UART_SendChar(BOOT_USARTx, ACK, timeout);
				*cmd = kYmodemCmdEnd;
			}

			return 0;
		}
	} break;

	case STX: {
		g_pSize = PAK_1K_SIZE;

		if(pData[PAK_IDX] != (pData[PAK_IDX_COMP] ^ 0xff)) {
			return -1;
		}

		crc = CalCRC16(pData + PAK_HDR, g_pSize);
		if(crc != (((uint16_t)pData[g_pSize + 3] << 8) | pData[g_pSize + 4])) {
			return -1;
		}
		
		if(pData[PAK_IDX] == 0) {
			if(pData[PAK_HDR] != 0) {
				// get fileName & fileSize
				pFileName = pData + PAK_HDR;
//				while((fileName[nameLen++] != 0) && (nameLen < 128)) {
				while(pFileName[nameLen++] != 0) {
					if(nameLen >= 128) {
						return -1;
					}
				}
				memcpy(g_fileNameBuf, pFileName, nameLen - 1);
				pFileSize = pFileName + nameLen;
				Str2Hex(pFileSize, &g_fSize);

				// Test the size of the image to be sent
				// Image size is greater than Flash size
				if(g_fSize > FMC_SIZE_IN_BYTES) {
					UART_SendChar(BOOT_USARTx, CA, timeout);
					UART_SendChar(BOOT_USARTx, CA, timeout);
					return -1;
				}

				// ack contiune
				UART_SendChar(BOOT_USARTx, ACK, timeout);
				UART_SendChar(BOOT_USARTx, C, timeout);

				*cmd = kYmodemCmdStart;
			} else {
				UART_SendChar(BOOT_USARTx, ACK, timeout);
				*cmd = kYmodemCmdEnd;
			}

			return 0;
		}
	} break;

	case EOT: {
		UART_SendChar(BOOT_USARTx, NAK, timeout);
		if((UART_RecvByte(BOOT_USARTx, pData + 1, timeout) == 0) && (pData[1] == EOT)) {
			UART_SendChar(BOOT_USARTx, ACK, timeout);
			UART_SendChar(BOOT_USARTx, C, timeout);
			*cmd = kYmodemCmdEOT;
			return 0;
		}
		return -1;
	}

	case CA: {
		if((UART_RecvByte(BOOT_USARTx, pData + 1, timeout) == 0) && (pData[1] == CA)) {
			UART_SendChar(BOOT_USARTx, ACK, timeout);
			*cmd = kYmodemCmdCA;
			return 0;
		}
		return -1;
	}

	case ABORT1:
	case ABORT2: {
		UART_SendChar(BOOT_USARTx, CA, timeout);
		UART_SendChar(BOOT_USARTx, CA, timeout);
		*cmd = kYmodemCmdAbort;
		return 0;
	}

	default: {
		return -1;
	}
	}

	// buffer received pData
	Receive2Buffer(pData + PAK_HDR, g_pSize);
	UART_SendChar(BOOT_USARTx, ACK, timeout);
	*cmd = kYmodemCmdData;

	return 0;
}

int32_t YmodemReceive() {
	ymodem_cmd_t cmd = kYmodemCmdIdel;
	uint32_t pIndex = 0;
	uint32_t errs = 0;
	int32_t end = 0;
	int32_t start = 0;

	while(end == 0) {
		if(ReceivePacket(g_packData, TIMEOUT, &cmd) < 0) {
			if(start > 0) {
				if(++errs > MAX_ERR) {
					UART_SendChar(BOOT_USARTx, CA, TIMEOUT);
					UART_SendChar(BOOT_USARTx, CA, TIMEOUT);
					return -1;
				}
			} else {
				UART_SendChar(BOOT_USARTx, C, TIMEOUT);
			}
			continue;
		}

		errs = 0;

		switch(cmd) {
		case kYmodemCmdStart:		
		case kYmodemCmdData: {
			if(g_packData[PAK_IDX] != pIndex) {
				UART_SendChar(BOOT_USARTx, ACK, TIMEOUT);
				continue;
			} //else {
			pIndex++;	
			start = 1;
			//}
		} break;

		case kYmodemCmdEnd: {
			end = 1;
		} break;

		case kYmodemCmdEOT: {
			pIndex = 0;
		} break;

		case kYmodemCmdCA:
		case kYmodemCmdAbort: {
			return 0;
		}

		default: {
			;
		}
		}
	}

	Buffer2Flash();

	return 0;
}
