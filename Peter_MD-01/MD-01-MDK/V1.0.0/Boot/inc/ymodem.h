#ifndef __YMODEM_H
#define __YMODEM_H

#define USER_FLASH_SIZE              0x3E800U

#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

int32_t YmodemReceive(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif
