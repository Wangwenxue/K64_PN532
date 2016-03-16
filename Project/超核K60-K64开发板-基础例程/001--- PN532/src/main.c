#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "PN532.h"

/* CH Kinetis�̼��� V2.50 �汾 */
/* �޸���Ƶ ��ʹ�� CMSIS��׼�ļ� startup_MKxxxx.c �е� CLOCK_SETUP �� */

/* UART ���ٳ�ʼ���ṹ��֧�ֵ�����* ʹ��ʱ�����Ƽ���׼��ʼ�� */
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
     ʵ�����ƣ�UART����
     ʵ��ƽ̨����ѻ������
     ����оƬ��MK60DN512ZVQ10
 ʵ��Ч����ʹ�ÿ������ϵ�UART�ӿں͵���ͨ�ţ�
        ������ÿ��500ms�����ݷ��͵����ԵĴ��ڵ���������
        ͬʱ�������ϻ�������С�Ƶ���˸��
        ���ڵ�ͨ���ٶ�Ϊ115200 8λ��Ч����λ
    ע�⣺��Ҫʹ��USB���������ӵ����Զ˺Ϳ������UART�ӿ�
         ���Զ���Ҫ��װCH340�����ʹ��ڵ�������
*/


uint8_t RxBuffer[200];
uint32_t RxCounter=0;
uint8_t write_data[16]={0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF};

/* ���ڽ����жϻص����� 
   �ں�����д�ж���Ҫ��������
*/
static void UART4_RX_ISR(uint16_t byteReceived)
{
    /* �����յ������ݷ��ͻ�ȥ */
    //UART_WriteByte(HW_UART0, byteReceived);
	 RxBuffer[RxCounter++] = byteReceived;
	 
}


int main(void)
{
    uint32_t instance; /*��� UART ��ģ��� */
    DelayInit();
    DelayMs(10);
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    
    /* ��ʼ��UART ʹ�ÿ��ٳ�ʼ����ʽ ������ 115200 ��������Ĭ�� ���س�ʼ���� UART��ģ��� */
    instance = UART_QuickInit(UART0_RX_PB16_TX_PB17, 115200);
    
    /* ��ʹ�ô��ڳ�ʼ���� printf ��Ĭ�����ӵ���һ������ʼ���Ĵ�����*/
    printf("UART%d OK!\r\n", instance);
	
	  instance = UART_QuickInit(UART4_RX_PC14_TX_PC15, 115200);
	  printf("UART%d OK! Hello PN512\r\n", instance);
	  
	  /*  ����UART �ж����� �򿪽����ж� ��װ�жϻص����� */
    UART_CallbackRxInstall(HW_UART4, UART4_RX_ISR);
    /* �򿪴��ڽ����жϹ��� IT �����жϵ���˼*/
    UART_ITDMAConfig(HW_UART4, kUART_IT_Rx, true);
	
	
	  nfc_WakeUp();//���� PN532 оƬ
	  printf("NFC wake up success!\r\n");
	
	  DelayMs(1000);
    nfc_InListPassiveTarget(); //Ѱ�� nfc��ǩ��Mifare ��������ȡUID
		
		nfc_PsdVerifyKeyA();
		
		nfc_read();
		
		nfc_write(write_data);
		
		nfc_read();
		
    while(1)
    {
        /* ���� ���ֽڷ��� ���� ע�� HW_UART0�������Ѿ���ʼ������ģ�� ���� ����������*/
        UART_WriteByte(instance, 'h');
        UART_WriteByte(instance, 'e');
        UART_WriteByte(instance, 'l');
        UART_WriteByte(instance, 'l');
        UART_WriteByte(instance, 'o');
        UART_WriteByte(instance, '\r');
        UART_WriteByte(instance, '\n');
        /* ��˸С�� */
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


