#include "user_spi.h"
#include "systick.h"
#include <string.h>

/* transmit variable */
#if SPI1_DMA
#define SPI1_TX_SIZE	1
uint8_t g_spi1TxBuffer[SPI1_TX_SIZE] = {0x00};
#endif

static void SPI_CfgRcu (void)
{
#if SPI1_DMA	
	rcu_periph_clock_enable(RCU_GPIOB); // SCK/PB13, MISO/PB14(not used), MOSI/PB15
	rcu_periph_clock_enable(RCU_SPI1);
	rcu_periph_clock_enable(RCU_DMA0);
#endif
}

static void SPI_CfgGpio (void)
{
#if SPI1_DMA
	/* SPI1 GPIO config: SCK/PB13, MISO/PB14(not used), MOSI/PB15 */
	uint32_t pin = GPIO_PIN_13 /*| GPIO_PIN_14 */ | GPIO_PIN_15;
	gpio_af_set(GPIOB, GPIO_AF_5, pin);
	gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, pin);
	gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, pin);
#endif
}

static void SPI_CfgDma (void)
{
	dma_single_data_parameter_struct dma_init_struct;

#if SPI1_DMA
	/* SPI1 transmit dma config: DMA0-DMA_CH4 */
	dma_deinit(DMA0, DMA_CH4);
	dma_init_struct.periph_addr   = (uint32_t)&SPI_DATA(SPI1);
	dma_init_struct.memory0_addr  = (uint32_t)g_spi1TxBuffer;
	dma_init_struct.direction     = DMA_MEMORY_TO_PERIPH;
	dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
	dma_init_struct.priority      = DMA_PRIORITY_LOW;
	dma_init_struct.number        = SPI1_TX_SIZE;
	dma_init_struct.periph_inc    = DMA_PERIPH_INCREASE_DISABLE;
	dma_init_struct.memory_inc    = DMA_MEMORY_INCREASE_ENABLE;
	dma_init_struct.circular_mode = DMA_CIRCULAR_MODE_DISABLE;
	dma_single_data_mode_init(DMA0, DMA_CH4, &dma_init_struct);
	/* configure DMA mode */
	dma_channel_subperipheral_select(DMA0, DMA_CH4, DMA_SUBPERI0);
#endif
}

static void SPI_Config (void)
{
	spi_parameter_struct spi_init_struct;

#if SPI1_DMA
	/* deinitilize SPI and the parameters */
	spi_i2s_deinit(SPI1);
	spi_struct_para_init(&spi_init_struct);

	/* SPI1 parameter config */
	spi_init_struct.trans_mode           = SPI_TRANSMODE_BDTRANSMIT; // SPI_TRANSMODE_FULLDUPLEX;
	spi_init_struct.device_mode          = SPI_MASTER;
	spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
	spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
	spi_init_struct.nss                  = SPI_NSS_SOFT; // SPI_NSS_HARD;
	spi_init_struct.prescale             = SPI_PSC_8;
	spi_init_struct.endian               = SPI_ENDIAN_MSB;
	spi_init(SPI1, &spi_init_struct);

//	spi_nss_output_enable(SPI1);

	/* SPI enable */
	spi_enable(SPI1);
#endif	

	dma_channel_enable(DMA0, DMA_CH4);
	spi_dma_enable(SPI1, SPI_DMA_TRANSMIT);
}

void SPI_UserInit (void)
{
	/* peripheral clock enable */
	SPI_CfgRcu();
	/* GPIO config */
	SPI_CfgGpio();
	/* DMA config */
	SPI_CfgDma();
	/* SPI config */
	SPI_Config();
}

void SPI_WriteOled (uint8_t *buf, uint16_t size)
{
#if SPI1_DMA
	dma_channel_disable(DMA0, DMA_CH4);
	dma_flag_clear(DMA0, DMA_CH4, DMA_FLAG_FTF);
	dma_memory_address_config(DMA0, DMA_CH4, DMA_MEMORY_0, (uint32_t)buf);
	dma_transfer_number_config(DMA0, DMA_CH4, size);
	dma_channel_enable(DMA0, DMA_CH4);
#else
	for (int i = 0; i < size; i++) {
		spi_i2s_data_transmit(SPI1, buf[i]);
		while(RESET == spi_i2s_flag_get(SPI1, SPI_FLAG_TBE));
	}
	for (int i = 0; i < 156; i++) {	// delay 25M -> 128, 30M -> 156
		__nop();
	}
#endif
}

void SPI_WaitOled (void)
{
	while (RESET == dma_flag_get(DMA0, DMA_CH4, DMA_FLAG_FTF));
	for (int i = 0; i < 156; i++) {	// delay 25M -> 128, 30M -> 156
		__nop();
	}
}
