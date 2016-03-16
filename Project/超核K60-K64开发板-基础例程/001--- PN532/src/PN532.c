#include "common.h"
#include "uart.h"

void CleanBuffer(uint16_t num);

extern uint8_t RxBuffer[200];
uint8_t flag_rev_finish;  //1为接收完成
uint8_t UID[4]; //存储 UID
extern uint32_t RxCounter;

////55 55 00 00 00 00 00 00 00 00 00 00 00 00 00 00 FF 03 FD D4 14 01 17 00  
void nfc_WakeUp(void)
{
    uint8_t data[24];
    uint8_t i;
    uint8_t CheckCode=0; //数据校验码
    uint8_t temp=0;
  
    while(1)
    { 
      //  flag_rev_finish=0;
        data[0]=0x55;
        data[1]=0x55;
        data[2]=0x00;
        data[3]=0x00;
        data[4]=0x00;
        data[5]=0x00;
        data[6]=0x00;
        data[7]=0x00;
        data[8]=0x00;
        data[9]=0x00;
        data[10]=0x00;
        data[11]=0x00;
        data[12]=0x00;
        data[13]=0x00;
        data[14]=0x00;
        data[15]=0x00;
        
        data[16]=0xFF;
        data[17]=0x03; //包 长度
        data[18]=0xFD; //包 长度 校验  0x100-data[3]
        data[19]=0xD4; //命令标识码
        data[20]=0x14; //命令标识码
        data[21]=0x01;
        data[22]=0x17; //数据 校验   0x100-
        data[23]=0x00;
        
				for(uint8_t i=0;i<24;i++)
					UART_WriteByte(4,data[i]);
				
          DelayMs(380); 
          
        //00 00 FF 00 FF 00 00 00 FF 02 FE D5 15 16 00    
     
            temp=0;
			      DelayMs(500); 
            for(i=11;i<13;i++)
            {
                temp+=RxBuffer[i];
            }
            CheckCode=0x100-temp;
            if(CheckCode==RxBuffer[13])
            {
                CheckCode=0x100-temp;
                CleanBuffer(40);//清除 串口接收缓冲区前30 个字节数据
							  RxCounter=0;  // wenxue  重新置0

                break;
            }
    
    }
}


//配置 106 kbps type A (ISO/IEC14443 Type A),
//00 00 FF 04 FC D4 4A 01 00 E1 00
void  nfc_InListPassiveTarget(void)
{
    uint8_t data[11];
    uint8_t i;
    uint8_t CheckCode=0; //数据校验码
    uint8_t temp=0;
    while(1)
    {   
        data[0]=0x00;
        data[1]=0x00;
        data[2]=0xFF;
        data[3]=0x04; //包 长度
        data[4]=0xFC; //包 长度 校验  0x100-data[3]
        data[5]=0xD4; //命令标识码
        data[6]=0x4A; //命令标识码
        data[7]=0x01;
        data[8]=0x00;
        data[9]=0xE1; //数据 校验   0x100-
        data[10]=0x00;
        			
			  for(uint8_t i=0;i<11;i++)//往UART4，发送 11字节长度的数据data
					UART_WriteByte(4,data[i]);
			
			
        DelayMs(180); 
      
			  DelayMs(500); 
       
        temp =0;
        for(i=11;i<23;i++)
            {
                temp+=RxBuffer[i];
            }
            CheckCode=0x100-temp;
            if(CheckCode==RxBuffer[23])
            {
                UID[0]=RxBuffer[19];
                UID[1]=RxBuffer[20];
                UID[2]=RxBuffer[21];
                UID[3]=RxBuffer[22];  
                
						  	printf("UID:");
							  for(uint8_t i=0;i<4;i++)
							    printf("%02x ",UID[i] ); 
							  printf("\r\n");
                
                CleanBuffer(40);//清除 串口接收缓冲区前30 个字节数据
							  RxCounter=0;  // wenxue 
                break;
            }
    }
}




// 密码授权，验证KeyA
//00 00 FF 0F F1 D4 40 01 60 03 FF FF FF FF FF FF UID1 UID2 UID3 UID4 2A 00
//00 00 FF 0F F1 D4 40 01 60 03 FF FF FF FF FF FF 94 8A 3B 0B 2A 00
void  nfc_PsdVerifyKeyA(void)
{
    uint8_t data[22];
    uint8_t temp=0; 
    uint8_t i;
    uint8_t CheckCode=0; //数据校验码
   
    data[0]=0x00;
    data[1]=0x00;
    data[2]=0xFF;
    
    data[3]=0x0F; //包 长度
    data[4]=0xF1; //包 长度 校验  0x100-data[3]
    
    data[5]=0xD4; //命令标识码
    data[6]=0x40; //命令标识码
    
    data[7]=0x01;
    data[8]=0x60;
    data[9]=0x03; 
    
    data[10]=0xFF; //KEY A 密码 FF FF FF FF FF FF
    data[11]=0xFF;
    data[12]=0xFF;
    data[13]=0xFF;
    data[14]=0xFF;
    data[15]=0xFF;
    
    data[16]=UID[0];
    data[17]=UID[1];
    data[18]=UID[2];
    data[19]=UID[3];
    
    for(i=5;i<20;i++)
    {
        temp+=data[i];
    }

    data[20]=0x100-temp;   //数据 校验   0x100-
    
    data[21]=0x00;    
     
		for(uint8_t i=0;i<22;i++)//往UART4，发送 22字节长度的数据data
					UART_WriteByte(4,data[i]);
    
    
    DelayMs(180);
   

  
    temp=0;
    for(i=11;i<14;i++)
    {
        temp+=RxBuffer[i];
    }
    CheckCode=0x100-temp;
    if(CheckCode==RxBuffer[14])
    {
       if(RxBuffer[13]==0x00)
       {
         CleanBuffer(40);//清除 串口接收缓冲区前40 个字节数据
         RxCounter=0;  // wenxue 
				 printf("Verify success!\r\n");

       }
    }
}




//默认 读 02区的16个字节
//00 00 FF 05 FB D4 40 01 30 02 B9 00
void  nfc_read()
{
    uint8_t data[12];
    uint8_t temp=0; 
    uint8_t i;
    uint8_t CheckCode=0; //数据校验码

   
    data[0]=0x00;
    data[1]=0x00;
    data[2]=0xFF;
    
    data[3]=0x05; //包 长度
    data[4]=0xFB; //包 长度 校验  0x100-data[3]
    
    data[5]=0xD4; //命令标识码
    data[6]=0x40; //命令标识码
    
    data[7]=0x01;
    data[8]=0x30;
    data[9]=0x02; //读第二块的16字节数据 
    
    data[10]=0xB9; //数据校验
    data[11]=0x00;

		
		for(uint8_t i=0;i<12;i++)//往UART4，发送 22字节长度的数据data
		  	UART_WriteByte(4,data[i]);
    
    DelayMs(180);
    DelayMs(800);
  
    temp=0;
    for(i=11;i<30;i++)
    {
        temp+=RxBuffer[i];
    }
    CheckCode=0x100-temp;
    if(CheckCode==RxBuffer[30])
    {       

         printf("Read Data:");				  
			   for(uint8_t i=14;i<30;i++)
				  	printf("%02x ",RxBuffer[i] ); 
				 printf("\r\n");
			
         CleanBuffer(40);//清除 串口接收缓冲区前40 个字节数据
		   	RxCounter=0;  // wenxue 
    }
}



//默认往 02区写 16个字节的第一个字节
//00 00 FF 15 EB D4 40 01 A0 02 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F D1 00
void  nfc_write(uint8_t buf[16])
{
    uint8_t data[28];
    uint8_t temp=0;
    uint8_t i;
    uint8_t CheckCode=0;
    
    data[0]=0x00;
    data[1]=0x00;
    data[2]=0xFF;
    
    data[3]=0x15; //包 长度
    data[4]=0xEB; //包 长度 校验  0x100-data[3]
    
    data[5]=0xD4; //命令标识码
    data[6]=0x40; //命令标识码
    
    data[7]=0x01; //读写大于6字节 就置1，看手册
    data[8]=0xA0; //写
    data[9]=0x02; //写第二块的16字节数据 
    
    data[10]=buf[0]; //第 1 字节 数据
    data[11]=buf[1];
    data[12]=buf[2]; //第 3 字节 数据
    data[13]=buf[3];
    data[14]=buf[4]; //第 5 字节 数据
    data[15]=buf[5];
    data[16]=buf[6]; //第 7 字节 数据
    data[17]=buf[7];
    data[18]=buf[8]; //第 9 字节 数据
    data[19]=buf[9];
    data[20]=buf[10]; //第 11 字节 数据
    data[21]=buf[11];
    data[22]=buf[12]; //第 13 字节 数据
    data[23]=buf[13];
    data[24]=buf[14]; //第 15 字节 数据
    data[25]=buf[15];
    
		 temp=0;
    for(i=5;i<26;i++)
    {
        temp+=data[i];
    }
    data[26]=0x100-temp; //数据 校验码
    data[27]=0x00;

    
    for(uint8_t i=0;i<28;i++)//往UART4，发送28字节长度的数据data
			UART_WriteByte(4,data[i]); 
		
    DelayMs(180);
		DelayMs(500);

  //00 00 FF 00 FF 00 00 00 FF 03 FD D5 41 00 EA 00
    temp=0;
    for(i=11;i<14;i++)
    {
        temp+=RxBuffer[i];
    }
    CheckCode=0x100-temp;
    if(CheckCode==RxBuffer[14])
    {
        
       printf("Write Data success!\r\n");		
			 CleanBuffer(40);//清除 串口接收缓冲区前40 个字节数据
			 RxCounter=0;  // wenxue 
         

    }
}


void CleanBuffer(uint16_t num)//清零 前 多少个字节的数据
{
    uint16_t i=0;
    
    for(i=0;i<num;i++)
      RxBuffer[i]=0x00;
}