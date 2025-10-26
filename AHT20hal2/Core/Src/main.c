#include "main.h"
#include "AHT20-21_DEMO_V1_3.h"
#include "usart.h"
#include <stdio.h>

// 函数声明
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void Error_Handler(void);

int main(void)
{
  HAL_Init();
  SystemClock_Config();  // 系统时钟配置
  
  // 初始化外设
  MX_GPIO_Init();        // GPIO初始化
  MX_USART1_UART_Init(); // 串口初始化
  
  // 初始化AHT20传感器
  AHT20_Init();
  HAL_Delay(100);  // 等待传感器稳定

  uint32_t ct[2];  // 存储湿度和温度原始数据
  float humidity, temperature;

  while (1)
  {
    // 读取带CRC校验的温湿度数据
    AHT20_Read_CTdata_crc(ct);

    // 数据转换（根据AHT20数据手册公式）
    humidity = (ct[0] * 100.0) / (1 << 20);    // 湿度计算（范围0-100%RH）
    temperature = (ct[1] * 200.0) / (1 << 20) - 50;  // 温度计算（范围-40~85℃）

    // 通过串口打印数据
    printf("湿度: %.2f %%RH, 温度: %.2f °C\r\n", humidity, temperature);

    HAL_Delay(2000);  // 每2秒采集一次
  }
}

/**
  * 系统时钟配置
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  // 配置外部高速时钟HSE（8MHz）
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;  // 8MHz * 9 = 72MHz
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  // 配置系统时钟（SYSCLK=72MHz）
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;  // HCLK=72MHz
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;   // PCLK1=36MHz
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;   // PCLK2=72MHz
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * GPIO初始化函数
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // 使能GPIOB时钟（AHT20的I2C引脚接GPIOB）
  __HAL_RCC_GPIOB_CLK_ENABLE();
  // 使能GPIOA时钟（LED和串口引脚）
  __HAL_RCC_GPIOA_CLK_ENABLE();

  // 配置AHT20的I2C引脚（SCL=PB6，SDA=PB7）
  // 初始化为推挽输出，高电平（I2C空闲状态）
  GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6 | GPIO_PIN_7, GPIO_PIN_SET);

  // 配置LED引脚（PA0，可选）
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */
// printf重定向到USART1
int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}
/* USER CODE END 4 */

/**
  * 错误处理函数
  */
void Error_Handler(void)
{
  while(1)
  {
    // LED闪烁提示错误（PA0引脚接LED）
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_0);
    HAL_Delay(500);
  }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
  // 断言失败处理
}
#endif
