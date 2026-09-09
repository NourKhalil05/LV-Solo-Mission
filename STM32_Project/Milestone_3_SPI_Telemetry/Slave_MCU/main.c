#include "main.h"

SPI_HandleTypeDef hspi1;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);

typedef struct {
    uint16_t case_id;
    float voltage;
    float speed;
} TelemetryCase;

TelemetryCase telemetry_db[5] = {
    {0x1001, 12.3f, 200.0f},
    {0x1002, 14.2f, 150.0f},
    {0x1003, 13.8f, 100.0f},
    {0x1004, 11.5f, 220.0f},
    {0x1005, 10.9f, 280.0f}
};

uint8_t slave_rx[3];
uint8_t slave_tx[3] = {0, 0, 0};

void Encode_Telemetry_24Bit(float volt, float speed, uint8_t *out_buf)
{
    uint16_t raw_v = (uint16_t)((volt / 24.0f) * 4095.0f);
    uint16_t raw_s = (uint16_t)((speed / 300.0f) * 4095.0f);

    uint32_t packed = ((uint32_t)(raw_v & 0xFFF) << 12) | (raw_s & 0xFFF);

    out_buf[0] = (uint8_t)((packed >> 16) & 0xFF);
    out_buf[1] = (uint8_t)((packed >> 8) & 0xFF);
    out_buf[2] = (uint8_t)(packed & 0xFF);
}

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_SPI1_Init();

  hspi1.Init.NSS = SPI_NSS_HARD_INPUT;
  HAL_SPI_Init(&hspi1);

  while (1)
  {
    HAL_SPI_TransmitReceive(&hspi1, slave_tx, slave_rx, 3, HAL_MAX_DELAY);

    uint16_t received_id = ((uint16_t)slave_rx[0] << 8) | slave_rx[1];

    for (int i = 0; i < 5; i++)
    {
        if (telemetry_db[i].case_id == received_id)
        {
            Encode_Telemetry_24Bit(telemetry_db[i].voltage, telemetry_db[i].speed, slave_tx);
            break;
        }
    }
  }
}

static void MX_SPI1_Init(void)
{
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_SLAVE;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_HARD_INPUT;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  HAL_SPI_Init(&hspi1);
}

static void MX_GPIO_Init(void)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();
}

void SystemClock_Config(void)
{
}
