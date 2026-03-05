


#include "drv_usart.h"
#include "usart.h"




DMA_InfoTypedef DMA_Info;


void DMA_InfoInit()
{
	DMA_Info.BuffSize    = RX_BUFF_MAX;
	DMA_Info.Offset      = 0;
	DMA_Info.RxBuff      = SPAD_Uart_Rx_Buffer;
  DMA_Info.MaxBuffAddr = &SPAD_Uart_Rx_Buffer[RX_BUFF_MAX];
	DMA_Info.FrameBuff   = SPAD_Uart_Rx_Data;
}


void kfifo_init(struct kfifo *fifo, 
                uint8_t *buffer, uint32_t buff_size)
{
    fifo->buffer = buffer;
    fifo->size   = buff_size; // 必须为 2 的幂次方，可通过一个 ((size - 1)&size) 判断  
    fifo->in     = 0;
    fifo->out    = 0;
		fifo->mask   = fifo->size - 1;
}

unsigned int __kfifo_put(struct kfifo *fifo,     
                         const unsigned char *buffer, unsigned int len)     
{     
    unsigned int l;     
    
    len = min(len, fifo->size - fifo->in + fifo->out);      
    
    /* first put the data starting from fifo->in to buffer end */     
    l = min(len, fifo->size - (fifo->in & fifo->mask));     
    memcpy(fifo->buffer + (fifo->in & fifo->mask), buffer, l);     
    
    /* then put the rest (if any) at the beginning of the buffer */     
    memcpy(fifo->buffer, buffer + l, len - l);         
    
    fifo->in += len;
    
    return len;     
} 


unsigned int __kfifo_get(struct kfifo *fifo, 
                         unsigned char *buffer, unsigned int len)     
{     
    unsigned int l;     
    
    len = min(len, fifo->in - fifo->out);     
   
    /* first get the data from fifo->out until the end of the buffer */     
    l = min(len, fifo->size - (fifo->out & fifo->mask));     
    memcpy(buffer, fifo->buffer + (fifo->out & fifo->mask), l);     
    
    /* then get the rest (if any) from the beginning of the buffer */     
    memcpy(buffer + l, fifo->buffer, len - l);        
    
    fifo->out += len;
        
    return len;     
}  

unsigned int __k





