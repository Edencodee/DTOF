
#include "bubbleSort.h"

int compare_uint32(const void *e1, const void *e2)
{
	return *(uint32_t *)e1 - *(uint32_t *)e2;
}

int compare_int_r(const void *e1, const void *e2)
{
	return *(uint32_t *)e2 - *(uint32_t *)e1 ;	
}

int compare_uint16(const void *e1, const void *e2)
{
	return *(uint16_t *)e1 - *(uint16_t *)e2;
}

int compare_uint8(const void *e1, const void *e2)
{
	return *(uint8_t *)e2 - *(uint8_t *)e1 ;	
}
int compare_str(const void *e1, const void *e2)
{
	return strcmp(*(char **)e1, *(char **)e2);
}

void byte_swap(void *pData1, void *pData2, size_t stSize)
{
	unsigned char *pcData1 = pData1;
	unsigned char *pcData2 = pData2;
	unsigned char ucTemp;
	
	while (stSize--){
		ucTemp = *pcData1; 
		*pcData1 = *pcData2; 
		*pcData2 = ucTemp;
		pcData1++; 
		pcData2++;
	}	
}

void bubbleSort(uint8_t *base, size_t nmemb, size_t size, COMPARE compare)
{
	int hasSwap=1;
	size_t i = 1;
	size_t j = 0;
	for (i = 1; hasSwap && i < nmemb; i++) {
		hasSwap = 0;
		for (j = 0; j < nmemb - i; j++) {
			void *pThis = ((unsigned char *)base) + size*j;
			void *pNext = ((unsigned char *)base) + size*(j+1);
			if (compare(pThis, pNext) > 0) {
				hasSwap = 1;
				byte_swap(pThis, pNext, size);
			}
		}
	}
}
/********************************************************************************************************
**函数信息 ：Struct_BubbleSort(void *base, size_t nmemb, size_t struct_size, 
									size_t pos, size_t size, 
									COMPARE compare)                  
**功能描述 : 对结构体数组中某一成员排序
**输入参数 ：*base ：
**输入参数 ：struct_size：结构体数组个数
**输入参数 ：pos:待排序成员在结构体的偏移量
**输入参数 ：size：待排序成员数据大小
**输入参数 ：compare：使用的比较函数
**输出参数 ：无
**备注     ：无
********************************************************************************************************/
void Struct_BubbleSort(void *base, size_t nmemb, size_t struct_size, 
									size_t pos, size_t size, 
									COMPARE compare)
{
	int hasSwap=1;
	size_t i = 1;
	size_t j = 0;
	for (i = 1; hasSwap && i < nmemb; i++) {
		hasSwap = 0;
		for (j = 0; j < nmemb - i; j++) {
			void *pThis = ((unsigned char *)base) + struct_size*j     + pos;
			void *pNext = ((unsigned char *)base) + struct_size*(j+1) + pos;
			if (compare(pThis, pNext) > 0) {
				hasSwap = 1;
				byte_swap(pThis - pos, pNext - pos, struct_size);
			}
		}
	}	
}
