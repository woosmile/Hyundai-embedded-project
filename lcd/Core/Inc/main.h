/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include <string.h>
#include <stdio.h>
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
#define MCO_Pin GPIO_PIN_0
#define MCO_GPIO_Port GPIOH
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define LCD1602_D6_Pin GPIO_PIN_10
#define LCD1602_D6_GPIO_Port GPIOB
#define LCD1602_E_Pin GPIO_PIN_7
#define LCD1602_E_GPIO_Port GPIOC
#define LCD1602_D7_Pin GPIO_PIN_8
#define LCD1602_D7_GPIO_Port GPIOA
#define LCD1602_RS_Pin GPIO_PIN_9
#define LCD1602_RS_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define LCD1602_D5_Pin GPIO_PIN_4
#define LCD1602_D5_GPIO_Port GPIOB
#define LCD1602_D4_Pin GPIO_PIN_5
#define LCD1602_D4_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

// LCD : RS_E_PORT, D8, D9, DATA_PORT, D4, D5, D6, D7
#define RS_PORT GPIOA
#define E_PORT GPIOC
#define RS GPIO_PIN_9
#define E GPIO_PIN_7

#define D4_5_6_PORT GPIOB
#define D7_PORT GPIOA
#define D4 GPIO_PIN_5
#define D5 GPIO_PIN_4
#define D6 GPIO_PIN_10
#define D7 GPIO_PIN_8

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
