# SPI 阻塞轮询 vs SPI+DMA（STM32G4）

## 为什么短帧下 DMA 可能更慢
- DMA 启动、寄存器配置、NVIC 进出栈有固定开销，1~4 字节在 40MHz 线上仅亚微秒。
- HAL 还要做状态检查与清标志，短帧时这些开销占主导。
- 每帧都启动 DMA等中断退出，帧长太短无法摊薄启动成本。
- 传输前后若有 printf/RTT、片选延时、HAL_GetTick 轮询等，都会算进测量时间。
- 若 DMA 与其他外设仲裁，同控制器竞争会再增加延迟。

## 何时选哪种方式
- 1~4 字节等极短寄存器读写：用阻塞/轮询更快。
- 长帧或需要释放 CPU：用 DMA。
- 若用 DMA，尽量在一次 CS 低电平内合并多字节，减少启动次数。

## 代码示例（简化版）

### 阻塞轮询收发
```c
HAL_StatusTypeDef spi_rw_blocking(uint8_t *tx, uint8_t *rx, uint16_t len)
{
    return HAL_SPI_TransmitReceive(&hspi1, tx, rx, len, 10); // 超时单位 ms
}
```

### DMA 收发（完成标志轮询）
```c
static volatile int8_t spi_dma_state = 0; // 0 进行中，1 完成，-1 错误

HAL_StatusTypeDef spi_rw_dma(uint8_t *tx, uint8_t *rx, uint16_t len)
{
    spi_dma_state = 0;
    if (HAL_SPI_TransmitReceive_DMA(&hspi1, tx, rx, len) != HAL_OK)
        return HAL_ERROR;

    uint32_t start = HAL_GetTick();
    while (spi_dma_state == 0) {
        if ((HAL_GetTick() - start) > 10) { // 10ms 软超时
            HAL_SPI_DMAStop(&hspi1);
            spi_dma_state = -1;
            return HAL_TIMEOUT;
        }
    }
    return (spi_dma_state > 0) ? HAL_OK : HAL_ERROR;
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi == &hspi1) spi_dma_state = 1;
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi == &hspi1) spi_dma_state = -1;
}
```

## 实战提示
- 保持 CS 低，合并多字节/多寄存器一次 DMA 发送，摊薄启动开销。
- 关键路径不要打印日志，放到片选释放之后。
- 超时按帧长调节，短帧可用几百微秒量级。
- 确保 SPI 所用 DMA 通道不与 UART/ADC 冲突，优先级合适。
- 用 DWT 只测线上时间，排除应用层延迟。
