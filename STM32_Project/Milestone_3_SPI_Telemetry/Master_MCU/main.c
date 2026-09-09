#include "main.h"
#include <stdio.h>
#include <string.h>

SPI_HandleTypeDef hspi1;
UART_HandleTypeDef huart1;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);

char uart_rx_char;
char terminal_buf[128];
uint8_t spi_tx[3];
uint8_t spi_rx[3];

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);

  char welcome[] = "\r\n=========================================\r\n"
                   "  STM32 Automotive SPI Telemetry Master  \r\n"
                   "=========================================\r\n";
  HAL_UART_Transmit(&huart1, (uint8_t*)welcome, strlen(welcome), HAL_MAX_DELAY);

  while (1)
  {
    char prompt[] = "\r\nEnter Case ID (1-5): ";
    HAL_UART_Transmit(&huart1, (uint8_t*)prompt, strlen(prompt), HAL_MAX_DELAY);

    HAL_UART_Receive(&huart1, (uint8_t*)&uart_rx_char, 1, HAL_MAX_DELAY);
    HAL_UART_Transmit(&huart1, (uint8_t*)&uart_rx_char, 1, 100);

    if (uart_rx_char >= '1' && uart_rx_char <= '5')
    {
        uint16_t case_id = 0x1000 + (uart_rx_char - '0');

        spi_tx[0] = (case_id >> 8) & 0xFF;
        spi_tx[1] = case_id & 0xFF;
        spi_tx[2] = 0x00;

        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
        HAL_Delay(1);
        HAL_SPI_TransmitReceive(&hspi1, spi_tx, spi_rx, 3, 1000);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);

        HAL_Delay(10);

        spi_tx[0] = 0xFF; spi_tx[1] = 0xFF; spi_tx[2] = 0xFF;
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
        HAL_Delay(1);
        HAL_SPI_TransmitReceive(&hspi1, spi_tx, spi_rx, 3, 1000);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);

        uint32_t payload = ((uint32_t)spi_rx[0] << 16) | ((uint32_t)spi_rx[1] << 8) | spi_rx[2];
        uint16_t raw_volt = (payload >> 12) & 0xFFF;
        uint16_t raw_speed = payload & 0xFFF;

        float volt = ((float)raw_volt / 4095.0f) * 24.0f;
        float speed = ((float)raw_speed / 4095.0f) * 300.0f;

        sprintf(terminal_buf, "\r\n[Case 0x%04X] -> Battery Voltage: %.1f V | Wheel Speed: %.0f km/h\r\n", 
                case_id, volt, speed);
        HAL_UART_Transmit(&huart1, (uint8_t*)terminal_buf, strlen(terminal_buf), HAL_MAX_DELAY);
    }
    else
    {
        char err[] = "\r\n[Error] Invalid Input! Please enter a digit between 1 and 5.\r\n";
        HAL_UART_Transmit(&huart1, (uint8_t*)err, strlen(err), 1000);
    }
  }
}

static void MX_SPI1_Init(void)
{
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  HAL_SPI_Init(&hspi1);
}

static void MX_USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  HAL_UART_Init(&huart1);
}

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void SystemClock_Config(void)
{
}
