/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
<<<<<<< HEAD
#include <string.h>
=======
#include <string.h>  // 用于字符串操作
>>>>>>> 7d5bf34f02e3037f56f4d9629cd730277be5cb25
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
<<<<<<< HEAD
#define SEND_INTERVAL 500  // 发送间隔，单位ms
#define RX_TIMEOUT    10   // 接收超时时间，单位ms
=======
#define SEND_INTERVAL 1000  // 发送间隔，单位：毫秒
>>>>>>> 7d5bf34f02e3037f56f4d9629cd730277be5cb25
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
<<<<<<< HEAD
uint8_t send_enable = 1;  // 发送使能标志，1：允许发送，0：禁止发送
uint8_t rx_data;          // 接收缓冲区
uint8_t tx_buffer[] = "hello windows!\r\n";  // 要发送的字符串
uint32_t last_send_time = 0;  // 上次发送时间
=======
uint32_t last_send_time = 0;  // 上次发送时间
uint8_t send_buffer[] = "hello windows!\r\n";  // 要发送的数据，包含回车换行
>>>>>>> 7d5bf34f02e3037f56f4d9629cd730277be5cb25
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
<<<<<<< HEAD
void Check_UART_Receive(void);
=======

>>>>>>> 7d5bf34f02e3037f56f4d9629cd730277be5cb25
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
<<<<<<< HEAD
/**
  * @brief  检查串口接收并处理
  * @param  None
  * @retval None
  */
void Check_UART_Receive(void)
{
  HAL_StatusTypeDef status;
  
  // 轮询检查是否有数据接收
  status = HAL_UART_Receive(&huart1, &rx_data, 1, RX_TIMEOUT);
  
  // 如果接收成功，处理控制字符
  if(status == HAL_OK)
  {
    switch(rx_data)
    {
      case '#':  // 暂停发送
        send_enable = 0;
        break;
      case '*':  // 继续发送
        send_enable = 1;
        break;
      default:   // 其他字符不处理
        break;
    }
  }
}
=======

>>>>>>> 7d5bf34f02e3037f56f4d9629cd730277be5cb25
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

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
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  last_send_time = HAL_GetTick();  // 初始化上次发送时间
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
<<<<<<< HEAD
    // 轮询检查串口接收
    Check_UART_Receive();
    
    // 如果允许发送，并且达到发送间隔，则发送数据
    if(send_enable && (HAL_GetTick() - last_send_time >= SEND_INTERVAL))
    {
      // 发送数据
      HAL_UART_Transmit(&huart1, tx_buffer, strlen((char*)tx_buffer), 0xFFFF);
=======
    // 轮询检查是否到达发送时间
    if (HAL_GetTick() - last_send_time >= SEND_INTERVAL)
    {
      // 使用轮询方式发送数据
      HAL_UART_Transmit(&huart1, send_buffer, strlen((char*)send_buffer), 100);
>>>>>>> 7d5bf34f02e3037f56f4d9629cd730277be5cb25
      
      // 更新上次发送时间
      last_send_time = HAL_GetTick();
    }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
<<<<<<< HEAD
    // 错误处理，可以添加LED闪烁等提示
=======
>>>>>>> 7d5bf34f02e3037f56f4d9629cd730277be5cb25
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
