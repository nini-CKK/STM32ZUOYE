/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "gpio.h"
#include "u8g2.h" 
#include "u8x8.h"  // 必须包含u8x8头文件
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define OLED_I2C_ADDR 0x78  // OLED的I2C地址（根据实际模块修改，常见0x78或0x3C）
/* USER CODE END PD */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void u8g2Init(u8g2_t *u8g2);
void u8g2DrawTest(u8g2_t *u8g2);
void draw(u8g2_t *u8g2);  // 声明draw函数
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// 1. 实现u8x8硬件I2C通信函数（u8g2依赖）
uint8_t u8x8_byte_stm32_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  static uint8_t buf[128];
  static uint8_t buf_len = 0;
  uint8_t *data = (uint8_t *)arg_ptr;

  switch(msg)
  {
    case U8X8_MSG_BYTE_INIT:
      // I2C已在MX_I2C2_Init中初始化，此处无需操作
      break;
    case U8X8_MSG_BYTE_START_TRANSFER:
      buf_len = 0;  // 重置缓冲区
      break;
    case U8X8_MSG_BYTE_SEND:
      // 数据存入缓冲区
      while(arg_int-- > 0)
        buf[buf_len++] = *data++;
      break;
    case U8X8_MSG_BYTE_END_TRANSFER:
      // 发送数据到OLED（地址0x78，可根据实际修改）
      if(HAL_I2C_Master_Transmit(&hi2c2, 0x78, buf, buf_len, 100) != HAL_OK)
        return 0;  // 发送失败
      break;
    default:
      return 0;
  }
  return 1;
}

// 2. 实现u8x8延时和GPIO函数（u8g2依赖）
uint8_t u8x8_gpio_and_delay_stm32(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  switch(msg)
  {
    case U8X8_MSG_DELAY_MILLI:
      HAL_Delay(arg_int);  // 毫秒延时
      break;
    case U8X8_MSG_DELAY_10MICRO:
      // 微秒延时（简单实现）
      for(uint32_t i=0; i<arg_int*10; i++) __NOP();
      break;
    // 其他GPIO消息无需处理（硬件I2C已接管）
    default:
      return 0;
  }
  return 1;
}

// 3. 初始化OLED（适配SSD1306 128x64）
void u8g2Init(u8g2_t *u8g2);
// 4. 显示内容（修复中文编码和语法错误）
void u8g2DrawTest(u8g2_t *u8g2);

// 5. 空实现draw函数（避免编译错误）
void draw(u8g2_t *u8g2);
/* USER CODE END 0 */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* MCU Configuration--------------------------------------------------------*/
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_I2C2_Init();  // 初始化I2C2（确保时钟频率为100kHz）
  MX_TIM1_Init();

  /* USER CODE BEGIN 2 */
  u8g2_t u8g2;
  u8g2Init(&u8g2);  // 初始化OLED
  /* USER CODE END 2 */

  /* Infinite loop */
  while (1)
  {
    /* 刷新显示 */
    u8g2_FirstPage(&u8g2);
    do
    {
      draw(&u8g2);
      u8g2DrawTest(&u8g2);  // 绘制内容
    } while (u8g2_NextPage(&u8g2));

    HAL_Delay(500);  // 500ms刷新一次
    /* USER CODE END 3 */
  }
}

// 以下代码保持不变（系统时钟、错误处理等）
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
    // 错误时可让LED闪烁（需在MX_GPIO_Init中配置PA5为输出）
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    HAL_Delay(500);
  }
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
