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
#include "stm32l4xx_hal.h"

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
#define uSD_Detect_Pin GPIO_PIN_12
#define uSD_Detect_GPIO_Port GPIOB
#define uSD_SCK_Pin GPIO_PIN_10
#define uSD_SCK_GPIO_Port GPIOC
#define uSD_MISO_Pin GPIO_PIN_11
#define uSD_MISO_GPIO_Port GPIOC
#define uSD_MOSI_Pin GPIO_PIN_12
#define uSD_MOSI_GPIO_Port GPIOC
#define uSD_CS_Pin GPIO_PIN_2
#define uSD_CS_GPIO_Port GPIOD
#define LED_2_Pin GPIO_PIN_8
#define LED_2_GPIO_Port GPIOB
#define LED_1_Pin GPIO_PIN_9
#define LED_1_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define SD_SPI_HANDLE   hspi3
#define SD_CS_GPIO_Port uSD_CS_GPIO_Port
#define SD_CS_Pin       uSD_CS_Pin
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
