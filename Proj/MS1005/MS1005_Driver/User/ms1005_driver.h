#ifndef __MS1005_DRIVER_H
#define __MS1005_DRIVER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f10x.h"
#include "stdio.h"

    /**
     * @brief 32-bit union for byte/word access
     */
    typedef union
    {
        uint32_t word;
        uint8_t buf[4];
    } u32_t;

    extern unsigned char Intn_flag; // MS1005中断标志(0:无中断,1:有中断)4
    extern void Delay(unsigned int d);

    /**
     * @brief GPIO Definitions		引脚定义
     */
// RST引脚
#define TDC_RSTN_PIN GPIO_Pin_0
#define TDC_RSTN_PORT GPIOB
#define TDC_RSTN_CLK RCC_APB2Periph_GPIOB

// INTN引脚
#define TDC_INTN_PIN GPIO_Pin_1
#define TDC_INTN_PORT GPIOB
#define TDC_INTN_CLK RCC_APB2Periph_GPIOB

// CAL引脚
#define TDC_CAL_PIN GPIO_Pin_15
#define TDC_CAL_PORT GPIOB
#define TDC_CAL_CLK RCC_APB2Periph_GPIOB

// NCAL引脚
#define TDC_NCAL_PIN GPIO_Pin_13
#define TDC_NCAL_PORT GPIOB
#define TDC_NCAL_CLK RCC_APB2Periph_GPIOB

// TBD init Exti
#define TDC_INTN_IRQn EXTI1_IRQn
#define TDC_INTN_PORT_SOURCE EXTI_SOURCE_GPIOA
#define TDC_INTN_PIN_SOURCE EXTI_SOURCE_PIN1
#define TDC_INTN_LINE EXTI_1

// TEST_Mode
#define TDC_START_PIN GPIO_Pin_11
#define TDC_START_PORT GPIOA
#define TDC_START_CLK RCC_APB2Periph_GPIOA

#define TDC_STOP1_PIN GPIO_Pin_12
#define TDC_STOP1_PORT GPIOA
#define TDC_STOP1_CLK RCC_APB2Periph_GPIOA

#define TDC_STOP2_PIN GPIO_Pin_12
#define TDC_STOP2_PORT GPIOA
#define TDC_STOP2_CLK RCC_APB2Periph_GPIOA

// TDC SPI NSS
#define TDC_SSN_PIN GPIO_Pin_4
#define TDC_SSN_PORT GPIOA
#define TDC_SSN_CLK RCC_APB2Periph_GPIOA

    /**
     * @brief 操作码
     */
    typedef enum
    {
        WRITE_CONFIG_RES = 0x80, // 写入配置寄存器          写入 32bit
        READ_STOP1_DATA0 = 0xB0, // 读取地址 STOP1 数据 0   读出 24bit
        READ_STOP1_DATA1 = 0xB1, // 读取地址 STOP1 数据 1   读出 24bit
        READ_STOP1_DATA2 = 0xB2, // 读取地址 STOP1 数据 2   读出 24bit
        READ_STOP1_DATA3 = 0xB3, // 读取地址 STOP1 数据 3   读出 24bit
        READ_STOP1_DATA4 = 0xB4, // 读取地址 STOP1 数据 4   读出 24bit
        READ_STOP1_DATA5 = 0xB5, // 读取地址 STOP1 数据 5   读出 24bit
        READ_STOP1_DATA6 = 0xB6, // 读取地址 STOP1 数据 6   读出 24bit
        READ_STOP1_DATA7 = 0xB7, // 读取地址 STOP1 数据 7   读出 24bit
        READ_STOP1_DATA8 = 0xB8, // 读取地址 STOP1 数据 8   读出 24bit
        READ_STOP1_DATA9 = 0xB9, // 读取地址 STOP1 数据 9   读出 24bit
        READ_STOP2_DATA0 = 0xC0, // 读取地址 STOP2 数据 0   读出 24bit
        READ_STOP2_DATA1 = 0xC1, // 读取地址 STOP2 数据 1   读出 24bit
        READ_STOP2_DATA2 = 0xC2, // 读取地址 STOP2 数据 2   读出 24bit
        READ_STOP2_DATA3 = 0xC3, // 读取地址 STOP2 数据 3   读出 24bit
        READ_STOP2_DATA4 = 0xC4, // 读取地址 STOP2 数据 4   读出 24bit
        READ_STOP2_DATA5 = 0xC5, // 读取地址 STOP2 数据 5   读出 24bit
        READ_STOP2_DATA6 = 0xC6, // 读取地址 STOP2 数据 6   读出 24bit
        READ_STOP2_DATA7 = 0xC7, // 读取地址 STOP2 数据 7   读出 24bit
        READ_STOP2_DATA8 = 0xC8, // 读取地址 STOP2 数据 8   读出 24bit
        READ_STOP2_DATA9 = 0xC9, // 读取地址 STOP2 数据 9   读出 24bit
        READ_CAL_DATA = 0xBB,    // 读取校验数据(CAL)       读出 24bit
        READ_CONFIG_RES = 0xBC,  // 读取配置寄存器的低8位    读出 8bit
        READ_STAT_RES = 0xBD,    // 读取状态(STAT)          读出 24bit
        INIT_DEVICE = 0x70,      // 初始化指令
        RESET_DEVICE = 0x50,     // 复位指令
        START_CAL_TDC = 0x04,    // CAL 测量指令
    } OPERATE_CODE;
    
    void TDC_Init(void);
    void MS1005_START(void);
    void MS1005_STOP(void);
    uint8_t TDC_ReadByte(uint8_t op);
    uint32_t TDC_Read3Bytes(uint8_t op);
    uint32_t TDC_ReadCal(void);
    uint32_t TDC_ReadState(void);
    uint32_t TDC_ReadNCalResult(uint8_t op);
    float TDC_ReadCalResult(uint8_t op);
    void TDC_WriteReg(uint8_t op, uint32_t val);
    void TDC_NCalModeCfg(void);
    void TDC_CalModeCfg(void);
    void TDC_WriteRegVerify(uint8_t op_write, uint32_t val, uint8_t read_op);
    void TDC_MeasureStart(void);
    void MS1005_AllReset(void);
    void TDC_GpioConfig(void);
    void MS1005_NVIC_Configuration(void);

#ifdef cplusplus
}
#endif

#endif
