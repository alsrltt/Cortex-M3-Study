
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_NVIC_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

int fputc(int ch, FILE *f)
{
  uint8_t temp[1]={ch};
  HAL_UART_Transmit(&huart1, temp, 1, 2);
  return(ch);
}
extern TIM_HandleTypeDef htim6; // tim.c에 있는 거라서
void SystemClock_Config(void);
unsigned char Timer_500m;
unsigned char flag=0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
     if(htim -> Instance == TIM6)
     {
        if(flag==1)
         {
           HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_2| GPIO_PIN_3| GPIO_PIN_4);
         }
     }
}

uint8_t Rx_Data[2];    
uint8_t Rx_Buffer[20];    
uint8_t USART1_len=0;           
uint8_t USART1_rx_end=0;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  static uint8_t usart1_chk=0;
  if(huart -> Instance == USART1)
  {
      USART1_rx_end=0;      
      switch(usart1_chk)
      {
          case 0: 
            if(Rx_Data[0]==0x02){                    
                //Rx_Buffer[USART1_len]=Rx_Data[0];
                //USART1_len++;              
                usart1_chk=1;
            }
            else 
              usart1_chk=0;
            break;            
          case 1:
            if(Rx_Data[0]==0x03){
              USART1_rx_end=1;
              usart1_chk=0;
            }
            else{
              Rx_Buffer[USART1_len]=Rx_Data[0];
              USART1_len++;           
            }
            break;            
          default : 
            usart1_chk=0;
            break;        
      }   
      HAL_UART_Receive_IT(&huart1, Rx_Data, 1);
  }
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4,RESET);
  flag=0;
  if(GPIO_Pin == GPIO_PIN_0)
    HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_2);
  else if(GPIO_Pin == GPIO_PIN_1)
    HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_3);
  else if(GPIO_Pin == GPIO_PIN_2)
    HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_4);  
  else if(GPIO_Pin == GPIO_PIN_3)
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4,SET);
  else{
    flag=1;
  }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  uint8_t i;
   uint32_t ADC_Value_32[2];      // ADC Value
   uint16_t ADC_Value_16[2];      // ADC Value

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_TIM6_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */
//  HAL_NVIC_DisableIRQ(TIM6_IRQn);
  HAL_TIM_Base_Start_IT(&htim6);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  HAL_TIMEx_PWMN_Start(&htim3, TIM_CHANNEL_1);
  
  
//  uint8_t Tx_Buffer[20];
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  TIM3->CCR1 = 0;
  HAL_UART_Receive_IT(&huart1, Rx_Data, 1);
  while (1)
  {     
        if(USART1_rx_end)
	{
          int i=0;
          int tmp =0;
          for(i=0;i<USART1_len;i++){
            tmp*=10;
            tmp += Rx_Buffer[i]-'0';
          }
          tmp = (int)(tmp*40.96);
          printf("%d\n\r",tmp);
	  USART1_len=0;
	  USART1_rx_end=0;
          TIM3->CCR1 =tmp;	  
	}

        
//         if(Timer_500m % 5 == 0)
//         {
//           HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_3);
//         }
//         if(Timer_500m % 10 == 0)
//         {
//            HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_4);
//           Timer_500m = 0;
//         }
//         
      
//        HAL_ADC_Start_DMA(&hadc1, ADC_Value_32, 1);
//	HAL_Delay(1000);
//	ADC_Value_16=(uint16_t)ADC_Value_32[0];
//	ADC_Value_16 &= 0xFFF;
//	printf("\n ADC Value=%d", ADC_Value_16);   
//        
//        int thousand = (ADC_Value_16 - 1) /1000;
//        uint16_t status[] = {
//          GPIO_PIN_2,
//          GPIO_PIN_3,
//          GPIO_PIN_4,
//          GPIO_PIN_2|GPIO_PIN_3,
//          GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4
//        };
//          
//        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4, GPIO_PIN_RESET);
//        HAL_GPIO_WritePin(GPIOE, status[thousand], GPIO_PIN_SET);
//      HAL_ADC_Start_DMA(&hadc1, ADC_Value_32, 2);			
//      HAL_Delay(100);
//      for(i=0; i<2; i++){
//         ADC_Value_16[i]=(uint16_t)ADC_Value_32[i];  
//      }	
//      ADC_Value_16[0] &= 0xFFF;
//      printf("X:%d \n", ADC_Value_16[0]);   	
//      ADC_Value_16[1] &= 0xFFF;
//      printf("Y:%d \n", ADC_Value_16[1]);  	
    
//    if(USART1_rx_end)
//	{
//	  HAL_UART_Transmit(&huart1, Rx_Buffer, USART1_len, 2);
//	  USART1_len=0;
//	  USART1_rx_end=0;	  
//	}
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
//     Tx_Buffer[0] = 'H';
//     Tx_Buffer[1] = 'E';
//     Tx_Buffer[2] = 'L';
//     Tx_Buffer[3] = 'L';
//     Tx_Buffer[4] = 'O';
//     while(1){
//       if(!(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0))){
//        HAL_UART_Transmit(&huart1, Tx_Buffer, 5, 2);
//        HAL_Delay(1000);
//       }
//     }
  }


  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* EXTI0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);
  /* EXTI1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);
  /* EXTI3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);
  /* EXTI2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);
  /* EXTI4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
