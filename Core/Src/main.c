/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdbool.h>
#include "ssd1306.h"
#include "knob.h"
#include "preset.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define GPIO_PORT_AMUX GPIOB
#define NUM_KNOBS 4
#define NUM_ADC_SAMPLES 32
#define NUM_ADC_CHANNELS 4
#define EMA_A 0.5
#define UPPER_BOUND_ADC 250
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

SD_HandleTypeDef hsd;

/* USER CODE BEGIN PV */
uint16_t adcAveraged[4] = { 0 };
uint32_t adcChannels[4] = { ADC_CHANNEL_0, ADC_CHANNEL_1, ADC_CHANNEL_2, ADC_CHANNEL_3 };
const uint16_t AMUXPins[4] = { AMUX_S0_Pin, AMUX_S1_Pin, AMUX_S2_Pin, AMUX_S3_Pin };
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_SDIO_SD_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */
void SD_Toggle();
void SD_Enable();
void SD_Disable();
void ADC_Read_Knobs();
void ADC_Mux_Select(uint8_t c);
void MIDI_Send(Knob *k, uint8_t value);
uint8_t MIDI_Scale_And_Filter(Knob *k, uint8_t adc_value);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// Polls each channel NUM_ADC_SAMPLES times and saves the average ADC reading
void SD_Toggle() {
    hsd.State != HAL_SD_STATE_READY ? SD_Enable() : SD_Disable();
}

void SD_Enable() {
    __HAL_SD_ENABLE(hsd);
    hsd.State = HAL_SD_STATE_READY;
}

void SD_Disable() {
    __HAL_SD_DISABLE(hsd);
    hsd.State = HAL_SD_STATE_RESET;
}

void ADC_Mux_Select(uint8_t c) {
    if (c > NUM_ADC_CHANNELS) return;

    for (int i = 0; i < NUM_ADC_CHANNELS; i++) {
        if (c & (1 << i)) {
            HAL_GPIO_WritePin(GPIO_PORT_AMUX, AMUXPins[i], GPIO_PIN_SET);
        } else {
            HAL_GPIO_WritePin(GPIO_PORT_AMUX, AMUXPins[i], GPIO_PIN_RESET);
        }
    }
}

void ADC_Read_Knobs() {
    for (uint8_t channel = 0; channel < NUM_ADC_CHANNELS; channel++) {
        uint16_t adcBuf[NUM_ADC_SAMPLES];

        ADC_Mux_Select(channel);

        // Select channel
        ADC_ChannelConfTypeDef sConfig = { 0 };
        sConfig.Channel = adcChannels[channel];
        sConfig.Rank = 1;
        sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
        if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
            Error_Handler();
        }

        // Sample the channel NUM_ADC_SAMPLES times to the buffer
        HAL_ADC_Start(&hadc1);
        for (uint8_t i = 0; i < NUM_ADC_SAMPLES; i++) {
            HAL_ADC_PollForConversion(&hadc1, 1000);
            adcBuf[i] = HAL_ADC_GetValue(&hadc1);
        }
        HAL_ADC_Stop(&hadc1);

        // Calculate average of all samples for the channel
        uint16_t adc_sum = 0;
        for (uint8_t i = 0; i < NUM_ADC_SAMPLES; i++) {
            adc_sum += adcBuf[i];
        }

        adcAveraged[channel] = adc_sum / NUM_ADC_SAMPLES;
    }
}

// Sends a CC message for knob k with the specified value
void MIDI_Send(Knob *k, uint8_t value) {
    MX_USB_Send_Midi(k->channel, k->cc, KnobMap(k, value, k->max_range));
}

// Scales an ADC value from (0, 255) to (0, k->max_values) an applies EMA filter
uint8_t MIDI_Scale_And_Filter(Knob *k, uint8_t adc_value) {
    float midi_scale_factor = 1.0 * k->max_values / UPPER_BOUND_ADC;
    return MIN(EMA_A * midi_scale_factor * adc_value + (1 - EMA_A) * k->value, k->max_range);
}
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
    MX_ADC1_Init();
    MX_I2C1_Init();
    MX_USB_DEVICE_Init();
    MX_SDIO_SD_Init();
    MX_RTC_Init();
    MX_FATFS_Init();
    /* USER CODE BEGIN 2 */

    Knob knobs[4];

    retSD = f_mount(&SDFatFS, "", 1);
    retSD = f_open(&SDFile, "knobs.json", FA_READ);

    char readBuf[f_size(&SDFile) + 1];
    unsigned int bytesRead;

    retSD = f_read(&SDFile, readBuf, sizeof(readBuf) - 1, &bytesRead);
    readBuf[bytesRead] = '\0';

    Preset_Load(knobs, readBuf);
    retSD = f_close(&SDFile);
    retSD = f_mount(NULL, "", 0);

    SD_Disable();
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 40);

    // Init displays
    for (uint8_t i = 0; i < NUM_KNOBS; i++) {
        ssd1306_Init(&hi2c1, &knobs[i]);
        ssd1306_WriteKnob(&hi2c1, &knobs[i]);
    }

    while (1) {
        ADC_Read_Knobs();

        for (uint8_t i = 0; i < NUM_ADC_CHANNELS; i++) {
            uint8_t curr_MIDI_val = MIDI_Scale_And_Filter(&knobs[i], adcAveraged[i]);

            if (curr_MIDI_val != knobs[i].value) {
                knobs[i].value = curr_MIDI_val;
                ssd1306_WriteKnob(&hi2c1, &knobs[i]);
                if (knobs[i].value == knobs[i].init_value) knobs[i].isLocked = false;
                if (!knobs[i].isLocked) MIDI_Send(&knobs[i], knobs[i].value);
            }
        }

        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }

    for (uint8_t i = 0; i < NUM_ADC_CHANNELS; i++)
        free(knobs[i].sub_labels);
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
    RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = { 0 };

    /** Configure the main internal regulator output voltage
     */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 25;
    RCC_OscInitStruct.PLL.PLLN = 192;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 4;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
            {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
            | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
            {
        Error_Handler();
    }
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
            {
        Error_Handler();
    }
}

/**
 * @brief ADC1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_ADC1_Init(void)
{

    /* USER CODE BEGIN ADC1_Init 0 */

    /* USER CODE END ADC1_Init 0 */

    ADC_ChannelConfTypeDef sConfig = { 0 };

    /* USER CODE BEGIN ADC1_Init 1 */

    /* USER CODE END ADC1_Init 1 */
    /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
     */
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc1.Init.Resolution = ADC_RESOLUTION_8B;
    hadc1.Init.ScanConvMode = ENABLE;
    hadc1.Init.ContinuousConvMode = ENABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
    if (HAL_ADC_Init(&hadc1) != HAL_OK)
            {
        Error_Handler();
    }
    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
     */
    sConfig.Channel = ADC_CHANNEL_0;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
            {
        Error_Handler();
    }
    /* USER CODE BEGIN ADC1_Init 2 */

    /* USER CODE END ADC1_Init 2 */

}

/**
 * @brief I2C1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C1_Init(void)
{

    /* USER CODE BEGIN I2C1_Init 0 */

    /* USER CODE END I2C1_Init 0 */

    /* USER CODE BEGIN I2C1_Init 1 */

    /* USER CODE END I2C1_Init 1 */
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 800000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
            {
        Error_Handler();
    }
    /* USER CODE BEGIN I2C1_Init 2 */

    /* USER CODE END I2C1_Init 2 */

}

/**
 * @brief RTC Initialization Function
 * @param None
 * @retval None
 */
static void MX_RTC_Init(void)
{

    /* USER CODE BEGIN RTC_Init 0 */

    /* USER CODE END RTC_Init 0 */

    /* USER CODE BEGIN RTC_Init 1 */

    /* USER CODE END RTC_Init 1 */
    /** Initialize RTC Only
     */
    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = 127;
    hrtc.Init.SynchPrediv = 255;
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    if (HAL_RTC_Init(&hrtc) != HAL_OK)
            {
        Error_Handler();
    }
    /* USER CODE BEGIN RTC_Init 2 */

    /* USER CODE END RTC_Init 2 */

}

/**
 * @brief SDIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_SDIO_SD_Init(void)
{

    /* USER CODE BEGIN SDIO_Init 0 */

    /* USER CODE END SDIO_Init 0 */

    /* USER CODE BEGIN SDIO_Init 1 */

    /* USER CODE END SDIO_Init 1 */
    hsd.Instance = SDIO;
    hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
    hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
    hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
    hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
    hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
    hsd.Init.ClockDiv = 8;
    /* USER CODE BEGIN SDIO_Init 2 */

    /* USER CODE END SDIO_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = { 0 };

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, LED_1_Pin | LED_2_Pin | LED_3_Pin | LED_4_Pin
            | LED_5_Pin | LED_6_Pin | AMUX_S1_Pin | AMUX_S0_Pin
            | AMUX_S2_Pin | AMUX_S3_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin : Detect_SDIO_Pin */
    GPIO_InitStruct.Pin = Detect_SDIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(Detect_SDIO_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pin : Button_6_Pin */
    GPIO_InitStruct.Pin = Button_6_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(Button_6_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pins : Button_1_Pin Button_2_Pin Button_3_Pin Button_4_Pin
     Button_5_Pin */
    GPIO_InitStruct.Pin = Button_1_Pin | Button_2_Pin | Button_3_Pin | Button_4_Pin
            | Button_5_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pins : LED_1_Pin LED_2_Pin LED_3_Pin LED_4_Pin
     LED_5_Pin LED_6_Pin AMUX_S1_Pin AMUX_S0_Pin
     AMUX_S2_Pin AMUX_S3_Pin */
    GPIO_InitStruct.Pin = LED_1_Pin | LED_2_Pin | LED_3_Pin | LED_4_Pin
            | LED_5_Pin | LED_6_Pin | AMUX_S1_Pin | AMUX_S0_Pin
            | AMUX_S2_Pin | AMUX_S3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
/*
 Knob knobs[4] = { { .init_value = 63, .row = 0, .col = 0, .label = "Cutoff", .channel = 0, .cc = 17, .value = 0, .max_values = 128, .max_range = 127, .isLocked = 1 },
 { .init_value = 127, .row = 0, .col = 1, .label = "Resonance", .channel = 1, .cc = 18, .value = 0, .max_values = 128, .max_range = 127, .isLocked = 0 },
 { .init_value = 5, .row = 1, .col = 0, .label = "Osc 0", .channel = 2, .cc = 19, .value = 0, .max_values = 12, .max_range = 127, .isLocked = 1 },
 { .init_value = 11, .row = 1, .col = 1, .label = "Osc 1", .channel = 3, .cc = 20, .value = 0, .max_values = 12, .max_range = 11, .isLocked = 1 } };

 for (int i = 2; i < 4; i++) {
 knobs[i].sub_labels = malloc(sizeof(*knobs[i].sub_labels) * (knobs[i].max_values));
 strncpy(knobs[i].sub_labels[0], "MultiSaw", MAX_LABEL_CHARS);
 strncpy(knobs[i].sub_labels[1], "TriWrap", MAX_LABEL_CHARS);
 strncpy(knobs[i].sub_labels[2], "Noise", MAX_LABEL_CHARS);
 strncpy(knobs[i].sub_labels[3], "Feedback", MAX_LABEL_CHARS);
 strncpy(knobs[i].sub_labels[4], "Pulse", MAX_LABEL_CHARS);
 strncpy(knobs[i].sub_labels[5], "Saw", MAX_LABEL_CHARS);
 strncpy(knobs[i].sub_labels[6], "Triangle", MAX_LABEL_CHARS);
 strncpy(knobs[i].sub_labels[7], "Pulse5", MAX_LABEL_CHARS);
 strncpy(knobs[i].sub_labels[8], "Pulse6", MAX_LABEL_CHARS);
 strncpy(knobs[i].sub_labels[9], "Pulse7", MAX_LABEL_CHARS);
 strncpy(knobs[i].sub_labels[10], "Pulse8", MAX_LABEL_CHARS);
 strncpy(knobs[i].sub_labels[11], "Pulse9", MAX_LABEL_CHARS);
 }

 knobs[0].sub_labels = malloc(sizeof(*knobs[0].sub_labels));
 strncpy(knobs[0].sub_labels[0], "Filter 1", MAX_LABEL_CHARS);

 knobs[1].sub_labels = malloc(sizeof(*knobs[1].sub_labels));
 strncpy(knobs[1].sub_labels[0], "Filter 2", MAX_LABEL_CHARS);
 */
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
    while (1) {
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
