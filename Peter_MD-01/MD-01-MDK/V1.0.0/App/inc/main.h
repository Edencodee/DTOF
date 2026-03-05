#ifndef __MAIN_H
#define __MAIN_H

#ifdef cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#include "gd32f4xx.h"
#include "systick.h"

#include "user_flash.h"
#include "user_uart.h"
#include "user_spi.h"
#include "user_gpio.h"
#include "user_timer.h"
#include "oled.h"
#include "ms1005.h"

#include "task.h"
#include "task_oled.h"
#include "task_short.h"
#include "task_led.h"
#include "task_output.h"
#include "task_dac.h"
#include "task_flash.h"
#include "task_meas.h"

#include "gauss_newton.h"
#include "spline_intp.h"
#include "matrix.h"

#include "modbus.h"

#ifdef cplusplus
}
#endif

#endif /* __MAIN_H */
