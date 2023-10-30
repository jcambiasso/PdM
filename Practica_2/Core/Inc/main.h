/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32f4xx_hal.h"
#include <stdbool.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

typedef uint32_t tick_t;
typedef bool bool_t;

/**
 * @brief Struct para un delay no bloqueante.
 */
typedef struct{
   tick_t startTime;		/*!< Instante, en milisengundos del sistema, en que inició el delay  */
   tick_t duration;			/*!< Duración, en milisegundos del sistema, desde @ref startTime   */
   bool_t running;			/*!< Indica si el delay está activado */
   bool_t enabled;			/*!< Indica si el delay está habilitado */
   bool_t singleShot;		/*!< Delays en singleShot no vuelven a contar hasta ejecutarse un @ref delayStart  */
} delay_t;

/**
 * @brief 	Prototipo de función que obtiene un tiempo en milisegundos.
 * @note 	Esto es modularizable, y es raro hardcodear una función muy target-specific (te estoy mirando HAL_GetTick())
 * 			a una estructura tan genérica. Declaro un prototipo de la función que será referenciada en estos métodos.
 * 			(es un puntero a una función de acepta void y devuelve uint32_t)
 */
typedef uint32_t (*GetCurrentTick_t)(void);

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
/**
 * Declaro la función del tipo @ref GetCurrentTick_t específica que será usada en las funciones de delay, la cual debería
 * obtener un tiempo transcurrido en milisegundos. Es extern así es obligatorio definirla al menos una vez en código de usuario.
 * Es muy parecido a usar __weak, con la diferencia que esta función puede ser cambiada en runtime, y el override de __weak es en linktime.
 */
extern GetCurrentTick_t getCurrentTick;
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/**
 * Una pequeño macro para inicializar un delay
 */
#define NewDelay(in,ss)\
{\
	.duration = in, \
	.start = 0, \
	.enabled = 1, \
	.singleShot = ss, \
	.running = 0 \
}

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
/* USER CODE BEGIN EFP */

/**
  * @brief  Inicializa un delay no bloqueante.
  * @param delay Puntero a estrucutra delay_t
  * @param duration Duration del delay
  * @param singleShot Determina si el delay es de disparo único
  * @retval None
  */
void delayInit( delay_t * delay, tick_t duration, bool_t singleShot );

/**
  * @brief  La primer llamada inicia el timer, y las subsiguientes verifica que Duration haya transcurrido.
  * @param delay Puntero a estrucutra delay_t
  * @retval true Transcurrió el tiempo establecido
  * @retval false No transcurrió el tiempo establecido
  */
bool_t delayRead( delay_t * delay );

/**
  * @brief  Permite reescribir la duración del delay.
  * @param delay Puntero a estrucutra delay_t
  * @param duration Nueva duración del delay
  * @param restart True para además reiniciar el delay con la nueva duración
  * @retval True si transcurrió el tiempo establecido, false si lo contrario.
  */
void delayWrite( delay_t * delay, tick_t duration, bool_t restart);

/**
  * @brief Habilita el delay
  * @param delay Puntero a estrucutra delay_t
  * @retval None
  */
void delayStart(delay_t * delay);

/**
  * @brief Deshbilita el delay.
  * @param delay Puntero a estrucutra delay_t
  * @retval None
  */
void delatStop(delay_t * delay);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define USER_Btn_Pin GPIO_PIN_13
#define USER_Btn_GPIO_Port GPIOC
#define RMII_MDC_Pin GPIO_PIN_1
#define RMII_MDC_GPIO_Port GPIOC
#define RMII_REF_CLK_Pin GPIO_PIN_1
#define RMII_REF_CLK_GPIO_Port GPIOA
#define RMII_MDIO_Pin GPIO_PIN_2
#define RMII_MDIO_GPIO_Port GPIOA
#define RMII_CRS_DV_Pin GPIO_PIN_7
#define RMII_CRS_DV_GPIO_Port GPIOA
#define RMII_RXD0_Pin GPIO_PIN_4
#define RMII_RXD0_GPIO_Port GPIOC
#define RMII_RXD1_Pin GPIO_PIN_5
#define RMII_RXD1_GPIO_Port GPIOC
#define LD1_Pin GPIO_PIN_0
#define LD1_GPIO_Port GPIOB
#define RMII_TXD1_Pin GPIO_PIN_13
#define RMII_TXD1_GPIO_Port GPIOB
#define LD3_Pin GPIO_PIN_14
#define LD3_GPIO_Port GPIOB
#define STLK_RX_Pin GPIO_PIN_8
#define STLK_RX_GPIO_Port GPIOD
#define STLK_TX_Pin GPIO_PIN_9
#define STLK_TX_GPIO_Port GPIOD
#define USB_PowerSwitchOn_Pin GPIO_PIN_6
#define USB_PowerSwitchOn_GPIO_Port GPIOG
#define USB_OverCurrent_Pin GPIO_PIN_7
#define USB_OverCurrent_GPIO_Port GPIOG
#define USB_SOF_Pin GPIO_PIN_8
#define USB_SOF_GPIO_Port GPIOA
#define USB_VBUS_Pin GPIO_PIN_9
#define USB_VBUS_GPIO_Port GPIOA
#define USB_ID_Pin GPIO_PIN_10
#define USB_ID_GPIO_Port GPIOA
#define USB_DM_Pin GPIO_PIN_11
#define USB_DM_GPIO_Port GPIOA
#define USB_DP_Pin GPIO_PIN_12
#define USB_DP_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define RMII_TX_EN_Pin GPIO_PIN_11
#define RMII_TX_EN_GPIO_Port GPIOG
#define RMII_TXD0_Pin GPIO_PIN_13
#define RMII_TXD0_GPIO_Port GPIOG
#define LD2_Pin GPIO_PIN_7
#define LD2_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
