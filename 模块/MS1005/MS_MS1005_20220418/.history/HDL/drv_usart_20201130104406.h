

#ifndef _DRV_USART_H
#define _DRV_USART_H

#include "board.h"






typedef struct
{
	uint8_t  *RxBuff;      // 接收缓存
	uint8_t  *MaxBuffAddr; // 最大地址，该地址不属于 Buff
	uint8_t  *FrameBuff;   // 单帧缓存地址，该存储空间最小需要等于最长数据帧长度
	uint16_t  BuffSize;    // 缓存大小
	uint16_t  Offset;      // 当前偏移
}DMA_InfoTypedef;


struct kfifo {     
    unsigned char *buffer;    /* the buffer holding the data */     
    unsigned int size;        /* the size of the allocated buffer */  
    unsigned int mask;        /* the size of the allocated buffer */     
    unsigned int in;          /* data is added at offset (in % size) */     
    unsigned int out;         /* data is extracted from off. (out % size) */         
}; 

#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))

void kfifo_init(struct kfifo *fifo, 
                uint8_t *buffer, uint32_t buff_size);
unsigned int __kfifo_get(struct kfifo *fifo, 
												unsigned char *buffer, unsigned int len);
unsigned int __kfifo_put(struct kfifo *fifo,     
                         const unsigned char *buffer, unsigned int len);





#endif

