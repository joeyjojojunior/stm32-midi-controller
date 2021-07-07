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
#include "ssd1306.h"
#include "knob.h"
#include "preset.h"
#include "sd.h"
#include "midi.h"
#include "led.h"
#include "button.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define NUM_ADC_SAMPLES 32
#define NUM_ADC_CHANNELS 4
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
RTC_HandleTypeDef hrtc;
I2C_HandleTypeDef hi2c1;
SD_HandleTypeDef hsd;
/* USER CODE BEGIN PV */
Knob knobs[4];

char presetFilenames[NUM_KNOBS][_MAX_LFN + 1];
char presetNames[NUM_KNOBS][MAX_LABEL_CHARS + 1];

uint16_t adcAveraged[NUM_ADC_CHANNELS] = { 0 };
uint16_t adcAveragedPrev[NUM_KNOBS] = { 0 };
const uint32_t adcChannels[NUM_ADC_CHANNELS] = { ADC_CHANNEL_0, ADC_CHANNEL_1, ADC_CHANNEL_2, ADC_CHANNEL_3 };
const uint16_t AMUXPins[4] = { AMUX_S0_Pin, AMUX_S1_Pin, AMUX_S2_Pin, AMUX_S3_Pin };

bool isPresetsLoaded = false;
bool isPatchesLoaded = false;
bool isDisplayLocked = false;

uint8_t page = 0;

volatile bool btnDown[NUM_BUTTONS] = { false };
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_SDIO_SD_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */




void ADC_ReadKnobs();
void ADC_MuxSelect(uint8_t c);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
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
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 40);

    SD_LoadPreset("knobs1.json");

    // Init displays
    for (uint8_t i = 0; i < NUM_KNOBS; i++) {
        ssd1306_Init(&knobs[i]);
        ssd1306_WriteKnob(&knobs[i]);
    }

    State state = NORMAL;
    State *s = &state;

    while (1) {
        switch (*s) {
        case NORMAL:
            if (!isDisplayLocked) {
                ssd1306_WriteAllKnobs();
                isDisplayLocked = true;
            }

            for (uint8_t i = 0; i < NUM_BUTTONS - 1; i++) {
                if (Button_IsDown(i)) {
                    LED_Off(page);
                    page = i;
                    break;
                }
            }
            LED_AllOff();
            LED_On(page);
            if (Button_IsDown(BUTTON_MENU)) {
                *s = MENU;
            }
            break;
        case MENU:
            if (!isDisplayLocked) {
                ssd1306_WriteMainMenu();
                isDisplayLocked = true;
            }

            LED_AllOff();
            LED_On(BUTTON_MENU);

            if (Button_IsDown(BUTTON_MENU)) *s = NORMAL;
            else if (Button_IsDown(BUTTON_1)) *s = LOAD_PRESET;
            else if (Button_IsDown(BUTTON_2)) *s = LOAD_PATCH;
            else if (Button_IsDown(BUTTON_3)) *s = SAVE_PATCH;

            Button_Ignore(BUTTON_4);
            Button_Ignore(BUTTON_5);
            break;
        case LOAD_PRESET:
            SD_FetchPresetNames();
            if (!isDisplayLocked) {
                ssd1306_WritePresets();
                isDisplayLocked = true;
            }
            isPresetsLoaded = true;

            for (uint8_t i = 0; i < NUM_ADC_CHANNELS; i++) {
                adcAveragedPrev[i] = adcAveraged[i];
            }

            LED_On(BUTTON_1);

            if (Button_IsDown(BUTTON_MENU)) *s = MENU;

            Button_Ignore(BUTTON_1);
            Button_Ignore(BUTTON_2);
            Button_Ignore(BUTTON_3);

            break;
        case LOAD_PATCH:
            if (!isDisplayLocked) {
                ssd1306_FillAll(Black);
                isDisplayLocked = true;
            }
            isPatchesLoaded = true;

            LED_On(BUTTON_2);

            if (Button_IsDown(BUTTON_MENU)) *s = MENU;

            Button_Ignore(BUTTON_1);
            Button_Ignore(BUTTON_2);
            Button_Ignore(BUTTON_3);

            break;
        case SAVE_PATCH:
            if (!isDisplayLocked) {
                ssd1306_FillAll(Black);
                isDisplayLocked = true;
            }

            LED_On(BUTTON_3);

            if (Button_IsDown(BUTTON_MENU)) *s = MENU;

            Button_Ignore(BUTTON_1);
            Button_Ignore(BUTTON_2);
            Button_Ignore(BUTTON_3);

            break;
        }

        ADC_ReadKnobs();

        for (uint8_t i = 0; i < NUM_ADC_CHANNELS; i++) {
            if (*s != NORMAL) {
                uint16_t knobDiff = abs(adcAveraged[i] - adcAveragedPrev[i]);
                if (knobDiff > 5) {
                    bool loadComplete = false;
                    if (isPresetsLoaded) {
                        loadComplete = SD_LoadPreset(presetFilenames[i]);
                        isPresetsLoaded = false;
                    } else if (isPatchesLoaded) {
                        //SD_LoadPatch(presetFilenames[i]);
                        isPatchesLoaded = false;
                    }

                    if (loadComplete) {
                        *s = NORMAL;
                        isDisplayLocked = false;
                    }
                }
            } else {
                uint8_t curr_MIDI_val = MIDI_Scale_And_Filter(&knobs[i], adcAveraged[i]);
                if (curr_MIDI_val != knobs[i].value) {
                    knobs[i].value = curr_MIDI_val;
                    ssd1306_WriteKnob(&knobs[i]);

                    if (knobs[i].value == knobs[i].init_value) knobs[i].isLocked = false;
                    if (!knobs[i].isLocked) MIDI_Send(&knobs[i], knobs[i].value);
                }
            }

        }

    }

    for (uint8_t i = 0; i < NUM_KNOBS; i++) {
        Knob_Free(&knobs[i]);
    }

}

/* USER CODE END WHILE */

/* USER CODE BEGIN 3 */

/* USER CODE END 3 */

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
    hsd.Init.ClockDiv = 12;
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
void ADC_MuxSelect(uint8_t c) {
    if (c > NUM_ADC_CHANNELS) return;

    for (int i = 0; i < NUM_ADC_CHANNELS; i++) {
        if (c & (1 << i)) {
            HAL_GPIO_WritePin(GPIO_PORT_AMUX, AMUXPins[i], GPIO_PIN_SET);
        } else {
            HAL_GPIO_WritePin(GPIO_PORT_AMUX, AMUXPins[i], GPIO_PIN_RESET);
        }
    }
}

void ADC_ReadKnobs() {
    for (uint8_t channel = 0; channel < NUM_ADC_CHANNELS; channel++) {
        uint16_t adcBuf[NUM_ADC_SAMPLES];

        ADC_MuxSelect(channel);

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
