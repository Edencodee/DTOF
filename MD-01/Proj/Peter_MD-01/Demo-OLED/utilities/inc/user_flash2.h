#ifndef __USER_FLASH_H
#define __USER_FLASH_H

#ifdef cplusplus
extern "C" {
#endif

#include "gd32f4xx.h"

#define PAGE_SIZE			0x1000		// 4k
#define FMC_SIZE_IN_BYTES	0x78000		// 512kB - 32kB
#define DATA_ADDR			0x8004000ul	// data 16k, sector 1, 0x8004000~0x8007fff
#define APP_ADDR			0x8008000ul	// boot 16k, sector 0, 0x8000000~0x8003fff
										// app, 0x8008000~0x80fffff

/* sector size */
#define SIZE_16KB           ((uint32_t)0x00004000U)        /*!< size of 16KB*/
#define SIZE_64KB           ((uint32_t)0x00010000U)        /*!< size of 64KB*/
#define SIZE_128KB          ((uint32_t)0x00020000U)        /*!< size of 128KB*/
#define SIZE_256KB          ((uint32_t)0x00040000U)        /*!< size of 256KB*/

/* FMC BANK address */
#define FMC_START_ADDRESS          FLASH_BASE                               /*!< FMC start address */
#define FMC_BANK0_START_ADDRESS    FMC_START_ADDRESS                        /*!< FMC BANK0 start address */
#define FMC_BANK1_START_ADDRESS    ((uint32_t)0x08100000U)                  /*!< FMC BANK1 start address */
#define FMC_SIZE                   (*(uint16_t *)0x1FFF7A22U)               /*!< FMC SIZE */
#define FMC_END_ADDRESS            (FLASH_BASE + (FMC_SIZE * 1024) - 1)     /*!< FMC end address */
#define FMC_MAX_END_ADDRESS        ((uint32_t)0x08300000U)                  /*!< FMC maximum end address */

/* FMC error message */
#define FMC_WRONG_SECTOR_NAME      ((uint32_t)0xFFFFFFFFU)        /*!< wrong sector name*/
#define FMC_WRONG_SECTOR_NUM       ((uint32_t)0xFFFFFFFFU)        /*!< wrong sector number*/
#define FMC_INVALID_SIZE           ((uint32_t)0xFFFFFFFFU)        /*!< invalid sector size*/
#define FMC_INVALID_ADDR           ((uint32_t)0xFFFFFFFFU)        /*!< invalid sector address*/

	/* FMC sector information */
	typedef struct {
		uint32_t sector_name;			/*!< the name of the sector 		 */
		uint32_t sector_num;			/*!< the number of the sector 		 */
		uint32_t sector_size;			/*!< the size of the sector 		 */
		uint32_t sector_start_addr;		/*!< the start address of the sector */
		uint32_t sector_end_addr;		/*!< the end address of the sector	 */
	} fmc_sector_info_struct;

	typedef enum {
		kFlagDebug = 0,
		kFlagAdjust,
		kFlagSearch,
	} flag_t;
	
	typedef enum {
		kNpn = 0,
		kPnp
	} mod_t;
	
	typedef enum {
		kNormal = 0,
		k1PCali,
		k2PCali,
		k3PCali
	} out_t;

	typedef enum {
		kOnNeg = 0,
		kOffNeg
	} pol_t;
	
	typedef enum {
		kRegSens = 0,
		kRegPeriod,
		kRegIntT,
		kRegNPN,
		kRegNONC1,	
		kRegOut,
		kRegBps,
		kRegOffs,
		kRegSetCnt,

		kRegWTop = 20,	// top pixel * 10
		kRegWBtm,		// btm pixel * 10
		kRegCenter,		// center pixel * 10
		kRegRange,
		kRegPixel,
		kRegPosCnt,
		
		kRegQ1Thr1 = 50,
		kRegQ1Thr2,
		kRegQ2Thr1,
		kRegQ2Thr2,
		kRegThrCnt,

		kRegPos = 100,
		kRegDist,
		kRegFlag,
		kRegTempCnt,
		
		kRegCnt
	} reg_t;

	extern __IO uint16_t g_reg[];
	extern const uint16_t regDef[];
	extern const uint16_t regMin[];
	extern const uint16_t regMax[];

	/* using in boot */
	fmc_sector_info_struct FLASH_InfoGet(uint32_t addr);
	void FLASH_UserErase(uint32_t sectorNum);
	void FLASH_UserWrite(uint32_t addr, uint32_t *buf, uint16_t size);

	/* using in app  */
	void FLASH_RegWrite(void);
	void FLASH_RegLoad(void);
	void FLASH_RegReinit(void);

#ifdef cplusplus
}
#endif

#endif
