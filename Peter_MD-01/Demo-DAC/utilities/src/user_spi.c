#include "user_spi.h"
#include "systick.h"

/* transmit variable */
#define SPI2_TX_SIZE	3
uint8_t g_spi2TxBuffer[SPI2_TX_SIZE] = {0xFC, 0x00, 0x00};

static void SPI_CfgRcu (void)
{	
	/* SPI2 */
	rcu_periph_clock_enable(RCU_GPIOC); // SCK/PC10, MISO/PC11, MOSI/PC12
	rcu_periph_clock_enable(RCU_DMA0);
	rcu_periph_clock_enable(RCU_SPI2);
	
	rcu_periph_clock_enable(SYNC_GPIO_CLK);
}

static void SPI_CfgGpio (void)
{
	/* SPI2 GPIO config: SCK/PC10, MISO/PC11, MOSI/PC12 */
	uint32_t pin = GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
	gpio_af_set(GPIOC, GPIO_AF_6, pin);
	gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, pin);
	gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, pin);

	/* SYNC config as output */
	gpio_mode_set(SYNC_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SYNC_PIN);
	gpio_output_options_set(SYNC_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, SYNC_PIN);
	GPIO_BOP(SYNC_PORT) = SYNC_PIN;
}

static void SPI_CfgDma (void)
{
	dma_single_data_parameter_struct dma_init_struct;

	/* SPI2 transmit dma config: DMA0-DMA_CH5 */
	dma_deinit(DMA0, DMA_CH5);
	dma_init_struct.periph_addr   = (uint32_t)&SPI_DATA(SPI2);
	dma_init_struct.memory0_addr  = (uint32_t)g_spi2TxBuffer;
	dma_init_struct.direction     = DMA_MEMORY_TO_PERIPH;
	dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
	dma_init_struct.priority      = DMA_PRIORITY_LOW;
	dma_init_struct.number        = SPI2_TX_SIZE;
	dma_init_struct.periph_inc    = DMA_PERIPH_INCREASE_DISABLE;
	dma_init_struct.memory_inc    = DMA_MEMORY_INCREASE_ENABLE;
	dma_init_struct.circular_mode = DMA_CIRCULAR_MODE_DISABLE;
	dma_single_data_mode_init(DMA0, DMA_CH5, &dma_init_struct);
	/* configure DMA mode */
	dma_channel_subperipheral_select(DMA0, DMA_CH5, DMA_SUBPERI0);
}

static void SPI_Config (void)
{
	spi_parameter_struct spi_init_struct;
	/* 1. SPI2 --------------------------------------------------- */
	/* deinitilize SPI and the parameters */
	spi_i2s_deinit(SPI2);
	spi_struct_para_init(&spi_init_struct);

	/* SPI2 parameter config */
	spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
	spi_init_struct.device_mode          = SPI_MASTER;
	spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
	spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_2EDGE;
	spi_init_struct.nss                  = SPI_NSS_SOFT;
	spi_init_struct.prescale             = SPI_PSC_8; // 200 / 8 = 25MHz, 240 / 8 = 30MHz
	spi_init_struct.endian               = SPI_ENDIAN_MSB;
	spi_init(SPI2, &spi_init_struct);
	
	/* SPI enable */
	spi_enable(SPI2);	

	dma_channel_enable(DMA0, DMA_CH5);
	spi_dma_enable(SPI2, SPI_DMA_TRANSMIT);
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

void SPI_WriteDAC(u16_t val)
{
	GPIO_BC(SYNC_PORT) = SYNC_PIN;
	g_spi2TxBuffer[1] = val.buf[kMsb];
	g_spi2TxBuffer[2] = val.buf[kLsb];
	
	dma_channel_disable(DMA0, DMA_CH5);
	dma_flag_clear(DMA0, DMA_CH5, DMA_FLAG_FTF);
//	dma_memory_address_config(DMA0, DMA_CH5, DMA_MEMORY_0, (uint32_t)g_spi2TxBuffer);
	dma_transfer_number_config(DMA0, DMA_CH5, SPI2_TX_SIZE);
	dma_channel_enable(DMA0, DMA_CH5);
	for (int i = 0; i < 156; i++) {	// delay 25M -> 128, 30M -> 156
		__nop();
	}
	GPIO_BOP(SYNC_PORT) = SYNC_PIN;
}
