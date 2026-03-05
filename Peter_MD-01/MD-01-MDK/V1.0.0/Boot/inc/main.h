#ifndef __MAIN_H
#define __MAIN_H

#ifdef cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>

#include "gd32f4xx.h"
#include "systick.h"
#include "ymodem.h"
#include "user_uart.h"
#include "user_flash.h"

#define BOOT_USARTx USER_COM1

#ifdef cplusplus
}
#endif

#endif /* __MAIN_H */
