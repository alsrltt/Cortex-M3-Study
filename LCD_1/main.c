#include "stm32f10x_lib.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"

#include <stdio.h>   
#include "main.h"


#include "lcd12864.h"

#define  ADC1_DR_Address    ((u32)0x4001244C)
u16 ADCConvertedValue[1];
extern u16 adc1_buffer[1];


void RCC_Configuration(void);
void GPIO_Configuration(void);
void USART1_Configuration(void);
void ADC_Configuration(void);
void DMA1_Configuration(void);
void NVIC_Configuration(void);

void USART1_SEND(u8 i); 
void putstr(char *str);

extern u8 Buffer_USART1[50],  USART1_rx_end, USART1_len;

void main(void)
{
     u8 i;
     u16 AD_value;
     
     RCC_Configuration();
     
     RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA |  RCC_APB2Periph_GPIOC |
                             RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE |  RCC_APB2Periph_AFIO, ENABLE);
     
     RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
     RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);


     
     GPIO_Configuration();
     USART1_Configuration();
     ADC_Configuration();
     DMA1_Configuration();
     NVIC_Configuration();     
     
     putstr("Start Program. \n");
     
     lcd12864_init();
    
     Set_Draw();
     clear_screen();
   
     

      
      

     while(1){        
       
            
            fill_screen();
            delay_ms(1000);
            
  //          clear_screen();
   //         delay_ms(1);
           

     }
}

void USART1_SEND(u8 i)
{
    USART_SendData(USART1, i);     
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);      
}

void putstr(char *str) 
{ 
     char ch;     
     while((ch=*str)!= '\0') { 
	USART_SendData(USART1, *str);     
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);    
	str++; 
     } 
 }



void GPIO_Configuration(void)
{
     GPIO_InitTypeDef GPIO_InitStructure;      
     
     RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 
     RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); 

     
     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;          //USART1, TX
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
     GPIO_Init(GPIOA, &GPIO_InitStructure);   

     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;          //USART1, RX
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
     GPIO_Init(GPIOA, &GPIO_InitStructure); 

 
       // KEY
     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 |
                                   GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_13;
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   
     GPIO_Init(GPIOC, &GPIO_InitStructure);

       // LED
     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4; 
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   
     GPIO_Init(GPIOE, &GPIO_InitStructure);      
     
       // Analog Input
     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   
     GPIO_Init(GPIOC, &GPIO_InitStructure); 
     
}  


void USART1_Configuration(void)
{
     USART_InitTypeDef USART_InitStructure;
     USART_ClockInitTypeDef  USART_ClockInitStructure;     
     NVIC_InitTypeDef NVIC_InitStructure;      
     
     USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;
     USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
     USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;
     USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;
     USART_ClockInit(USART1, &USART_ClockInitStructure);

     USART_InitStructure.USART_BaudRate = 115200;
     USART_InitStructure.USART_WordLength = USART_WordLength_8b;
     USART_InitStructure.USART_StopBits = USART_StopBits_1;
     USART_InitStructure.USART_Parity = USART_Parity_No ;
     USART_InitStructure.USART_HardwareFlowControl =   USART_HardwareFlowControl_None;

     USART_InitStructure.USART_Mode = USART_Mode_Rx |  USART_Mode_Tx;     
     USART_Init(USART1, &USART_InitStructure);         
     USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);              
     NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQChannel; 
     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;   
     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;         
     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             
     NVIC_Init(&NVIC_InitStructure);     
     USART_Cmd(USART1, ENABLE);
}

void ADC_Configuration(void)
{
     ADC_InitTypeDef ADC_InitStructure;

     ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
     ADC_InitStructure.ADC_ScanConvMode = ENABLE;
     ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
     ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
     ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
     ADC_InitStructure.ADC_NbrOfChannel = 1;
     ADC_Init(ADC1, &ADC_InitStructure);

     ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 1, ADC_SampleTime_55Cycles5);
   
     ADC_DMACmd(ADC1, ENABLE);
     ADC_Cmd(ADC1, ENABLE);
     ADC_ResetCalibration(ADC1);   
     while(ADC_GetResetCalibrationStatus(ADC1));
     ADC_StartCalibration(ADC1);     
     while(ADC_GetCalibrationStatus(ADC1));
     ADC_SoftwareStartConvCmd(ADC1, ENABLE);    
}

void DMA1_Configuration(void)
 {
 
 	DMA_InitTypeDef DMA_InitStructure;

  	DMA_DeInit(DMA1_Channel1);
  	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;         	   // DMA 주변장치 데이터 시작 어드레스
  	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADCConvertedValue;          // DMA 메모리 데이터 시작 어드레스
  	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                      // 주변장치 데이터를 보낼 데이터로
  	DMA_InitStructure.DMA_BufferSize = 1;                                   // 보낼 데이터 크기(byte단위)
  	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;                // 자동으로 주변장치 데이터 어드레스 증가 금지

     DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;  
  	//DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     // 자동으로 메모리 데이터 어드레스 증가 금지  
  
  	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;      // 주변장치에서 한번에 16bit
  	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;           // 메모리에서 한번에 16bit
  	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                               // 환형 큐 모드, 보낼 데이터 크기가 없어지면 다시 reload
  	DMA_InitStructure.DMA_Priority = DMA_Priority_High;                           // DMA 우선순위 높음
  	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                        // 메모리 to 메모리 모드 아님
  	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  
  	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);  //TC:Transfer complete
  
  	/* Enable DMA1 channel1 */
  	DMA_Cmd(DMA1_Channel1, ENABLE); 
}

void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	#ifdef  VECT_TAB_RAM  
  	/* Set the Vector Table base location at 0x20000000 */ 
  	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
	#else  /* VECT_TAB_FLASH  */
  	/* Set the Vector Table base location at 0x08000000 */ 
  	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
	#endif
  
 	NVIC_InitStructure.NVIC_IRQChannel=DMA1_Channel1_IRQChannel;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
  	NVIC_Init(&NVIC_InitStructure);  
  
}

void RCC_Configuration(void)
{   
     ErrorStatus HSEStartUpStatus;
     /* RCC system reset(for debug purpose) */
     RCC_DeInit();

     /* Enable HSE */
     RCC_HSEConfig(RCC_HSE_ON);       

     /* Wait till HSE is ready */
     HSEStartUpStatus = RCC_WaitForHSEStartUp();

     if(HSEStartUpStatus == SUCCESS)
     {
          /* Enable Prefetch Buffer */
          FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

          /* Flash 2 wait state */
          FLASH_SetLatency(FLASH_Latency_2);
 	
          /* HCLK = SYSCLK */
          RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
          /* PCLK2 = HCLK */
          RCC_PCLK2Config(RCC_HCLK_Div1); 

          /* PCLK1 = HCLK/2 */
          RCC_PCLK1Config(RCC_HCLK_Div2);

          /* PLLCLK = 8MHz * 9 = 72 MHz */
          RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

          /* Enable PLL */ 
          RCC_PLLCmd(ENABLE);

          /* Wait till PLL is ready */
          while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
          {
          }

          /* Select PLL as system clock source */
          RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

          /* Wait till PLL is used as system clock source */
          while(RCC_GetSYSCLKSource() != 0x08)
          {
          }
     }
}