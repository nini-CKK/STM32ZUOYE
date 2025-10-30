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
#include "stm32f1xx_hal.h"  // 确保HAL库核心定义包含
#include "app_cfg.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <includes.h>   // uC/OS-III头文件
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */


// 3. 补全LED引脚操作宏（解决LED0_TOGGLE未声明错误，需与gpio.h一致）
// 假设LED0接PA0、LED1接PA1（若实际硬件不同，修改引脚和端口即可）
#define LED0_PIN    GPIO_PIN_0
#define LED0_PORT   GPIOB
#define LED1_PIN    GPIO_PIN_1
#define LED1_PORT   GPIOB
#define LED0_TOGGLE()  HAL_GPIO_TogglePin(LED0_PORT, LED0_PIN)
#define LED1_TOGGLE()  HAL_GPIO_TogglePin(LED1_PORT, LED1_PIN)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
// 声明外部串口句柄（定义在usart.c中）
extern UART_HandleTypeDef huart1;

// 任务控制块（TCB）
static OS_TCB   AppTaskStartTCB;    // 启动任务TCB
static OS_TCB   AppTaskLed0TCB;     // LED0任务TCB
static OS_TCB   AppTaskLed1TCB;     // LED1任务TCB
static OS_TCB   AppTaskUartTCB;     // 串口任务TCB

// 任务堆栈（使用上面定义的宏，解决未定义错误）
static CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];
static CPU_STK  AppTaskLed0Stk[APP_TASK_LED0_STK_SIZE];
static CPU_STK  AppTaskLed1Stk[APP_TASK_LED1_STK_SIZE];
static CPU_STK  AppTaskUartStk[APP_TASK_UART_STK_SIZE];

// 任务函数声明（后续实现，解决“未引用”警告）
static void AppTaskStart(void *p_arg);    // 启动任务：创建其他任务
static void AppTaskLed0(void *p_arg);     // LED0闪烁任务（1s周期）
static void AppTaskLed1(void *p_arg);     // LED1闪烁任务（3s周期）
static void AppTaskUart(void *p_arg);     // 串口发送任务（2s周期）
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
// 仅声明一次SystemClock_Config，解决重复定义错误
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
  * @brief 系统时钟配置（72MHz，基于HSE）
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  // 初始化外部高速时钟（HSE）和PLL
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;  // 8MHz HSE *9 =72MHz
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  // 配置系统时钟、AHB和APB总线时钟
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;    // HCLK=72MHz
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;     // PCLK1=36MHz
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;     // PCLK2=72MHz
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief 启动任务：创建其他应用任务后删除自身
  * @param p_arg: 任务参数（未使用）
  * @retval 无
  */
static void AppTaskStart(void *p_arg)
{
  OS_ERR  err;
  (void)p_arg;  // 消除未使用参数警告

  // 初始化uC/OS-III推荐组件
  BSP_Init();
  CPU_Init();
  Mem_Init();

#if OS_CFG_STAT_TASK_EN > 0u
  OSStatTaskCPUUsageInit(&err);  // 初始化CPU使用率统计（可选）
#endif

  // 1. 创建LED0任务（1s周期闪烁）
  OSTaskCreate((OS_TCB     *)&AppTaskLed0TCB,
               (CPU_CHAR   *)"LED0 Task",
               (OS_TASK_PTR )AppTaskLed0,
               (void       *)0,
               (OS_PRIO     )APP_TASK_LED0_PRIO,
               (CPU_STK    *)&AppTaskLed0Stk[0],
               (CPU_STK_SIZE)APP_TASK_LED0_STK_SIZE / 10,
               (CPU_STK_SIZE)APP_TASK_LED0_STK_SIZE,
               (OS_MSG_QTY  )0,
               (OS_TICK     )0,
               (void       *)0,
               (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               (OS_ERR     *)&err);

  // 2. 创建LED1任务（3s周期闪烁）
  OSTaskCreate((OS_TCB     *)&AppTaskLed1TCB,
               (CPU_CHAR   *)"LED1 Task",
               (OS_TASK_PTR )AppTaskLed1,
               (void       *)0,
               (OS_PRIO     )APP_TASK_LED1_PRIO,
               (CPU_STK    *)&AppTaskLed1Stk[0],
               (CPU_STK_SIZE)APP_TASK_LED1_STK_SIZE / 10,
               (CPU_STK_SIZE)APP_TASK_LED1_STK_SIZE,
               (OS_MSG_QTY  )0,
               (OS_TICK     )0,
               (void       *)0,
               (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               (OS_ERR     *)&err);

  // 3. 创建串口发送任务（2s周期）
  OSTaskCreate((OS_TCB     *)&AppTaskUartTCB,
               (CPU_CHAR   *)"UART Task",
               (OS_TASK_PTR )AppTaskUart,
               (void       *)0,
               (OS_PRIO     )APP_TASK_UART_PRIO,
               (CPU_STK    *)&AppTaskUartStk[0],
               (CPU_STK_SIZE)APP_TASK_UART_STK_SIZE / 10,
               (CPU_STK_SIZE)APP_TASK_UART_STK_SIZE,
               (OS_MSG_QTY  )0,
               (OS_TICK     )0,
               (void       *)0,
               (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               (OS_ERR     *)&err);

  // 启动任务完成使命，删除自身释放资源
  OSTaskDel((OS_TCB *)0, &err);  // 参数为0表示删除当前任务

  while (DEF_TRUE)  // 理论上不会执行
  {
    OSTimeDlyHMSM(0, 0, 1, 0, OS_OPT_TIME_HMSM_STRICT, &err);
  }
}

/**
  * @brief LED0任务：1s周期闪烁（500ms亮/500ms灭）
  * @param p_arg: 任务参数（未使用）
  * @retval 无
  */
static void AppTaskLed0(void *p_arg)
{
  OS_ERR err;
  (void)p_arg;

  while (DEF_TRUE)
  {
    LED0_TOGGLE();  // 翻转LED0状态
    // 延时500ms（1s周期），使用uC/OS-III延时函数（避免HAL_Delay冲突）
    OSTimeDlyHMSM(0, 0, 0, 500, OS_OPT_TIME_HMSM_STRICT, &err);
  }
}

/**
  * @brief LED1任务：3s周期闪烁（1.5s亮/1.5s灭）
  * @param p_arg: 任务参数（未使用）
  * @retval 无
  */
static void AppTaskLed1(void *p_arg)
{
  OS_ERR err;
  (void)p_arg;

  while (DEF_TRUE)
  {
    LED1_TOGGLE();  // 翻转LED1状态
    // 延时1.5s（3s周期）
    OSTimeDlyHMSM(0, 0, 1, 500, OS_OPT_TIME_HMSM_STRICT, &err);
  }
}

/**
  * @brief 串口任务：2s周期发送字符串
  * @param p_arg: 任务参数（未使用）
  * @retval 无
  */
static void AppTaskUart(void *p_arg)
{
  OS_ERR err;
  (void)p_arg;
  // 要发送的字符串（包含换行符，方便串口助手查看）
  uint8_t uart_msg[] = "hello uc/OS! 欢迎来到RTOS多任务环境！\r\n";

  while (DEF_TRUE)
  {
    // 阻塞式发送字符串（超时100ms）
    HAL_UART_Transmit(&huart1, uart_msg, sizeof(uart_msg)-1, 100);
    // 延时2s（发送周期）
    OSTimeDlyHMSM(0, 0, 2, 0, OS_OPT_TIME_HMSM_STRICT, &err);
  }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  OS_ERR  err;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  OSInit(&err);  // 初始化uC/OS-III内核
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  // 创建启动任务（uC/OS-III多任务入口）
  OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,
               (CPU_CHAR   *)"App Task Start",
               (OS_TASK_PTR )AppTaskStart,
               (void       *)0,
               (OS_PRIO     )APP_TASK_START_PRIO,
               (CPU_STK    *)&AppTaskStartStk[0],
               (CPU_STK_SIZE)APP_TASK_START_STK_SIZE / 10,
               (CPU_STK_SIZE)APP_TASK_START_STK_SIZE,
               (OS_MSG_QTY  )0,
               (OS_TICK     )0,
               (void       *)0,
               (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               (OS_ERR     *)&err);

  // 启动uC/OS-III内核（启动后进入多任务调度）
  OSStart(&err);
  /* USER CODE END 2 */

  /* Infinite loop（OSStart后不会执行到这里） */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  while (1)
  {
    LED0_TOGGLE();  // 错误时LED0快速闪烁（200ms周期）
    HAL_Delay(200);
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
  // 断言失败时通过串口输出错误信息（需确保printf已重定向）
  printf("Assert failed: file %s on line %d\r\n", file, line);
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
