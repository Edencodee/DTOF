/**
 * @file drv_flash.c
 * @brief 
 * @author jinf (jinf_li@foxmail.com)
 * @version 1.0
 * @date 2021-01-12
 * 
 * @copyright Copyright (c) 2021
 * 
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Version <th>Author  <th>Description
 * <tr><td>2021-01-12 <td>1.0     <td>jinf    <td>内容
 * </table>
 */

#include "drv_flash.h"



static uint32_t GetPage(uint32_t Addr);

/**
 * @brief Flash_Write_Byte
 * 
 * @param  flash_address    flash address
 * @param  data_buffer      the buffer of data to be written
 * @param  length           the 
 * @return int 
 */
int Flash_Write_Byte(uint32_t flash_address, uint8_t *data_buffer, uint32_t length)
{
  /*要擦除的起始扇区(包含)及结束扇区(不包含)，如8-12，表示擦除8、9、10、11扇区*/

  uint32_t FirstPage = 0;
  uint32_t NbOfPages = 0;
  uint32_t Address = 0;
  uint32_t PageError = 0;
  uint32_t FLASH_USER_END_ADDR = flash_address + length;                //实际数据所占地址
  uint32_t FLASH_ERASE_END_ADDR = flash_address + (length + 7) / 8 * 8; //实际擦除的区域，8的倍数
  uint8_t cnt = 0;
  uint32_t pos = 0;
  uint8_t *Data_Buffer = data_buffer;

  __IO uint32_t MemoryProgramStatus = 0;
  __IO uint32_t data32 = 0;
  __IO uint8_t data8 = 0;

  static FLASH_EraseInitTypeDef EraseInitStruct;
  //printf("FLASH_USER_END_ADDR = 0x%8x, FLASH_ERASE_END_ADDR = 0x%8x\n", FLASH_USER_END_ADDR, FLASH_ERASE_END_ADDR);
  /* FLASH 解锁 ********************************/
  /* 使能访问FLASH控制寄存器 */
  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  /* Clear OPTVERR bit set on virgin samples */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

  /* Erase the user Flash area
  (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

  /* Get the 1st page to erase */
  FirstPage = GetPage(flash_address);

  /* Get the number of pages to erase from 1st page */
  NbOfPages = GetPage(FLASH_USER_END_ADDR) - FirstPage + 1;

  /* Fill EraseInit structure*/
  EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.Page = FirstPage;
  EraseInitStruct.NbPages = NbOfPages;

  /* 开始擦除操作 */
  if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
  {
    /*擦除出错，返回，实际应用中可加入处理 */
    return -1;
  }
  /* 以“字”的大小为单位写入数据 ********************************/
  Address = flash_address;

  while (Address < FLASH_ERASE_END_ADDR)
  {
    data32 = 0x00;
    for (cnt = 0; cnt < 4; cnt++)
    {
      if (Address < FLASH_USER_END_ADDR)
      {
        data32 |= (Data_Buffer[pos++] << (cnt * 8));
      }
    }
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, data32) == HAL_OK)
    {
      Address = Address + 4; /* increment to next double word*/
      //Data_Buffer = Data_Buffer + 4;
    }
    else
    {
      /*写入出错，返回，实际应用中可加入处理 */
      return -1;
    }
  }

  /* 给FLASH上锁，防止内容被篡改*/
  HAL_FLASH_Lock();

  /* 从FLASH中读取出数据进行校验***************************************/
  /*  MemoryProgramStatus = 0: 写入的数据正确
	  MemoryProgramStatus != 0: 写入的数据错误，其值为错误的个数 */
  Address = flash_address;
  Data_Buffer = data_buffer;
  MemoryProgramStatus = 0;

  while (Address < FLASH_USER_END_ADDR)
  {
    data8 = *(__IO uint8_t *)Address;

    if (data8 != *Data_Buffer++)
    {
      MemoryProgramStatus++;
    }

    Address = Address + 1;
  }
  /* 数据校验不正确 */
  if (MemoryProgramStatus)
  {
    return -1;
  }
  else /*数据校验正确*/
  {
    return 0;
  }
}
/**
 * @brief 
 * @param  flash_address    My Param doc
 * @param  data_buffer      My Param doc
 * @param  length           读取数据
 * @return int 
 */
int Flash_Read_Byte(uint32_t flash_address, uint8_t *data_buffer, uint32_t length)
{
  int result = 1;
  for (uint32_t i = 0; i < length; i++)
  {
    data_buffer[i] = *(__IO uint8_t *)flash_address;
    flash_address += 1;
  }

  return result;
}

/**
  * @brief  Gets the page of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The page of a given address
  */
static uint32_t GetPage(uint32_t Addr)
{
  return (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
}
