#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "rtc.h"

void RTC_ISR(void)
{
    printf("RTC INT\r\n");
}


int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PB16_TX_PB17, 115200);
    
    /* 声明一个时间结构 */
    RTC_DateTime_Type td = {0};
    td.day = 20;
    td.hour = 23;
    td.minute = 59;
    td.second = 50;
    td.year = 2014;
    td.month = 11;
    printf("RTC alarm test\r\n");
    
    RTC_QuickInit();
    if(RTC_IsTimeValid() == false)
    {
        printf("bad time, reset!\r\n");
        RTC_SetTime(&td);
    }
    /* 开启中断 */
    RTC_CallbackInstall(RTC_ISR);
    RTC_ITDMAConfig(kRTC_IT_TimeAlarm, true);
    
    /* 设置闹钟在当前3秒后 */
    RTC_GetTime(&td);
    td.second += 3;
    RTC_SetAlarm(&td);

    while(1)
    {
        /* 获得时间 */
        RTC_GetTime(&td);//获得时间
        printf("%d-%d-%d %d:%d:%d\r\n", td.year, td.month, td.day, td.hour, td.minute, td.second);
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(1000);
    }
}


