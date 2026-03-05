

#include "spad_uart_driver.h"


#include "main.h"

// static void Get_SPAD_Point_1CH(PacketF_1CH_LiDAR *spad_paceket, SenPF* point);
// static void Get_SPAD_Point_2CH(PacketF_2CH_LiDAR *spad_paceket, SenPF* point);
void Get_SPAD_Point_1CH(SPAD_MSG_PACKET *spad_paceket);
void Get_SPAD_Point_2CH(SPAD_MSG_PACKET *spad_paceket);
//void SPAD_Parse_Uart_Rx(u8* buf, )
//{
//	;
//}

uint8_t Get_Data_XOR(uint8_t *buf, size_t size)
{
	uint8_t crc = 0;
	
	for (int i = 0; i < size; i++)
	{
		crc ^= buf[i];
	}
	
	return crc;
}

//
static PacketF_1CH_LiDAR spad_frame_packet_1CH = {0};
static PacketF_2CH_LiDAR spad_frame_packet_2CH = {0};
SenPF spad_point = {0 , 0};
SPAD_MSG_PACKET spad_frame_packet[2] = 
{
	{(uint32_t *)&spad_frame_packet_1CH.header, PackSize1, &spad_point, Get_SPAD_Point_1CH},
	{(uint32_t *)&spad_frame_packet_2CH.header, PackSize2, &spad_point, Get_SPAD_Point_2CH},
};
/*功能描述：解析SPAD回传封包数据 	
**输入参数：buf：待解析数据首地址
**输入参数：point：存放数据点的
**输入参数：data_len：待解析数据长度
**输入参数：packet_size: 结构体包大小
**输出参数：bresult: 解析结果 reset:解析失败； set :解析成功
**备注说明：v1.0
**存在BUG:若是buf数据结尾不是以spad_frame_packet的帧尾，会丢失解析完后剩余的数据
**/
uint8_t SPAD_Msg_Parsing(uint8_t* buf, SenPF* point, uint16_t data_len, uint16_t workmode)//size_t packet_size)
{
	uint16_t pos = 0;
	uint8_t bresult = RESET;
	uint16_t packet_size = spad_frame_packet[workmode].packet_size;

	if (data_len < packet_size)
	{
		return bresult;
	}
	
	while (data_len - pos > packet_size)
	{
		if (SPAD_PACKET_LIDAR_HEADER == buf[pos])//找到帧头
		{		
			break;
		}
		pos++;
	}
	
	//校验位正确
	if (buf[pos+packet_size-1] == Get_Data_XOR(buf+1, packet_size-2))
	{
		memcpy(spad_frame_packet[workmode].packet_buf, &buf[pos], packet_size);
		spad_frame_packet[workmode].getpoint(&spad_frame_packet[workmode]);

		point->dist = spad_frame_packet[workmode].scan_point->dist;
		point->engy = spad_frame_packet[workmode].scan_point->engy;
		bresult = SET; 
	}
	
	return bresult;
}

/*功能描述：解析SPAD回传封包数据 	
**输入参数：buf：待解析数据首地址
**输入参数：point：存放数据点的
**输入参数：datalen：待解析数据长度
**输入参数：packet_size: 结构体包大小
**输出参数：bresult: 解析结果 reset:解析失败； set :解析成功
**备注说明：v1.1 采用环形缓冲实现读取数据
**存在BUG:若是buf数据结尾不是以spad_frame_packet的帧尾，会丢失解析完后剩余的数据
**/
uint8_t SPAD_Msg_Parsing_V1_1(struct kfifo *fifo, SenPF* point, uint16_t workmode)//size_t packet_size)
{
	uint16_t pos = 0;
	uint8_t bresult = RESET;
	uint16_t packet_size = spad_frame_packet[workmode].packet_size;
	uint32_t data_len = 0;


	data_len = __kfifo_len(fifo);

	if (data_len < packet_size)
	{
		return bresult;
	}
	
	while
	while (data_len - pos > packet_size)
	{
		if (SPAD_PACKET_LIDAR_HEADER == buf[pos])//找到帧头
		{		
			break;
		}
		pos++;
	}
	
	//校验位正确
	if (buf[pos+packet_size-1] == Get_Data_XOR(buf+1, packet_size-2))
	{
		memcpy(spad_frame_packet[workmode].packet_buf, &buf[pos], packet_size);
		spad_frame_packet[workmode].getpoint(&spad_frame_packet[workmode]);

		point->dist = spad_frame_packet[workmode].scan_point->dist;
		point->engy = spad_frame_packet[workmode].scan_point->engy;
		bresult = SET; 
	}
	
	return bresult;
}
void Get_SPAD_Point_1CH(SPAD_MSG_PACKET *spad_paceket)
{
	spad_paceket->scan_point->dist = ((PacketF_1CH_LiDAR *)(spad_paceket->packet_buf))->data[0].dist;
	spad_paceket->scan_point->engy = ((PacketF_1CH_LiDAR *)(spad_paceket->packet_buf))->data[0].engy;
}

void Get_SPAD_Point_2CH(SPAD_MSG_PACKET *spad_paceket)
{
	int point_choose = 0;
	PacketF_2CH_LiDAR *tmp_spad_packet_2CH = (PacketF_2CH_LiDAR *)(spad_paceket->packet_buf);


	if (tmp_spad_packet_2CH->data[0].engy > skl100_params.engy_lower_limit 
		&& tmp_spad_packet_2CH->data[1].engy > skl100_params.engy_lower_limit)
	{
		//选取距离值较小的点		
		
		if (tmp_spad_packet_2CH->data[0].dist > tmp_spad_packet_2CH->data[1].dist)
		{
			point_choose = 1; //
		}
		else
		{	
			point_choose = 0;
		}
		
	}

	else if (tmp_spad_packet_2CH->data[0].engy > skl100_params.engy_lower_limit)
	{
		point_choose = 0;
	}
	else if (tmp_spad_packet_2CH->data[1].engy > skl100_params.engy_lower_limit)
	{
		point_choose = 1;
	}
	else//两个点强度都不满足
	{
		/* code */

	}

	spad_paceket->scan_point->dist = tmp_spad_packet_2CH->data[point_choose].dist;
	spad_paceket->scan_point->engy = tmp_spad_packet_2CH->data[point_choose].engy;
	
}


