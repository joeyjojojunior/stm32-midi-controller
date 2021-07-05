/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Detect_SDIO_Pin GPIO_PIN_14
#define Detect_SDIO_GPIO_Port GPIOC
#define Button_6_Pin GPIO_PIN_15
#define Button_6_GPIO_Port GPIOC
#define Button_1_Pin GPIO_PIN_4
#define Button_1_GPIO_Port GPIOA
#define Button_2_Pin GPIO_PIN_5
#define Button_2_GPIO_Port GPIOA
#define Button_3_Pin GPIO_PIN_7
#define Button_3_GPIO_Port GPIOA
#define LED_1_Pin GPIO_PIN_0
#define LED_1_GPIO_Port GPIOB
#define LED_2_Pin GPIO_PIN_1
#define LED_2_GPIO_Port GPIOB
#define LED_3_Pin GPIO_PIN_2
#define LED_3_GPIO_Port GPIOB
#define LED_4_Pin GPIO_PIN_10
#define LED_4_GPIO_Port GPIOB
#define LED_5_Pin GPIO_PIN_12
#define LED_5_GPIO_Port GPIOB
#define LED_6_Pin GPIO_PIN_13
#define LED_6_GPIO_Port GPIOB
#define AMUX_S1_Pin GPIO_PIN_14
#define AMUX_S1_GPIO_Port GPIOB
#define Button_4_Pin GPIO_PIN_10
#define Button_4_GPIO_Port GPIOA
#define USB_OTG_FS_DM_Pin GPIO_PIN_11
#define USB_OTG_FS_DM_GPIO_Port GPIOA
#define USB_OTG_FS_DP_Pin GPIO_PIN_12
#define USB_OTG_FS_DP_GPIO_Port GPIOA
#define Button_5_Pin GPIO_PIN_15
#define Button_5_GPIO_Port GPIOA
#define AMUX_S0_Pin GPIO_PIN_3
#define AMUX_S0_GPIO_Port GPIOB
#define AMUX_S2_Pin GPIO_PIN_8
#define AMUX_S2_GPIO_Port GPIOB
#define AMUX_S3_Pin GPIO_PIN_9
#define AMUX_S3_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
