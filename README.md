# ASU Racing Team — Low Voltage Solo Mission

This repository contains the firmware source code, architecture designs, and hardware simulation files for the **Low Voltage (LV) Subteam Selection Phase 2**.

---

## 📂 Repository Structure

```text
.
├── PCB/
│   ├── Schematics/
│   ├── Layout/
│   ├── Libraries/
│   └── Gerbers/
│
├── STM32_Project/
│   ├── Milestone_1_LED_Blink/
│   │   ├── Core/Src/main.c
│   │   └── Simulation/
│   │
│   ├── Milestone_2_ADC_UART/
│   │   ├── Core/Src/main.c
│   │   └── Simulation/
│   │
│   └── Milestone_3_SPI_Telemetry/
│       ├── Master_MCU/
│       │   └── Core/Src/main.c
│       ├── Slave_MCU/
│       │   └── Core/Src/main.c
│       └── Simulation/
│
└── README.md
```

---

## 🚀 STM32 Firmware Projects

### Milestone 1: LED Blink & Toolchain Validation
* **Target MCU:** STM32F103C8T6 (ARM Cortex-M3 @ 72 MHz)
* **Pin Configuration:** `PC13` configured as GPIO Output.
* **Operation:** Toggles the onboard LED at 1 Hz (500 ms delay).

---

### Milestone 2: Multi-Channel ADC Acquisition with UART Telemetry
* **Analog Inputs:**
  * `PA0` (ADC1_IN0): Engine Temperature Sensor ($0 - 150^\circ\text{C}$)
  * `PA1` (ADC1_IN1): Throttle Position Sensor ($0 - 100\%$)
* **Telemetry Output:** USART1 @ 9600 Baud (8-N-1) transmitting live formatted strings:
  ```text
  Engine Temp = 45.0 C | Throttle = 72.0 %
  ```

---

### Milestone 3: Dual-MCU SPI Telemetry Network (Master-Slave)
* **Architecture:** Master MCU queries Slave MCU over SPI with 2-cycle synchronization to read packed telemetry frames.
* **SPI Configuration:** Full-Duplex, 8-bit Data Size, MSB First, Software/Hardware NSS management.
* **24-Bit Frame Bit-Packing:**
  * Bits [23:12] (12 bits): Battery Voltage ($0 - 24\text{V}$, scaled to $0 - 4095$)
  * Bits [11:0] (12 bits): Wheel Speed ($0 - 300\text{ km/h}$, scaled to $0 - 4095$)
* **Data Cases:**
  * `0x1001` $\to$ Voltage: 12.3 V, Speed: 200 km/h
  * `0x1002` $\to$ Voltage: 14.2 V, Speed: 150 km/h
  * `0x1003` $\to$ Voltage: 13.8 V, Speed: 100 km/h
  * `0x1004` $\to$ Voltage: 11.5 V, Speed: 220 km/h
  * `0x1005` $\to$ Voltage: 10.9 V, Speed: 280 km/h

---

## 📄 Documentation & HAL References
All API calls are strictly mapped to the official STMicroelectronics HAL manual (*UM1850*):
* `HAL_GPIO_TogglePin`: Section 16.2.3, Page 242
* `HAL_ADC_PollForConversion` & `HAL_ADC_GetValue`: Section 8.2.2, Pages 105–109
* `HAL_UART_Transmit` & `HAL_UART_Receive`: Section 53.2.2, Page 785
* `HAL_SPI_TransmitReceive`: Section 47.2.2, Page 705
