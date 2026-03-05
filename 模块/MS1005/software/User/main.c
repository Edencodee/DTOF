#include "stm32f10x.h"	
#include "stdio.h"
#include <string.h>
#include "1003.h"
#include "W5500.h"

void RCC_Configuration(void);		//ÉèÖÃÏµÍ³Ê±ÖÓÎª72MHZ(Õâ¸ö¿ÉÒÔ¸ù¾ÝÐèÒª¸Ä)
void NVIC_Configuration(void);		//STM32ÖÐ¶ÏÏòÁ¿±íÅäÅäÖÃ
void Timer2_Init_Config(void);		//Timer2³õÊ¼»¯ÅäÖÃ
void System_Initialization(void);	//STM32ÏµÍ³³õÊ¼»¯º¯Êý(³õÊ¼»¯STM32Ê±ÖÓ¼°ÍâÉè)
void Delay(unsigned int d);			//ÑÓÊ±º¯Êý(ms)
void UART1_Init(void);					//STM32´®¿Ú³õÊ¼»¯
void MS1003_Initialization(void);
void MS1003_init(void);

void MS1003_START();
void MS1003_STOP();
void Read_data(void);
void CAL_PLUSE_1(void);
void CAL_PLUSE_2(void);
void CAL_PLUSE_3(void);
void CAL_PLUSE_4(void);
void CAL_PLUSE_5(void);
void CAL_PLUSE_6(void);
void CAL_PLUSE_7(void);
void CAL_PLUSE_8(void);
void CAL_PLUSE_9(void);
void CAL_PLUSE_10(void);
void NCAL_PLUSE_1(void);
void NCAL_PLUSE_2(void);
void NCAL_PLUSE_3(void);
void NCAL_PLUSE_4(void);
void NCAL_PLUSE_5(void);
void NCAL_PLUSE_6(void);
void NCAL_PLUSE_7(void);
void NCAL_PLUSE_8(void);
void NCAL_PLUSE_9(void);
void NCAL_PLUSE_10(void);


void MS1003_SPI1_Send_Byte(unsigned char dat);

unsigned int Timer2_Counter=0; //Timer2¶¨Ê±Æ÷¼ÆÊý±äÁ¿(ms)
unsigned int MS1003_Send_Delay_Counter=0;
unsigned char reg;
unsigned char USART1_RX_flag;



#if 1
#pragma import(__use_no_semihosting)             //±ê×¼¿âÐèÒªµÄÖ§³Öº¯Êý                 
struct __FILE {    int handle;  }; 
FILE __stdout;       //¶¨Òå_sys_exit()ÒÔ±ÜÃâÊ¹ÓÃ°ëÖ÷»úÄ£Ê½   
void _sys_exit(int x) {    x = x; }//ÖØ¶¨Òåfputcº¯Êý 
int fputc(int ch, FILE *f){          
while( !(USART1->SR & (1 << 7)) );   
USART_SendData(USART1,(uint8_t)ch);     
return ch;}

#endif 





 




/*******************************************************************************
* º¯ÊýÃû  : System_Initialization
* ÃèÊö    : STM32ÏµÍ³³õÊ¼»¯º¯Êý(³õÊ¼»¯STM32Ê±ÖÓ¼°ÍâÉè)
* ÊäÈë    : ÎÞ
* Êä³ö    : ÎÞ
* ·µ»Ø    : ÎÞ 
* ËµÃ÷    : ÎÞ
*******************************************************************************/

void System_Initialization(void)
{
	RCC_Configuration();		//ÉèÖÃÏµÍ³Ê±ÖÓÎª72MHZ(Õâ¸ö¿ÉÒÔ¸ù¾ÝÐèÒª¸Ä)
  NVIC_Configuration();		//STM32ÖÐ¶ÏÏòÁ¿±íÅäÅäÖÃ
	MS1003_SPI_Configuration();		//MS1003 SPI³õÊ¼»¯ÅäÖÃ(STM32 SPI1)
//	W5500_SPI_Configuration();		//W5500  SPI³õÊ¼»¯ÅäÖÃ(STM32 SPI2)
	Timer2_Init_Config();		//Timer2³õÊ¼»¯ÅäÖÃ
	MS1003_GPIO_Configuration();	//1003 GPIO³õÊ¼»¯ÅäÖÃ	
}

/*******************************************************************************
* º¯ÊýÃû  : RCC_Configuration
* ÃèÊö    : ÉèÖÃÏµÍ³Ê±ÖÓÎª72MHZ(Õâ¸ö¿ÉÒÔ¸ù¾ÝÐèÒª¸Ä)
* ÊäÈë    : ÎÞ
* Êä³ö    : ÎÞ
* ·µ»ØÖµ  : ÎÞ
* ËµÃ÷    : STM32F107xºÍSTM32F105xÏµÁÐMCUÓëSTM32F103xÏµÁÐMCUÊ±ÖÓÅäÖÃÓÐËù²»Í¬
*******************************************************************************/
void RCC_Configuration(void)
{
  ErrorStatus HSEStartUpStatus;               //Íâ²¿¸ßËÙÊ±ÖÓ(HSE)µÄ¹¤×÷×´Ì¬±äÁ¿
  
  RCC_DeInit();                               //½«ËùÓÐÓëÊ±ÖÓÏà¹ØµÄ¼Ä´æÆ÷ÉèÖÃÎªÄ¬ÈÏÖµ
  RCC_HSEConfig(RCC_HSE_ON);                  //Æô¶¯Íâ²¿¸ßËÙÊ±ÖÓHSE 
  HSEStartUpStatus = RCC_WaitForHSEStartUp(); //µÈ´ýÍâ²¿¸ßËÙÊ±ÖÓ(HSE)ÎÈ¶¨

  if(SUCCESS == HSEStartUpStatus)             //Èç¹ûÍâ²¿¸ßËÙÊ±ÖÓÒÑ¾­ÎÈ¶¨
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable); //FlashÉèÖÃ
    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
    
  
    RCC_HCLKConfig(RCC_SYSCLK_Div1); //ÉèÖÃAHBÊ±ÖÓµÈÓÚÏµÍ³Ê±ÖÓ(1·ÖÆµ)/72MHZ
    RCC_PCLK2Config(RCC_HCLK_Div1);  //ÉèÖÃAPB2Ê±ÖÓºÍHCLKÊ±ÖÓÏàµÈ/72MHz(×î´óÎª72MHz)
    RCC_PCLK1Config(RCC_HCLK_Div2);  //ÉèÖÃAPB1Ê±ÖÓÊÇHCLKÊ±ÖÓµÄ2·ÖÆµ/36MHz(×î´óÎª36MHz)
  
//#ifndef STM32F10X_CL                 //Èç¹ûÊ¹ÓÃµÄ²»ÊÇSTM32F107x»òSTM32F105xÏµÁÐMCU,PLLÒÔÏÂÅäÖÃ  
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); //PLLCLK = 8MHz * 9 = 72 MHz 
//#else                                //Èç¹ûÊ¹ÓÃµÄÊÇSTM32F107x»òSTM32F105xÏµÁÐMCU,PLLÒÔÏÂÅäÖÃ
    /***** ÅäÖÃPLLx *****/
    /* PLL2 configuration: PLL2CLK = (HSE / 5) * 8 = 40 MHz */
 /*   RCC_PREDIV2Config(RCC_PREDIV2_Div5);
    RCC_PLL2Config(RCC_PLL2Mul_8);

    RCC_PLL2Cmd(ENABLE); //Ê¹ÄÜPLL2 
    while (RCC_GetFlagStatus(RCC_FLAG_PLL2RDY) == RESET);//µÈ´ýPLL2ÎÈ¶¨
*/
    /* PLL configuration: PLLCLK = (PLL2 / 5) * 9 = 72 MHz */ 
   /* RCC_PREDIV1Config(RCC_PREDIV1_Source_PLL2, RCC_PREDIV1_Div5);
    RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);
#endif*/

    RCC_PLLCmd(ENABLE); //Ê¹ÄÜPLL
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET); //µÈ´ýPLLÎÈ¶¨

    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);          //ÉèÖÃÏµÍ³Ê±ÖÓµÄÊ±ÖÓÔ´ÎªPLL

    while(RCC_GetSYSCLKSource() != 0x08);               //¼ì²éÏµÍ³µÄÊ±ÖÓÔ´ÊÇ·ñÊÇPLL
    RCC_ClockSecuritySystemCmd(ENABLE);                 //Ê¹ÄÜÏµÍ³°²È«Ê±ÖÓ 

	/* Enable peripheral clocks --------------------------------------------------*/
  	/* Enable I2C1 and I2C1 clock */
 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  	/* Enable GPIOA GPIOB SPI1 and USART1 clocks */
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB
					| RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD
					| RCC_APB2Periph_AFIO, ENABLE);    
  }
}
/*******************************************************************************
* º¯ÊýÃû  : NVIC_Configuration
* ÃèÊö    : STM32ÖÐ¶ÏÏòÁ¿±íÅäÅäÖÃ
* ÊäÈë    : ÎÞ
* Êä³ö    : ÎÞ
* ·µ»ØÖµ  : ÎÞ
* ËµÃ÷    : ÉèÖÃKEY1(PC11)µÄÖÐ¶ÏÓÅÏÈ×é
*******************************************************************************/
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;						//¶¨ÒåNVIC³õÊ¼»¯½á¹¹Ìå

  	/* Set the Vector Table base location at 0x08000000 */
  	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);				//ÉèÖÃÖÐ¶ÏÓÅÏÈ¼¶×éÎª1£¬ÓÅÏÈ×é(¿ÉÉè0¡«4Î»)
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;				//ÉèÖÃÖÐ¶ÏÏòÁ¿ºÅ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;	//ÉèÖÃÇÀÏÈÓÅÏÈ¼¶
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;			//ÉèÖÃÏìÓ¦ÓÅÏÈ¼¶
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//Ê¹ÄÜNVIC
	NVIC_Init(&NVIC_InitStructure);
	
	MS1003_NVIC_Configuration();
}

/*******************************************************************************
* º¯ÊýÃû  : Timer2_Init_Config
* ÃèÊö    : Timer2³õÊ¼»¯ÅäÖÃ
* ÊäÈë    : ÎÞ
* Êä³ö    : ÎÞ
* ·µ»Ø    : ÎÞ 
* ËµÃ÷    : ÎÞ
*******************************************************************************/
void Timer2_Init_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);		//Ê¹ÄÜTimer2Ê±ÖÓ
	
	TIM_TimeBaseStructure.TIM_Period = 9;						//ÉèÖÃÔÚÏÂÒ»¸ö¸üÐÂÊÂ¼þ×°Èë»î¶¯µÄ×Ô¶¯ÖØ×°ÔØ¼Ä´æÆ÷ÖÜÆÚµÄÖµ(¼ÆÊýµ½10Îª1ms)
	TIM_TimeBaseStructure.TIM_Prescaler = 7199;					//ÉèÖÃÓÃÀ´×÷ÎªTIMxÊ±ÖÓÆµÂÊ³ýÊýµÄÔ¤·ÖÆµÖµ(10KHzµÄ¼ÆÊýÆµÂÊ)
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//ÉèÖÃÊ±ÖÓ·Ö¸î:TDTS = TIM_CKD_DIV1
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//TIMÏòÉÏ¼ÆÊýÄ£Ê½
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);				//¸ù¾ÝTIM_TimeBaseInitStructÖÐÖ¸¶¨µÄ²ÎÊý³õÊ¼»¯TIMxµÄÊ±¼ä»ùÊýµ¥Î»
	 
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE ); 				//Ê¹ÄÜTIM2Ö¸¶¨µÄÖÐ¶Ï
	
	TIM_Cmd(TIM2, ENABLE);  									//Ê¹ÄÜTIMxÍâÉè
}

/*******************************************************************************
* º¯ÊýÃû  : TIM2_IRQHandler
* ÃèÊö    : ¶¨Ê±Æ÷2ÖÐ¶Ï¶Ï·þÎñº¯Êý
* ÊäÈë    : ÎÞ
* Êä³ö    : ÎÞ
* ·µ»Ø    : ÎÞ 
* ËµÃ÷    : ÎÞ
*******************************************************************************/
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		Timer2_Counter++;
		MS1003_Send_Delay_Counter++;
	}
}
/*******************************************************************************
* º¯ÊýÃû  : UART1_Init
* ÃèÊö    : STM32´®¿Ú³õÊ¼»¯ÅäÖÃ(STM32 SPI1)
* ÊäÈë    : ÎÞ
* Êä³ö    : ÎÞ
* ·µ»ØÖµ  : ÎÞ
* ËµÃ÷    : ÎÞ
*******************************************************************************/

void UART1_Init(void)
{
GPIO_InitTypeDef GPIO_InitStructure;
USART_InitTypeDef USART_InitStructure;
NVIC_InitTypeDef  NVIC_InitStructure;
RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); //UART1
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //	
//UART1_TX   GPIOA_Pin_9
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //GPIOA_Pin_9
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //¸´ÓÃÍÆÃâÊä³ö
GPIO_Init(GPIOA, &GPIO_InitStructure);                    //ÅäÖÃTX¿Ú
//UART1_RX      GPIOA_Pin_10
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //GPIOA_Pin_10
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//¸¡¿ÕÊäÈë
GPIO_Init(GPIOA, &GPIO_InitStructure);									//ÅäÖÃRX¿Ú
//UART1Í¨Ñ¶²ÎÊýÅäÖÃ
USART_InitStructure.USART_BaudRate = 115200;                  //Í¨Ñ¶²¨ÌØÂÊ
USART_InitStructure.USART_WordLength = USART_WordLength_8b;   //8¸öÊý¾ÝÎ»
USART_InitStructure.USART_StopBits = USART_StopBits_1;        //1¸öÍ£Ö¹Î»
USART_InitStructure.USART_Parity = USART_Parity_No;
USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //
USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;       //
USART_Init(USART1, &USART_InitStructure);
//Uart1 NVIC  ÖÐ¶ÏÅäÖÃ
NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;       //IRQ
USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);          //Êý¾Ý½ÓÊÕÖÐ¶ÏÊ¹ÄÜ
NVIC_Init(&NVIC_InitStructure);
USART_Cmd(USART1, ENABLE);
}
/*******************************************************************************
* º¯ÊýÃû  : Delay
* ÃèÊö    : ÑÓÊ±º¯Êý(ms)
* ÊäÈë    : d:ÑÓÊ±ÏµÊý£¬µ¥Î»ÎªºÁÃë
* Êä³ö    : ÎÞ
* ·µ»Ø    : ÎÞ 
* ËµÃ÷    : ÑÓÊ±ÊÇÀûÓÃTimer2¶¨Ê±Æ÷²úÉúµÄ1ºÁÃëµÄ¼ÆÊýÀ´ÊµÏÖµÄ
*******************************************************************************/
void Delay(unsigned int d)
{
	Timer2_Counter=0; 
	while(Timer2_Counter < d);
}

void USART1_IRQHandler(void)                    //´®¿Ú1ÖÐ¶Ï·þÎñ³ÌÐò
 {
     unsigned char  Res;
     if(USART_GetITStatus(USART1, USART_IT_RXNE) ) 
         {
         //    Res =USART_ReceiveData(USART1);    //¶ÁÈ¡½ÓÊÕµ½µÄÊý¾Ý     
          //   if(Res == 0x55)//¶ÔÓÚ½ÓÊÕÖ¸¶¨³¤¶ÈµÄ×Ö·û´®
           //  {
                 USART1_RX_flag=1;        //¼ÇÂ¼½ÓÊÕµ½µÄÖµ  
					   Res= USART_ReceiveData(USART1); //Îª1£¬Ôò½«´®¿Ú1µÄÊý¾Ý¸ø±äÁ¿
							USART_SendData(USART1,Res);   //¼ÌÐø½«´ËÊý¾Ý´ÓUSART1·¢ËÍ³öÈ¥
           //  }             
      }
          //Òç³ö-Èç¹û·¢ÉúÒç³öÐèÒªÏÈ¶ÁSR,ÔÙ¶ÁDR¼Ä´æÆ÷Ôò¿ÉÇå³ý²»¶ÏÈëÖÐ¶ÏµÄÎÊÌâ
/*     if(USART_GetFlagStatus(USART1,USART_FLAG_ORE) == SET)
     {
         USART_ReceiveData(USART1);
         USART_ClearFlag(USART1,USART_FLAG_ORE);
     }*/
      USART_ClearFlag(USART1,USART_IT_RXNE); //Ò»¶¨ÒªÇå³ý½ÓÊÕÖÐ¶Ï
 }


 

unsigned char INTN_fig(int delay)
{
 while(Intn_flag==1)
 { 
  delay--;
  if(!delay)
  {
   return 1;
  }
 // Delay(1);  
 }
 return 0;
}

/*Ð£×¼Ä£Ê½Ô¤ÆÚÒ»Âö³å£¬Êµ¼ÊÒ»Âö³å*/

void CAL_PLUSE_1()
{
				float  i,i1;
				unsigned char k;
				Write_MS1003_1Byte_Addr(ADDR1); //MS1003Ð´¼Ä´æÆ÷µØÖ·
				MS1003_SPI1_Send_Byte(0x02);
				MS1003_SPI1_Send_Byte(0x3a);
				MS1003_SPI1_Send_Byte(0x60);
				MS1003_SPI1_Send_Byte(0x10);
				GPIO_SetBits(MS1003_SCS_PORT, MS1003_SCS); //ÖÃMS1003µÄSCSÎª¸ßµçÆ½
				Intn_flag = 1;

						while(1)
						{
							//	i1 = Read_MS1003_NCAL_3Byte(0xbC);
						Intn_flag = 1;
						Write_MS1003_1Byte(Init);	
						MS1003_START();
						MS1003_STOP();
						key_flag = 0;
							if(!INTN_fig(50))	
							{
							printf("jiaozhun:");	
							i1 = Read_MS1003_CAL_3Byte(0xb0) * 1000;
							i = Read_MS1003_NCAL_3Byte(0xbd); 
				//			k =	Read_MS1003_PLUSE_3Byte();																
							printf("   state:");	
							printf("%.0lf",i);	
							printf("   reg0:");	
							printf("%.2lf",i1);		
							printf("\r\n");
							}	
							if (key_flag == 1)
							{CAL_PLUSE_2();}
						}
}
/*Ð£×¼Ä£Ê½Ô¤ÆÚÁ½Âö³å£¬Êµ¼ÊÁ½Âö³å*/
void CAL_PLUSE_2()
{
				float  i,i1,i2;
				unsigned char k;
				Write_MS1003_1Byte_Addr(ADDR1); //MS1003Ð´¼Ä´æÆ÷µØÖ·
				MS1003_SPI1_Send_Byte(0x03);
				MS1003_SPI1_Send_Byte(0x3a);
				MS1003_SPI1_Send_Byte(0x60);
				MS1003_SPI1_Send_Byte(0x11);
				GPIO_SetBits(MS1003_SCS_PORT, MS1003_SCS); //ÖÃMS1003µÄSCSÎª¸ßµçÆ½
				Intn_flag = 1;
						while(1)
						{
						Intn_flag = 1;
						Write_MS1003_1Byte(Init);	
						MS1003_START();
						MS1003_STOP();
						key_flag = 0;
							if(!INTN_fig(50))	
							{
							printf("jiaozhun:");	
							i1 = Read_MS1003_CAL_3Byte(0xb0) * 1000;
							i2 = Read_MS1003_CAL_3Byte(0xb1) * 1000;	
							i = Read_MS1003_NCAL_3Byte(0xbd); 
							k =	Read_MS1003_PLUSE_3Byte();																									
							printf("   state:");	
							printf("%.0lf",i);	
							printf("   reg0:");	
							printf("%.2lf",i1);	
							printf("  reg1:");								
							printf("%.2lf",i2);	
							printf("\r\n");
							}	
							if (key_flag == 1)
							{CAL_PLUSE_3();}
						}
}
/*Ð£×¼Ä£Ê½Ô¤ÆÚÈýÂö³å£¬Êµ¼ÊÈýÂö³å*/
void CAL_PLUSE_3()
{
	float  i,i1,i2,i3;
	unsigned char k;
	Write_MS1003_1Byte_Addr(ADDR1); //MS1003Ð´¼Ä´æÆ÷µØÖ·
MS1003_SPI1_Send_Byte(0x04);
				MS1003_SPI1_Send_Byte(0x3a);
				MS1003_SPI1_Send_Byte(0x60);
				MS1003_SPI1_Send_Byte(0x11);
				GPIO_SetBits(MS1003_SCS_PORT, MS1003_SCS); //ÖÃMS1003µÄSCSÎª¸ßµçÆ½
				Intn_flag = 1;
						while(1)
						{
						Intn_flag = 1;
						Write_MS1003_1Byte(Init);	
						MS1003_START();
						MS1003_STOP();
						MS1003_STOP();	
							key_flag = 0;
							if(!INTN_fig(50))	
							{
							printf("jiaozhun:");	
							i1 = Read_MS1003_CAL_3Byte(0xb0) * 1000;
							i2 = Read_MS1003_CAL_3Byte(0xb1) * 1000;
							i3 = Read_MS1003_CAL_3Byte(0xb2) * 1000;								
							i = Read_MS1003_NCAL_3Byte(0xbd); 							
							k =	Read_MS1003_PLUSE_3Byte();																								
							printf("   state:");	
							printf("%.0lf",i);	
							printf("   reg0:");	
							printf("%.2lf",i1);	
							printf("  reg1:");								
							printf("%.2lf",i2);
							printf("  reg2:");								
							printf("%.2lf",i3);								
							printf("\r\n");
							}	
								if (key_flag == 1)
							{CAL_PLUSE_4();}
						}
}
/*Ð£×¼Ä£Ê½Ô¤ÆÚËÄÂö³å£¬Êµ¼ÊËÄÂö³å*/
void CAL_PLUSE_4()
{
	float  i,i1,i2,i3,i4;
	unsigned char k;
	Write_MS1003_1Byte_Addr(ADDR1); //MS1003Ð´¼Ä´æÆ÷µØÖ·
MS1003_SPI1_Send_Byte(0x05);
				MS1003_SPI1_Send_Byte(0x3a);
				MS1003_SPI1_Send_Byte(0x60);
				MS1003_SPI1_Send_Byte(0x11);
				GPIO_SetBits(MS1003_SCS_PORT, MS1003_SCS); //ÖÃMS1003µÄSCSÎª¸ßµçÆ½
				Intn_flag = 1;
						while(1)
						{
						Intn_flag = 1;
						Write_MS1003_1Byte(Init);	
						MS1003_START();
						MS1003_STOP();
						MS1003_STOP();	
							key_flag = 0;
							if(!INTN_fig(50))	
							{
								printf("jiaozhun:");
							i1 = Read_MS1003_CAL_3Byte(0xb0) * 1000;
							i2 = Read_MS1003_CAL_3Byte(0xb1) * 1000;
							i3 = Read_MS1003_CAL_3Byte(0xb2) * 1000;
							i4 = Read_MS1003_CAL_3Byte(0xb3) * 1000;								
							i = Read_MS1003_NCAL_3Byte(0xbd); 
							k =	Read_MS1003_PLUSE_3Byte();									
							printf("   state:");	
							printf("%.0lf",i);	
							printf("   reg0:");	
							printf("%.2lf",i1);	
							printf("  reg1:");								
							printf("%.2lf",i2);
							printf("  reg2:");								
							printf("%.2lf",i3);	
							printf("  reg3:");								
							printf("%.2lf",i4);	
							printf("\r\n");
							}	
								if (key_flag == 1)
							{CAL_PLUSE_5();}
						}
}
/*Ð£×¼Ä£Ê½Ô¤ÆÚÎåÂö³å£¬Êµ¼ÊÎåÂö³å*/
void CAL_PLUSE_5()
{
	float  i,i1,i2,i3,i4,i5;
	unsigned char k;
	Write_MS1003_1Byte_Addr(ADDR1); //MS1003Ð´¼Ä´æÆ÷µØÖ·
MS1003_SPI1_Send_Byte(0x06);
				MS1003_SPI1_Send_Byte(0x3a);
				MS1003_SPI1_Send_Byte(0x60);
				MS1003_SPI1_Send_Byte(0x11);
				GPIO_SetBits(MS1003_SCS_PORT, MS1003_SCS); //ÖÃMS1003µÄSCSÎª¸ßµçÆ½
				Intn_flag = 1;
						while(1)
						{
						Intn_flag = 1;
						Write_MS1003_1Byte(Init);	
						MS1003_START();
						MS1003_STOP();
						MS1003_STOP();
						MS1003_STOP();							
							key_flag = 0;
							if(!INTN_fig(50))	
							{
								printf("jiaozhun:");
							i1 = Read_MS1003_CAL_3Byte(0xb0) * 1000;
							i2 = Read_MS1003_CAL_3Byte(0xb1) * 1000;
							i3 = Read_MS1003_CAL_3Byte(0xb2) * 1000;
							i4 = Read_MS1003_CAL_3Byte(0xb3) * 1000;
							i5 = Read_MS1003_CAL_3Byte(0xb4) * 1000;									
							i = Read_MS1003_NCAL_3Byte(0xbd); 
							k =	Read_MS1003_PLUSE_3Byte();									
							printf("   state:");	
							printf("%.0lf",i);	
							printf("   reg0:");	
							printf("%.2lf",i1);	
							printf("  reg1:");								
							printf("%.2lf",i2);
							printf("  reg2:");								
							printf("%.2lf",i3);	
							printf("  reg3:");								
							printf("%.2lf",i4);	
							printf("  reg4:");								
							printf("%.2lf",i5);	
							printf("\r\n");
							}	
								if (key_flag == 1)
							{CAL_PLUSE_6();}
						}
}

/*Ð£×¼Ä£Ê½Ô¤ÆÚÁùÂö³å£¬Êµ¼ÊÁùÂö³å*/
void CAL_PLUSE_6()
{
	float  i,i1,i2,i3,i4,i5,i6;
	unsigned char k;
	Write_MS1003_1Byte_Addr(ADDR1); //MS1003Ð´¼Ä´æÆ÷µØÖ·
MS1003_SPI1_Send_Byte(0x07);
				MS1003_SPI1_Send_Byte(0x3a);
				MS1003_SPI1_Send_Byte(0x60);
				MS1003_SPI1_Send_Byte(0x11);
				GPIO_SetBits(MS1003_SCS_PORT, MS1003_SCS); //ÖÃMS1003µÄSCSÎª¸ßµçÆ½
				Intn_flag = 1;
						while(1)
						{
						Intn_flag = 1;
						Write_MS1003_1Byte(Init);	
						MS1003_START();
						MS1003_STOP();
						MS1003_STOP();
						MS1003_STOP();	
							key_flag = 0;
							if(!INTN_fig(50))	
							{
								printf("jiaozhun:");
							i1 = Read_MS1003_CAL_3Byte(0xb0) * 1000;
							i2 = Read_MS1003_CAL_3Byte(0xb1) * 1000;
							i3 = Read_MS1003_CAL_3Byte(0xb2) * 1000;
							i4 = Read_MS1003_CAL_3Byte(0xb3) * 1000;
							i5 = Read_MS1003_CAL_3Byte(0xb4) * 1000;		
							i6 = Read_MS1003_CAL_3Byte(0xb5) * 1000;	
							i = Read_MS1003_NCAL_3Byte(0xbd); 
							k =	Read_MS1003_PLUSE_3Byte();									
							printf("   state:");	
							printf("%.0lf",i);	
							printf("   reg0:");	
							printf("%.2lf",i1);	
							printf("  reg1:");								
							printf("%.2lf",i2);
							printf("  reg2:");								
							printf("%.2lf",i3);	
							printf("  reg3:");								
							printf("%.2lf",i4);	
							printf("  reg4:");								
							printf("%.2lf",i5);
							printf("  reg5:");								
							printf("%.2lf",i6);	
							printf("\r\n");
							}	
								if (key_flag == 1)
							{CAL_PLUSE_7();}
						}
}

/*Ð£×¼Ä£Ê½Ô¤ÆÚÆßÂö³å£¬Êµ¼ÊÆßÂö³å*/
void CAL_PLUSE_7()
{
	float  i,i1,i2,i3,i4,i5,i6,i7;
	unsigned char k;
	Write_MS1003_1Byte_Addr(ADDR1); //MS1003Ð´¼Ä´æÆ÷µØÖ·
MS1003_SPI1_Send_Byte(0x08);
				MS1003_SPI1_Send_Byte(0x3a);
				MS1003_SPI1_Send_Byte(0x60);
				MS1003_SPI1_Send_Byte(0x11);
				GPIO_SetBits(MS1003_SCS_PORT, MS1003_SCS); //ÖÃMS1003µÄSCSÎª¸ßµçÆ½
				Intn_flag = 1;
						while(1)
						{
						Intn_flag = 1;
						Write_MS1003_1Byte(Init);	
						MS1003_START();
						MS1003_STOP();
						MS1003_STOP();	
						MS1003_STOP();
						MS1003_STOP();	
							key_flag = 0;
							if(!INTN_fig(100))	
							{
								printf("jiaozhun:");
							i1 = Read_MS1003_CAL_3Byte(0xb0) * 1000;
							i2 = Read_MS1003_CAL_3Byte(0xb1) * 1000;
							i3 = Read_MS1003_CAL_3Byte(0xb2) * 1000;
							i4 = Read_MS1003_CAL_3Byte(0xb3) * 1000;
							i5 = Read_MS1003_CAL_3Byte(0xb4) * 1000;		
							i6 = Read_MS1003_CAL_3Byte(0xb5) * 1000;
							i7 = Read_MS1003_CAL_3Byte(0xb6) * 1000;	
							i = Read_MS1003_NCAL_3Byte(0xbd); 
							k =	Read_MS1003_PLUSE_3Byte();									
							printf("   state:");	
							printf("%.0lf",i);	
							printf("   reg0:");	
							printf("%.2lf",i1);	
							printf("  reg1:");								
							printf("%.2lf",i2);
							printf("  reg2:");								
							printf("%.2lf",i3);	
							printf("  reg3:");								
							printf("%.2lf",i4);	
							printf("  reg4:");								
							printf("%.2lf",i5);
							printf("  reg5:");								
							printf("%.2lf",i6);
							printf("  reg6:");								
							printf("%.2lf",i7);	
							printf("\r\n");
							}	
								if (key_flag == 1)
							{CAL_PLUSE_8();}
						}
}

/*Ð£×¼Ä£Ê½Ô¤ÆÚ°ËÂö³å£¬Êµ¼Ê°ËÂö³å*/
void CAL_PLUSE_8()
{
	float  i,i1,i2,i3,i4,i5,i6,i7,i8;
	unsigned char k;
	Write_MS1003_1Byte_Addr(ADDR1); //MS1003Ð´¼Ä´æÆ÷µØÖ·
MS1003_SPI1_Send_Byte(0x09);
				MS1003_SPI1_Send_Byte(0x3a);
				MS1003_SPI1_Send_Byte(0x60);
				MS1003_SPI1_Send_Byte(0x11);
				GPIO_SetBits(MS1003_SCS_PORT, MS1003_SCS); //ÖÃMS1003µÄSCSÎª¸ßµçÆ½
				Intn_flag = 1;
						while(1)
						{
						Intn_flag = 1;
						Write_MS1003_1Byte(Init);	
						MS1003_START();
						MS1003_STOP();
						MS1003_STOP();
						MS1003_STOP();
						MS1003_STOP();	
							key_flag = 0;
							if(!INTN_fig(100))	
							{
								printf("jiaozhun:");
							i1 = Read_MS1003_CAL_3Byte(0xb0) * 1000;
							i2 = Read_MS1003_CAL_3Byte(0xb1) * 1000;
							i3 = Read_MS1003_CAL_3Byte(0xb2) * 1000;
							i4 = Read_MS1003_CAL_3Byte(0xb3) * 1000;
							i5 = Read_MS1003_CAL_3Byte(0xb4) * 1000;		
							i6 = Read_MS1003_CAL_3Byte(0xb5) * 1000;
							i7 = Read_MS1003_CAL_3Byte(0xb6) * 1000;	
							i8 = Read_MS1003_CAL_3Byte(0xb7) * 1000;		
							i = Read_MS1003_NCAL_3Byte(0xbd); 
							k =	Read_MS1003_PLUSE_3Byte();									
							printf("   state:");	
							printf("%.0lf",i);	
							printf("   reg0:");	
							printf("%.2lf",i1);	
							printf("  reg1:");								
							printf("%.2lf",i2);
							printf("  reg2:");								
							printf("%.2lf",i3);	
							printf("  reg3:");								
							printf("%.2lf",i4);	
							printf("  reg4:");								
							printf("%.2lf",i5);
							printf("  reg5:");								
							printf("%.2lf",i6);
							printf("  reg6:");								
							printf("%.2lf",i7);
							printf("  reg7:");								
							printf("%.2lf",i8);
							printf("\r\n");
							}	
								if (key_flag == 1)
							{CAL_PLUSE_9();}
						}
}

/*Ð£×¼Ä£Ê½Ô¤ÆÚ¾ÅÂö³å£¬Êµ¼Ê¾ÅÂö³å*/
void CAL_PLUSE_9()
{
	float  i,i1,i2,i3,i4,i5,i6,i7,i8,i9;
	unsigned char k;
	Write_MS1003_1Byte_Addr(ADDR1); //MS1003Ð´¼Ä´æÆ÷µØÖ·
MS1003_SPI1_Send_Byte(0x0a);
				MS1003_SPI1_Send_Byte(0x3a);
				MS1003_SPI1_Send_Byte(0x60);
				MS1003_SPI1_Send_Byte(0x11);
				GPIO_SetBits(MS1003_SCS_PORT, MS1003_SCS); //ÖÃMS1003µÄSCSÎª¸ßµçÆ½
				Intn_flag = 1;
						while(1)
						{
						Intn_flag = 1;
						Write_MS1003_1Byte(Init);	
						MS1003_START();
						MS1003_STOP();
						MS1003_STOP();
						MS1003_STOP();
						MS1003_STOP();
						MS1003_STOP();	
							key_flag = 0;
							if(!INTN_fig(100))	
							{
								printf("jiaozhun:");
							i1 = Read_MS1003_CAL_3Byte(0xb0) * 1000;
							i2 = Read_MS1003_CAL_3Byte(0xb1) * 1000;
							i3 = Read_MS1003_CAL_3Byte(0xb2) * 1000;
							i4 = Read_MS1003_CAL_3Byte(0xb3) * 1000;
							i5 = Read_MS1003_CAL_3Byte(0xb4) * 1000;		
							i6 = Read_MS1003_CAL_3Byte(0xb5) * 1000;
							i7 = Read_MS1003_CAL_3Byte(0xb6) * 1000;	
							i8 = Read_MS1003_CAL_3Byte(0xb7) * 1000;	
							i9 = Read_MS1003_CAL_3Byte(0xb8) * 1000;	
							i = Read_MS1003_NCAL_3Byte(0xbd); 
							k =	Read_MS1003_PLUSE_3Byte();									
							printf("   state:");	
							printf("%.0lf",i);	
							printf("   reg0:");	
							printf("%.2lf",i1);	
							printf("  reg1:");								
							printf("%.2lf",i2);
							printf("  reg2:");								
							printf("%.2lf",i3);	
							printf("  reg3:");								
							printf("%.2lf",i4);	
							printf("  reg4:");								
							printf("%.2lf",i5);
							printf("  reg5:");								
							printf("%.2lf",i6);
							printf("  reg6:");								
							printf("%.2lf",i7);
							printf("  reg7:");								
							printf("%.2lf",i8);
							printf("  reg8:");								
							printf("%.2lf",i9);
							printf("\r\n");
							}	
								if (key_flag == 1)
							{CAL_PLUSE_10();}
						}
}

/*Ð£×¼Ä£Ê½Ô¤ÆÚÊ®Âö³å£¬Êµ¼ÊÊ®Âö³å*/
void CAL_PLUSE_10()
{
	float  i,i1,i2,i3,i4,i5,i6,i7,i8,i9,i10;
	unsigned char k;
	Write_MS1003_1Byte_Addr(ADDR1); //MS1003Ð´¼Ä´æÆ÷µØÖ·
MS1003_SPI1_Send_Byte(0x0b);
				MS1003_SPI1_Send_Byte(0x3a);
				MS1003_SPI1_Send_Byte(0x60);
				MS1003_SPI1_Send_Byte(0x11);
				GPIO_SetBits(MS1003_SCS_PORT, MS1003_SCS); //ÖÃMS1003µÄSCSÎª¸ßµçÆ½
				Intn_flag = 1;
						while(1)
						{
						Intn_flag = 1;
						Write_MS1003_1Byte(Init);	
						MS1003_START();
						MS1003_STOP();
						MS1003_STOP();	
						MS1003_STOP();
						MS1003_STOP();
						MS1003_STOP();							
							key_flag = 0;
							if(!INTN_fig(100))	
							{
								printf("jiaozhun:");
							i1 = Read_MS1003_CAL_3Byte(0xb0) * 1000;
							i2 = Read_MS1003_CAL_3Byte(0xb1) * 1000;
							i3 = Read_MS1003_CAL_3Byte(0xb2) * 1000;
							i4 = Read_MS1003_CAL_3Byte(0xb3) * 1000;
							i5 = Read_MS1003_CAL_3Byte(0xb4) * 1000;		
							i6 = Read_MS1003_CAL_3Byte(0xb5) * 1000;
							i7 = Read_MS1003_CAL_3Byte(0xb6) * 1000;	
							i8 = Read_MS1003_CAL_3Byte(0xb7) * 1000;	
							i9 = Read_MS1003_CAL_3Byte(0xb8) * 1000;	
							i10 = Read_MS1003_CAL_3Byte(0xb9) * 1000;	
							i = Read_MS1003_NCAL_3Byte(0xbd); 
							k =	Read_MS1003_PLUSE_3Byte();									
							printf("   state:");	
							printf("%.0lf",i);	
							printf("   reg0:");	
							printf("%.2lf",i1);	
							printf("  reg1:");								
							printf("%.2lf",i2);
							printf("  reg2:");								
							printf("%.2lf",i3);	
							printf("  reg3:");								
							printf("%.2lf",i4);	
							printf("  reg4:");								
							printf("%.2lf",i5);
							printf("  reg5:");								
							printf("%.2lf",i6);
							printf("  reg6:");								
							printf("%.2lf",i7);
							printf("  reg7:");								
							printf("%.2lf",i8);
							printf("  reg8:");								
							printf("%.2lf",i9);
							printf("  reg9:");								
							printf("%.2lf",i10);
							printf("\r\n");
							}	
								if (key_flag == 1)
							{CAL_PLUSE_1();}
						}
}


/*·ÇÐ£×¼Ä£Ê½Ô¤¼ÆÊ®Âö³å£¬Êµ¼ÊÒ»Âö³å*/
void NCAL_PLUSE_1()
{
				float  m,i,i1;
	int n;
	unsigned char k;
				Write_MS1003_1Byte_Addr(ADDR1); //MS1003Ð´¼Ä´æÆ÷µØÖ·
				MS1003_SPI1_Send_Byte(0x0B);
				MS1003_SPI1_Send_Byte(0x3a);
				MS1003_SPI1_Send_Byte(0x40);
				MS1003_SPI1_Send_Byte(0x60);
				GPIO_SetBits(MS1003_SCS_PORT, MS1003_SCS); //ÖÃMS1003µÄSCSÎª¸ßµçÆ½
				Intn_flag = 1;
  
						while(1)
						{
						Intn_flag = 1;
						Write_MS1003_1Byte(Init);	
						Write_MS1003_1Byte(0x04);
						if(!INTN_fig(5000))	
							{	
						m  = Read_MS1003_NCAL_3Byte(0xbb);	
							}
				Write_MS1003_1Byte(Init);
							
						MS1003_START();		
						MS1003_STOP();
						key_flag = 0;
							if(!INTN_fig(5000))	
							{
								printf("feijiaozhun:");	
							i1 = Read_MS1003_NCAL_3Byte(0xb0);						
							i = Read_MS1003_NCAL_3Byte(0xbd);

							k =	Read_MS1003_PLUSE_3Byte();
							printf("   state:");									
							printf("%.0lf",i);
							printf("   CAL:");	
							printf("%.0lf",m);	
							printf("   reg0:");	
							printf("%.0lf",i1);		
							printf("\r\n");
							}	
							if (key_flag == 1)
							{NCAL_PLUSE_2();}
						}
}
/*·ÇÐ£×¼Ä£Ê½Ô¤¼ÆÊ®Âö³å£¬Êµ¼ÊÁ½Âö³å*/
void NCAL_PLUSE_2()
{
				float  m,i,i1,i2;
	unsigned char k;
	Write_MS1003_1Byte_Addr(ADDR1); //MS1003Ð´¼Ä´æÆ÷µØÖ·
				MS1003_SPI1_Send_Byte(0x0B);
				MS1003_SPI1_Send_Byte(0x3a);
				MS1003_SPI1_Send_Byte(0x40);
				MS1003_SPI1_Send_Byte(0x60);
				GPIO_SetBits(MS1003_SCS_PORT, MS1003_SCS); //ÖÃMS1003µÄSCSÎª¸ßµçÆ½
				Intn_flag = 1;
						while(1)
						{
						Intn_flag = 1;
						Write_MS1003_1Byte(Init);
						Write_MS1003_1Byte(0x04);
						if(!INTN_fig(5000))	
							{	
						m  = Read_MS1003_NCAL_3Byte(0xbb);	
							}	
				Write_MS1003_1Byte(Init);
							
						MS1003_START();
						MS1003_STOP();
						MS1003_STOP();	
						key_flag = 0;
							if(!INTN_fig(500))	
							{
								printf("feijiaozhun:");
							i1 = Read_MS1003_NCAL_3Byte(0xb0);
				//			Delay(10);
							i = Read_MS1003_NCAL_3Byte(0xbd); 		
				//			Delay(1);
							i2 = Read_MS1003_NCAL_3Byte(0xb1);	
					//		i = Read_MS1003_NCAL_3Byte(0xbd); 	
						//		GPIO_SetBits(MS1003_SCS_PORT, MS1003_SCS);//ÖÃMS1003µÄSCSÎª¸ßµçÆ½
							k =	Read_MS1003_PLUSE_3Byte();	
							printf("   state:");	
							printf("%.0lf",i);
							printf("   CAL:");	
							printf("%.0lf",m);	
							printf("   reg0:");	
							printf("%.0lf",i1);		
							printf("  reg1:");								
							printf("%.0lf",i2);	
							printf("\r\n");
							}	
							if (key_flag == 1)
							{NCAL_PLUSE_3();}
						}
}

/*·ÇÐ£×¼Ä£Ê½Ô¤¼ÆÊ®Âö³å£¬Êµ¼ÊÈýÂö³å*/
void NCAL_PLUSE_3()
{
	float  m,i,i1,i2,i3;
	unsigned char k;
	Write_MS1003_1Byte_Addr(ADDR1); //MS1003Ð´¼Ä´æÆ÷µØÖ·
MS1003_SPI1_Send_Byte(0x0b);
				MS1003_SPI1_Send_Byte(0x3a);
				MS1003_SPI1_Send_Byte(0x40);
				MS1003_SPI1_Send_Byte(0x60);
				GPIO_SetBits(MS1003_SCS_PORT, MS1003_SCS); //ÖÃMS1003µÄSCSÎª¸ßµçÆ½
				Intn_flag = 1;
						while(1)
						{
						Intn_flag = 1;
						Write_MS1003_1Byte(Init);	
						Write_MS1003_1Byte(0x04);
						if(!INTN_fig(5000))	
							{	
						m  = Read_MS1003_NCAL_3Byte(0xbb);	
							}
		Write_MS1003_1Byte(Init);
							
						MS1003_START();
						MS1003_STOP();
						MS1003_STOP();
						MS1003_STOP();	
							key_flag = 0;
							if(!INTN_fig(500))	
							{
								printf("feijiaozhun:");
							i1 = Read_MS1003_NCAL_3Byte(0xb0) ;
							i2 = Read_MS1003_NCAL_3Byte(0xb1) ;
							i3 = Read_MS1003_NCAL_3Byte(0xb2) ;	
							i = Read_MS1003_NCAL_3Byte(0xbd); 	
							k =	Read_MS1003_PLUSE_3Byte();	
							printf("   state:");	
							printf("%.0lf",i);
							printf("   CAL:");	
							printf("%.0lf",m);	
							printf("   reg0:");	
							printf("%.0lf",i1);		
							printf("  reg1:");								
							printf("%.0lf",i2);
							printf("  reg2:");								
							printf("%.0lf",i3);								
							printf("\r\n");
							}	
								if (key_flag == 1)
							{NCAL_PLUSE_4();}
						}
}

/*·ÇÐ£×¼Ä£Ê½Ô¤¼ÆÊ®Âö³å£¬Êµ¼ÊËÄÂö³å*/
void NCAL_PLUSE_4()
{
	float  m,i,i1,i2,i3,i4;
	unsigned char k;
	Write_MS1003_1Byte_Addr(ADDR1); //MS1003Ð´¼Ä´æÆ÷µØÖ·
MS1003_SPI1_Send_Byte(0x0b);
				MS1003_SPI1_Send_Byte(0x3a);
				MS1003_SPI1_Send_Byte(0x40);
				MS1003_SPI1_Send_Byte(0x60);
				GPIO_SetBits(MS1003_SCS_PORT, MS1003_SCS); //ÖÃMS1003µÄSCSÎª¸ßµçÆ½
				Intn_flag = 1;
						while(1)
						{
						Intn_flag = 1;
						Write_MS1003_1Byte(Init);	
						Write_MS1003_1Byte(0x04);
						if(!INTN_fig(5000))	
							{	
						m  = Read_MS1003_NCAL_3Byte(0xbb);	
							}	
					Write_MS1003_1Byte(Init);		
					
						MS1003_START();
						MS1003_STOP();
						MS1003_STOP();
						MS1003_STOP();
						MS1003_STOP();	
							key_flag = 0;
							if(!INTN_fig(500))	
							{
								printf("feijiaozhun:");	
							i1 = Read_MS1003_NCAL_3Byte(0xb0);
							i2 = Read_MS1003_NCAL_3Byte(0xb1) ;
							i3 = Read_MS1003_NCAL_3Byte(0xb2) ;
							i4 = Read_MS1003_NCAL_3Byte(0xb3) ;		

							i = Read_MS1003_NCAL_3Byte(0xbd); 
							k =	Read_MS1003_PLUSE_3Byte();
							printf("   state:");	
							printf("%.0lf",i);
							printf("   CAL:");	
							printf("%.0lf",m);	
							printf("   reg0:");	
							printf("%.0lf",i1);		
							printf("  reg1:");								
							printf("%.0lf",i2);
							printf("  reg2:");								
							printf("%.0lf",i3);	
							printf("  reg3:");								
							printf("%.0lf",i4);	
							printf("\r\n");
							}	
								if (key_flag == 1)
							{NCAL_PLUSE_5();}
						}
}

/*·ÇÐ£×¼Ä£Ê½Ô¤¼ÆÊ®Âö³å£¬Êµ¼ÊÎåÂö³å*/
void NCAL_PLUSE_5()
{
	float  m,i,i1,i2,i3,i4,i5;
	unsigned char k;
	Write_MS1003_1Byte_Addr(ADDR1); //MS1003Ð´¼Ä´æÆ÷µØÖ·
MS1003_SPI1_Send_Byte(0x0b);
				MS1003_SPI1_Send_Byte(0x3a);
				MS1003_SPI1_Send_Byte(0x40);
				MS1003_SPI1_Send_Byte(0x60);
				GPIO_SetBits(MS1003_SCS_PORT, MS1003_SCS); //ÖÃMS1003µÄSCSÎª¸ßµçÆ½
				Intn_flag = 1;
						while(1)
						{
						Intn_flag = 1;
						Write_MS1003_1Byte(Init);	
						Write_MS1003_1Byte(0x04);
						if(!INTN_fig(5000))	
							{	
						m  = Read_MS1003_NCAL_3Byte(0xbb);	
							}
							Write_MS1003_1Byte(Init);
							
						MS1003_START();
						MS1003_STOP();
						MS1003_STOP();
						MS1003_STOP();
						MS1003_STOP();
						MS1003_STOP();	
							key_flag = 0;
							if(!INTN_fig(500))	
							{
								printf("feijiaozhun:");	
							i1 = Read_MS1003_NCAL_3Byte(0xb0);
							i2 = Read_MS1003_NCAL_3Byte(0xb1) ;
							i3 = Read_MS1003_NCAL_3Byte(0xb2) ;
							i4 = Read_MS1003_NCAL_3Byte(0xb3) ;
							i5 = Read_MS1003_NCAL_3Byte(0xb4) ;							
							i = Read_MS1003_NCAL_3Byte(0xbd); 	
							k =	Read_MS1003_PLUSE_3Byte();
							printf("   state:");	
							printf("%.0lf",i);
							printf("   CAL:");	
							printf("%.0lf",m);
							printf("   reg0:");	
							printf("%.0lf",i1);		
							printf("  reg1:");								
							printf("%.0lf",i2);
							printf("  reg2:");								
							printf("%.0lf",i3);	
							printf("  reg3:");								
							printf("%.0lf",i4);	
							printf("  reg4:");								
							printf("%.0lf",i5);	
							printf("\r\n");
							}	
								if (key_flag == 1)
							{NCAL_PLUSE_6();}
						}
}

/*·ÇÐ£×¼Ä£Ê½Ô¤¼ÆÊ®Âö³å£¬Êµ¼ÊÁùÂö³å*/
void NCAL_PLUSE_6()
{
	float  m,i,i1,i2,i3,i4,i5,i6;
	unsigned char k;
	Write_MS1003_1Byte_Addr(ADDR1); //MS1003Ð´¼Ä´æÆ÷µØÖ·
MS1003_SPI1_Send_Byte(0x0b);
				MS1003_SPI1_Send_Byte(0x3a);
				MS1003_SPI1_Send_Byte(0x40);
				MS1003_SPI1_Send_Byte(0x60);
				GPIO_SetBits(MS1003_SCS_PORT, MS1003_SCS); //ÖÃMS1003µÄSCSÎª¸ßµçÆ½
				Intn_flag = 1;
						while(1)
						{
						Intn_flag = 1;
						Write_MS1003_1Byte(Init);	
						Write_MS1003_1Byte(0x04);
						if(!INTN_fig(5000))	
							{	
						m  = Read_MS1003_NCAL_3Byte(0xbb);	
							}
							Write_MS1003_1Byte(Init);
							
						MS1003_START();
						MS1003_STOP();
						MS1003_STOP();
						MS1003_STOP();
						MS1003_STOP();
						MS1003_STOP();
						MS1003_STOP();	
							key_flag = 0;
							if(!INTN_fig(500))	
							{
								printf("feijiaozhun:");	
							i1 = Read_MS1003_NCAL_3Byte(0xb0);
							i2 = Read_MS1003_NCAL_3Byte(0xb1);
							i3 = Read_MS1003_NCAL_3Byte(0xb2);
							i4 = Read_MS1003_NCAL_3Byte(0xb3);
							i5 = Read_MS1003_NCAL_3Byte(0xb4);		
							i6 = Read_MS1003_NCAL_3Byte(0xb5);	
							i = Read_MS1003_NCAL_3Byte(0xbd); 	
							k =	Read_MS1003_PLUSE_3Byte();
							printf("   state:");	
							printf("%.0lf",i);
							printf("   CAL:");	
							printf("%.0lf",m);
							printf("   reg0:");	
							printf("%.0lf",i1);		
							printf("  reg1:");								
							printf("%.0lf",i2);
							printf("  reg2:");								
							printf("%.0lf",i3);	
							printf("  reg3:");								
							printf("%.0lf",i4);	
							printf("  reg4:");								
							printf("%.0lf",i5);
							printf("  reg5:");								
							printf("%.0lf",i6);	
							printf("\r\n");
							}	
								if (key_flag == 1)
							{NCAL_PLUSE_7();}
						}
}

/*·ÇÐ£×¼Ä£Ê½Ô¤¼ÆÊ®Âö³å£¬Êµ¼ÊÆßÂö³å*/
void NCAL_PLUSE_7()
{
	float  m,i,i1,i2,i3,i4,i5,i6,i7;
	unsigned char k;
	Write_MS1003_1Byte_Addr(ADDR1); //MS1003Ð´¼Ä´æÆ÷µØÖ·
MS1003_SPI1_Send_Byte(0x0b);
				MS1003_SPI1_Send_Byte(0x3a);
				MS1003_SPI1_Send_Byte(0x40);
				MS1003_SPI1_Send_Byte(0x60);
				GPIO_SetBits(MS1003_SCS_PORT, MS1003_SCS); //ÖÃMS1003µÄSCSÎª¸ßµçÆ½
				Intn_flag = 1;
						while(1)
						{
						Intn_flag = 1;
						Write_MS1003_1Byte(Init);
						Write_MS1003_1Byte(0x04);
						if(!INTN_fig(5000))	
							{	
						m  = Read_MS1003_NCAL_3Byte(0xbb);	
							}
						Write_MS1003_1Byte(Init);
				
						MS1003_START();
						MS1003_STOP();
						MS1003_STOP();	
						MS1003_STOP();
						MS1003_STOP();
						MS1003_STOP();
						MS1003_STOP();
						MS1003_STOP();	
							key_flag = 0;
							if(!INTN_fig(500))	
							{
								printf("feijiaozhun:");	
							i1 = Read_MS1003_NCAL_3Byte(0xb0);
							i2 = Read_MS1003_NCAL_3Byte(0xb1);
							i3 = Read_MS1003_NCAL_3Byte(0xb2);
							i4 = Read_MS1003_NCAL_3Byte(0xb3) ;
							i5 = Read_MS1003_NCAL_3Byte(0xb4) ;		
							i6 = Read_MS1003_NCAL_3Byte(0xb5) ;
							i7 = Read_MS1003_NCAL_3Byte(0xb6) ;	
							i = Read_MS1003_NCAL_3Byte(0xbd); 	
							k =	Read_MS1003_PLUSE_3Byte();
							printf("   state:");	
							printf("%.0lf",i);
							printf("   CAL:");	
							printf("%.0lf",m);	
							printf("   reg0:");	
							printf("%.0lf",i1);		
							printf("  reg1:");								
							printf("%.0lf",i2);
							printf("  reg2:");								
							printf("%.0lf",i3);	
							printf("  reg3:");								
							printf("%.0lf",i4);	
							printf("  reg4:");								
							printf("%.0lf",i5);
							printf("  reg5:");								
							printf("%.0lf",i6);
							printf("  reg6:");								
							printf("%.0lf",i7);	
							printf("\r\n");
							}	
								if (key_flag == 1)
							{NCAL_PLUSE_8();}
						}
}

/*·ÇÐ£×¼Ä£Ê½Ô¤¼ÆÊ®Âö³å£¬Êµ¼Ê°ËÂö³å*/
void NCAL_PLUSE_8()
{
	float  m,i,i1,i2,i3,i4,i5,i6,i7,i8;
	unsigned char k;
	Write_MS1003_1Byte_Addr(ADDR1); //MS1003Ð´¼Ä´æÆ÷µØÖ·
MS1003_SPI1_Send_Byte(0x0b);
				MS1003_SPI1_Send_Byte(0x3a);
				MS1003_SPI1_Send_Byte(0x40);
				MS1003_SPI1_Send_Byte(0x60);
				GPIO_SetBits(MS1003_SCS_PORT, MS1003_SCS); //ÖÃMS1003µÄSCSÎª¸ßµçÆ½
				Intn_flag = 1;
						while(1)
						{
						Intn_flag = 1;
						Write_MS1003_1Byte(Init);
						Write_MS1003_1Byte(0x04);
						if(!INTN_fig(5000))	
							{	
						m  = Read_MS1003_NCAL_3Byte(0xbb);	
							}	
				Write_MS1003_1Byte(Init);
							
							
						MS1003_START();
						MS1003_STOP();
						MS1003_STOP();
						MS1003_STOP();
						MS1003_STOP();
						MS1003_STOP();
						MS1003_STOP();
						MS1003_STOP();
						MS1003_STOP();	
							key_flag = 0;
							if(!INTN_fig(500))	
							{
								printf("feijiaozhun:");	
							i1 = Read_MS1003_NCAL_3Byte(0xb0);
							i2 = Read_MS1003_NCAL_3Byte(0xb1);
							i3 = Read_MS1003_NCAL_3Byte(0xb2) ;
							i4 = Read_MS1003_NCAL_3Byte(0xb3) ;
							i5 = Read_MS1003_NCAL_3Byte(0xb4) ;		
							i6 = Read_MS1003_NCAL_3Byte(0xb5) ;
							i7 = Read_MS1003_NCAL_3Byte(0xb6) ;	
							i8 = Read_MS1003_NCAL_3Byte(0xb7) ;		
							i = Read_MS1003_NCAL_3Byte(0xbd); 	
							k =	Read_MS1003_PLUSE_3Byte();
							printf("   state:");	
							printf("%.0lf",i);
							printf("   CAL:");	
							printf("%.0lf",m);	
							printf("   reg0:");	
							printf("%.0lf",i1);		
							printf("  reg1:");								
							printf("%.0lf",i2);
							printf("  reg2:");								
							printf("%.0lf",i3);	
							printf("  reg3:");								
							printf("%.0lf",i4);	
							printf("  reg4:");								
							printf("%.0lf",i5);
							printf("  reg5:");								
							printf("%.0lf",i6);
							printf("  reg6:");								
							printf("%.0lf",i7);
							printf("  reg7:");								
							printf("%.0lf",i8);
							printf("\r\n");
							}	
								if (key_flag == 1)
							{NCAL_PLUSE_9();}
						}
}

/*·ÇÐ£×¼Ä£Ê½Ô¤¼ÆÊ®Âö³å£¬Êµ¼Ê¾ÅÂö³å*/
void NCAL_PLUSE_9()
{
	float  m,i,i1,i2,i3,i4,i5,i6,i7,i8,i9;
	unsigned char k;
	Write_MS1003_1Byte_Addr(ADDR1); //MS1003Ð´¼Ä´æÆ÷µØÖ·
MS1003_SPI1_Send_Byte(0x0b);
				MS1003_SPI1_Send_Byte(0x3a);
				MS1003_SPI1_Send_Byte(0x40);
				MS1003_SPI1_Send_Byte(0x60);
				GPIO_SetBits(MS1003_SCS_PORT, MS1003_SCS); //ÖÃMS1003µÄSCSÎª¸ßµçÆ½
				Intn_flag = 1;
						while(1)
						{
						Intn_flag = 1;
						Write_MS1003_1Byte(Init);
						Write_MS1003_1Byte(0x04);
						if(!INTN_fig(5000))	
							{	
						m  = Read_MS1003_NCAL_3Byte(0xbb);	
							}	
					Write_MS1003_1Byte(Init);
							
							
						MS1003_START();
						MS1003_STOP();
						MS1003_STOP();
						MS1003_STOP();
						MS1003_STOP();
						MS1003_STOP();	
						MS1003_STOP();
						MS1003_STOP();
						MS1003_STOP();
						MS1003_STOP();	
							key_flag = 0;
							if(!INTN_fig(500))	
							{
								printf("feijiaozhun:");	
							i1 = Read_MS1003_NCAL_3Byte(0xb0) ;
							i2 = Read_MS1003_NCAL_3Byte(0xb1) ;
							i3 = Read_MS1003_NCAL_3Byte(0xb2) ;
							i4 = Read_MS1003_NCAL_3Byte(0xb3) ;
							i5 = Read_MS1003_NCAL_3Byte(0xb4) ;		
							i6 = Read_MS1003_NCAL_3Byte(0xb5) ;
							i7 = Read_MS1003_NCAL_3Byte(0xb6) ;	
							i8 = Read_MS1003_NCAL_3Byte(0xb7) ;	
							i9 = Read_MS1003_NCAL_3Byte(0xb8) ;	
							i = Read_MS1003_NCAL_3Byte(0xbd); 	
							k =	Read_MS1003_PLUSE_3Byte();
							printf("   state:");	
							printf("%.0lf",i);
							printf("   CAL:");	
							printf("%.0lf",m);	
							printf("   reg0:");	
							printf("%.0lf",i1);		
							printf("  reg1:");								
							printf("%.0lf",i2);
							printf("  reg2:");								
							printf("%.0lf",i3);	
							printf("  reg3:");								
							printf("%.0lf",i4);	
							printf("  reg4:");								
							printf("%.0lf",i5);
							printf("  reg5:");								
							printf("%.0lf",i6);
							printf("  reg6:");								
							printf("%.0lf",i7);
							printf("  reg7:");								
							printf("%.0lf",i8);
							printf("  reg8:");								
							printf("%.0lf",i9);
							printf("\r\n");
							}	
								if (key_flag == 1)
							{NCAL_PLUSE_10();}
						}
}



/*·ÇÐ£×¼Ä£Ê½Ô¤¼ÆÊ®Âö³å£¬Êµ¼ÊÊ®Âö³å*/
void NCAL_PLUSE_10()   
{
	float  m,i,i1,i2,i3,i4,i5,i6,i7,i8,i9,i10;
	unsigned char k;
	Write_MS1003_1Byte_Addr(ADDR1); //MS1003Ð´¼Ä´æÆ÷µØÖ·
MS1003_SPI1_Send_Byte(0x0b);
				MS1003_SPI1_Send_Byte(0x3a);
				MS1003_SPI1_Send_Byte(0x40);
				MS1003_SPI1_Send_Byte(0x60);
				GPIO_SetBits(MS1003_SCS_PORT, MS1003_SCS); //ÖÃMS1003µÄSCSÎª¸ßµçÆ½
				Intn_flag = 1;
						while(1)
						{
						Intn_flag = 1;
						Write_MS1003_1Byte(Init);
						Write_MS1003_1Byte(0x04);
						if(!INTN_fig(5000))	
							{	
						m  = Read_MS1003_NCAL_3Byte(0xbb);	
							}	
								Write_MS1003_1Byte(Init);		
					
						MS1003_START();
						MS1003_STOP();
						MS1003_STOP();	
						MS1003_STOP();
						MS1003_STOP();
						MS1003_STOP();
						MS1003_STOP();
						MS1003_STOP();	
						MS1003_STOP();
						MS1003_STOP();
						MS1003_STOP();	
							key_flag = 0;
							if(!INTN_fig(5000))	
							{
								printf("feijiaozhun:");	
							i1 = Read_MS1003_NCAL_3Byte(0xb0);
							i2 = Read_MS1003_NCAL_3Byte(0xb1);
							i3 = Read_MS1003_NCAL_3Byte(0xb2);
							i4 = Read_MS1003_NCAL_3Byte(0xb3);
							i5 = Read_MS1003_NCAL_3Byte(0xb4);		
							i6 = Read_MS1003_NCAL_3Byte(0xb5);
							i7 = Read_MS1003_NCAL_3Byte(0xb6) ;	
							i8 = Read_MS1003_NCAL_3Byte(0xb7);	
							i9 = Read_MS1003_NCAL_3Byte(0xb8);	
							i10 = Read_MS1003_NCAL_3Byte(0xb9);	
							i = Read_MS1003_NCAL_3Byte(0xbd); 	
							k =	Read_MS1003_PLUSE_3Byte();
							printf("   state:");	
							printf("%.0lf",i);
							printf("   CAL:");	
							printf("%.0lf",m);	
							printf("   reg0:");	
							printf("%.0lf",i1);		
							printf("  reg1:");								
							printf("%.0lf",i2);
							printf("  reg2:");								
							printf("%.0lf",i3);	
							printf("  reg3:");								
							printf("%.0lf",i4);	
							printf("  reg4:");								
							printf("%.0lf",i5);
							printf("  reg5:");								
							printf("%.0lf",i6);
							printf("  reg6:");								
							printf("%.0lf",i7);
							printf("  reg7:");								
							printf("%.0lf",i8);
							printf("  reg8:");								
							printf("%.0lf",i9);
							printf("  reg9:");								
							printf("%.0lf",i10);
							printf("\r\n");
							}	
								if (key_flag == 1)
							{NCAL_PLUSE_1();}
						}
}






 int main(void)
{
				System_Initialization();	//STM32ÏµÍ³³õÊ¼»¯º¯Êý(³õÊ¼»¯STM32Ê±ÖÓ¼°ÍâÉè) 
				UART1_Init();								//STM32´®¿Ú³õÊ¼»
				MS1003_Initialization();    //1003³õÊ¼»¯²¢ÅäÖÃ32Î»¼Ä´æÆ÷¯
				if(GPIOB->IDR& GPIO_IDR_IDR15)					
				{
					while(1)
					{
					if(key_flag == 1 )
					{
						CAL_PLUSE_1();  //½øÐÐÐ£×¼Ä£Ê½²âÁ¿
					}
					}
				}
			 else  if(GPIOB->IDR& GPIO_IDR_IDR13)
			 {	
					while(1)
					{
					if(key_flag == 1 )
					{
						NCAL_PLUSE_1();  //½øÐÐ·ÇÐ£×¼Ä£Ê½²âÁ¿
					}
					}	
			 }	
}
