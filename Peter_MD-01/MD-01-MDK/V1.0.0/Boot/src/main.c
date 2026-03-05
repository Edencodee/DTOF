#include "main.h"

#define BOOT_VERSION	"1.0.0"
typedef void (*jump_func_t)(void);
static void BSP_Init(void);
static void JumpToApp(void);
void SerialDownload(void);

extern uint32_t g_fSize;
extern uint8_t g_fileNameBuf[];

int main(void)
{
	int32_t ret = -1;
	uint8_t ch1 = 0, ch2 = 0;

	/* initilize peripherals */
    BSP_Init();

	printf("Run CD33 Boot Version %s\r\n", BOOT_VERSION);
	printf("Please press 'space' key into menu in 2s!\r\n");

	ret = UART_RecvByte(USER_COM1, &ch1, 2000); // ret = -1 timeouit

	if(ret == 0) {
		if(ch1 == 0x20) {
			printf("Please slecet: \r\n");
			printf("1: run app \r\n");
			printf("2: update app \r\n");
			printf("3: reboot \r\n");

			ret = UART_RecvByte(USER_COM1, &ch2, -1);

			if(ret == 0) {
				switch(ch2) {
				case 0x31:
					JumpToApp();
					break;

				case 0x32:
					SerialDownload();
					delay_1ms(100);
					printf("file name %s\r\n", g_fileNameBuf);
					printf("file size %d\r\n", g_fSize);
					printf("\r\n");
					break;

				case 0x33:
					__set_FAULTMASK(1);
					NVIC_SystemReset();
					break;

				default:
					printf("Select error!\r\n");
				}
			}
		} else {
			printf("Input error %d!\r\n", ch1);
		}
	} else {
		printf("Input timeout!\r\n");
	}

	delay_1ms(100);

	JumpToApp();
	
    while(1) {
    }
}

void SerialDownload(void) {
	printf("Waiting for the file to be sent ... (press 'a' to abort)\n\r");
	int32_t size = YmodemReceive();

	if(size < 0) {
		printf("Programming error!\n\r");
	} else {
		printf("Programming Completed.\n\r");
	}
}

static void JumpToApp() {
	jump_func_t applicationEntry;
	uint32_t jumpAddress;
	if(((*(volatile unsigned long*)APP_ADDR) & 0x2FFE0000) == 0x20000000) { /* Jump to user application */
		__disable_irq(); //close irq
		jumpAddress = *(volatile unsigned long*) (APP_ADDR + 4);
		applicationEntry = (jump_func_t) jumpAddress;
		/* Initialize user application's Stack Pointer */
		__set_MSP(*(volatile unsigned long*) APP_ADDR);  // init stack pointer
		applicationEntry();
		printf("Err to runing user application!\r\n");
	} else {
		printf("No user application to run!\r\n");
	}
}

static void BSP_Init(void) {
	__enable_irq();
	/* configure systick */
	systick_config();
	
	UART_UserDMAInit();
	UART_UserInit(USER_COM1, 115200);
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f) {
	usart_data_transmit(USER_COM1, (uint8_t)ch);
	while(RESET == usart_flag_get(USER_COM1, USART_FLAG_TBE));

	return ch;
}
