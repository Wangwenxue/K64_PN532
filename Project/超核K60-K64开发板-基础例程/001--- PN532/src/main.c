#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "PN532.h"

/* CH Kinetis固件库 V2.50 版本 */
/* 修改主频 请使用 CMSIS标准文件 startup_MKxxxx.c 中的 CLOCK_SETUP 宏 */

/* UART 快速初始化结构所支持的引脚* 使用时还是推荐标准初始化 */
/*
 UART1_RX_PE01_TX_PE00   
 UART0_RX_PF17_TX_PF18   
 UART3_RX_PE05_TX_PE04   
 UART5_RX_PF19_TX_PF20   
 UART5_RX_PE09_TX_PE08   
 UART2_RX_PE17_TX_PE16   
 UART4_RX_PE25_TX_PE24   
 UART0_RX_PA01_TX_PA02   
 UART0_RX_PA15_TX_PA14   
 UART3_RX_PB10_TX_PB11   
 UART0_RX_PB16_TX_PB17   
 UART1_RX_PC03_TX_PC04   
 UART4_RX_PC14_TX_PC15   
 UART3_RX_PC16_TX_PC17   
 UART2_RX_PD02_TX_PD03   
 UART0_RX_PD06_TX_PD07   
 UART2_RX_PF13_TX_PF14   
 UART5_RX_PD08_TX_PD09   
*/

 /*
     实验名称：UART串口
     实验平台：渡鸦开发板
     板载芯片：MK60DN512ZVQ10
 实验效果：使用开发板上的UART接口和电脑通信，
        开发板每隔500ms将数据发送到电脑的串口调试助手上
        同时开发板上还伴随着小灯的闪烁。
        串口的通信速度为115200 8位有效数据位
    注意：需要使用USB数据线连接到电脑端和开发板的UART接口
         电脑端需要安装CH340驱动和串口调试助手
*/


uint8_t RxBuffer[200];
uint32_t RxCounter=0;
uint8_t write_data[16]={0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF};

/* 串口接收中断回调函数 
   在函数中写中断想要做的事情
*/
static void UART4_RX_ISR(uint16_t byteReceived)
{
    /* 将接收到的数据发送回去 */
    //UART_WriteByte(HW_UART0, byteReceived);
	 RxBuffer[RxCounter++] = byteReceived;
	 
}


int main(void)
{
    uint32_t instance; /*存放 UART 的模块号 */
    DelayInit();
    DelayMs(10);
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    
    /* 初始化UART 使用快速初始化方式 波特率 115200 其他配置默认 返回初始化后 UART的模块号 */
    instance = UART_QuickInit(UART0_RX_PB16_TX_PB17, 115200);
    
    /* 当使用串口初始化后 printf 被默认连接到第一个被初始化的串口上*/
    printf("UART%d OK!\r\n", instance);
	
	  instance = UART_QuickInit(UART4_RX_PC14_TX_PC15, 115200);
	  printf("UART%d OK! Hello PN512\r\n", instance);
	  
	  /*  配置UART 中断配置 打开接收中断 安装中断回调函数 */
    UART_CallbackRxInstall(HW_UART4, UART4_RX_ISR);
    /* 打开串口接收中断功能 IT 就是中断的意思*/
    UART_ITDMAConfig(HW_UART4, kUART_IT_Rx, true);
	
	
	  nfc_WakeUp();//唤醒 PN532 芯片
	  printf("NFC wake up success!\r\n");
	
	  DelayMs(1000);
    nfc_InListPassiveTarget(); //寻找 nfc标签（Mifare 卡），获取UID
		
		nfc_PsdVerifyKeyA();
		
		nfc_read();
		
		nfc_write(write_data);
		
		nfc_read();
		
    while(1)
    {
        /* 串口 按字节发送 数据 注意 HW_UART0必须是已经初始化过的模块 否则 将产生错误*/
        UART_WriteByte(instance, 'h');
        UART_WriteByte(instance, 'e');
        UART_WriteByte(instance, 'l');
        UART_WriteByte(instance, 'l');
        UART_WriteByte(instance, 'o');
        UART_WriteByte(instance, '\r');
        UART_WriteByte(instance, '\n');
        /* 闪烁小灯 */
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


