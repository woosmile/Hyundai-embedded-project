/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adc_if.h
  * @author  MCD Application Team
  * @brief   Header for ADC interface configuration
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ADC_IF_H__
#define __ADC_IF_H__

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */
#include "stm32l0xx_hal.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/**
  * @brief Battery level in mV
  */
#define BAT_CR2032                  ((uint32_t) 3000)
/**
  * @brief Maximum battery level in mV
  */
#define VDD_BAT                     BAT_CR2032
/**
  * @brief Minimum battery level in mV
  */
#define VDD_MIN                     1800

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/

/**
  * @brief  Initializes the ADC input
  */
void SYS_InitMeasurement(ADC_HandleTypeDef *h_adc);

/**
  * @brief DeInitializes the ADC
  */
void SYS_DeInitMeasurement(void);

/**
  * @brief This function reads the ADC channel
  * @param channel channel number to read
  * @return adc measured level value
  */
uint32_t ADC_ReadChannels(uint32_t channel);

/**
  * @brief  Get the current temperature
  * @return value temperature in degree Celsius( q7.8 )
  */
int16_t SYS_GetTemperatureLevel(void);

/**
  * @brief Get the current battery level
  * @return value battery level in linear scale
  */
uint16_t SYS_GetBatteryLevel(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /* __ADC_IF_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
